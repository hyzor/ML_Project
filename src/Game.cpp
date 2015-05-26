#include "Game.h"

#include "MyGA.h"
#include "MyGA2.h"

Game::Game()
{
	mB2VelIterations = 6;
	mB2PosIterations = 2;
}


Game::Game(SDL_Window* sdlWindow, SDL_Renderer* sdlRenderer, unsigned int screenW, unsigned int screenH,
	b2World* _b2World, TextureManager* textureManager, TTF_Font* mainFont)
	: Game()
{
	mSDL_Window = sdlWindow;
	mSDL_Renderer = sdlRenderer;
	mScreenWidth = screenW;
	mScreenHeight = screenH;

	mB2World = _b2World;
	mMainFont = mainFont;
	mTextureManager = textureManager;
}

Game::~Game()
{
	if (mGaPop)
	{
		mGaPop->destroy();
		delete mGaPop;
		mGaPop = nullptr;
	}

	if (mGA)
	{
		delete mGA;
		mGA = nullptr;
	}

	//mWorld->Clear();
	World::Destroy();
	mWorld = nullptr;
	mTextureManager = nullptr;
	//TextureManager::Destroy();
}

bool Game::Init(std::string assetsDir, std::string fontsDir, std::string texturesDir, double dt_fixed)
{
	// World
	mWorld = World::GetInstance();
	mWorld->Init(mB2World, mScreenWidth, mScreenHeight);

	mBackground = mTextureManager->LoadTexture(mAssetsDir + "Background_stars_800x600.png");

	Ship* enemyShip = mWorld->SpawnEntity<Ship>(mScreenWidth - (48/2), mScreenHeight - (64/2), 48,
		64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));
	enemyShip->Init(Ship::Type::STATIONARY);
	enemyShip->Init_b2(mWorld->Getb2World(), false, Entity::Type::SHIP, 0.0f);

	Entity* obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.25f, mScreenHeight *0.25f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.5f , mScreenHeight *0.5f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.75f, mScreenHeight *0.75f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	GARealAlleleSetArray setArray;
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);
	setArray.add(0.1f, 0.9f);

	// GA population
	mGaPop = new GAPopulation();

	mGaPop->initialize();

	int popSize = 50;

	MyGenome* myGenome1;
		for (int i = 0; i < popSize; i++){
			myGenome1 = new MyGenome(1, setArray, 48.0f*0.5f, 64.0f*0.5f, 48, 64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"), MyGenome::Evaluate);
			mGaPop->add(myGenome1);
		}

	for (int i = 0; i < mGaPop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&mGaPop->individual(i);
		myGenome->initialize();
	}

	// GA
	//mGA = new MyGA(*mGaPop);
	mGA = new MyGA2(*mGaPop);
	mGA->crossover(MyGenome::OnePointCrossover);
	mGA->nGenerations(2);
	mGA->pMutation(0.15f);
	mGA->pCrossover(0.95f);
	mGA->scoreFilename("GA_score.dat");
	mGA->scoreFrequency(1);
	mGA->flushFrequency(1);
	mGA->selectScores(GAStatistics::AllScores);
	mGA->recordDiversity(gaTrue);
	
	mGA->Init(this, enemyShip);

	mGA->elitist(gaTrue); // if GASimpleGA
	//mGA->nReplacement(popSize - 1); // if GASteadyStateGA

	std::cout << "Initial genomes:\n";
	for (int i = 0; i < mGA->populationSize(); ++i)
	{
		MyGenome* genome = (MyGenome*)&mGA->population().individual(i);

		MyGenome::Init(*genome);
		genome->Init_b2(GetWorld()->Getb2World(), false, Entity::Type::SHIP, genome->gene(genome->length()-1));
		genome->Init_SDL();
		genome->SetType(Entity::Type::SHIP);
		genome->Reset();

		genome->SetCollisionEnabled(false);
		((Ship*)genome)->Reset();
		genome->Setb2BodyType(b2_staticBody);

		std::cout << genome->GetID() << ": ";

		for (int j = 0; j < genome->length(); ++j)
		{
			std::cout << genome->gene(j) << ", ";
		}

		std::cout << " Score: " << genome->score() << "\n";
	}

	MyGenome* testGenome = (MyGenome*)&mGA->population().individual(0);

	testGenome->Setb2BodyType(b2_dynamicBody);
	testGenome->SetCollisionEnabled(true);
	
	float speedup = 10.0f;

	// Evolve by explicitly calling the GA step function
	while (!mGA->done())
	{
		mGA->step(dt_fixed, speedup);
	}

	std::cout << "\n-----------------------\n\n";

	std::cout << "Genomes: \n";
	for (int i = 0; i < mGA->populationSize(); ++i)
	{
		MyGenome* genome = (MyGenome*)&mGA->population().individual(i);
		std::cout << genome->GetID() << ": ";

		for (int j = 0; j < genome->length(); ++j)
		{
			std::cout << genome->gene(j) << ", ";
		}

		std::cout << " Score: " << genome->score() << "\n";
	}

	std::cout << "\n";

	// Dump the results of the GA to the screen.
	MyGenome* genome = (MyGenome*)&mGA->statistics().bestIndividual();

	for (int i = 0; i < genome->length(); ++i)
	{
		std::cout << genome->gene(i) << "\n";
	}

	std::cout << "The best individual is genome " << genome->GetID() << "\n";
	std::cout << "Best of generation data is in file '" << mGA->scoreFilename() << "'\n";

	// Dump statistics
	std::cout << mGA->statistics() << "\n";

	return true;
}

void Game::UpdateWorld(float dt)
{
	mWorld->Update(dt);
}

void Game::Draw(double alpha)
{
	SDL_SetRenderDrawColor(mSDL_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mSDL_Renderer);

	mBackground->Render(0, 0, mSDL_Renderer);

	mWorld->Draw(mSDL_Renderer, alpha);

	//mB2World->DrawDebugData();

	SDL_RenderPresent(mSDL_Renderer);
}

void Game::DoPostProcessing()
{
	mWorld->DoPostProcessing();
}

Ship* Game::GetPlayerShip()
{
	if (!mPlayerShip)
		mPlayerShip = mWorld->SpawnEntity<Ship>(mScreenWidth / 2, mScreenHeight / 2, 48,
		64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));

	return mPlayerShip;
}

World* Game::GetWorld() const
{
	return mWorld;
}

void Game::Reset()
{

}

void Game::RunGA(float dt)
{

}

void Game::DoPhysicsStep(float dt, int b2VelIterations, int b2PosIterations)
{
	mB2World->Step(dt, b2VelIterations, b2PosIterations);
}
