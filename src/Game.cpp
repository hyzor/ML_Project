#include "Game.h"

#include "MyGA.h"

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
	if (mDebugText)
	{
		mDebugText->Free();
		delete mDebugText;
		mDebugText = nullptr;
	}

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

bool Game::Init(std::string assetsDir, std::string fontsDir, std::string texturesDir, float dt)
{
	mDebugText = new SDL_Wrapper::Texture();

	// World
	mWorld = World::GetInstance();
	mWorld->Init(mB2World, mScreenWidth, mScreenHeight);

	mBackground = mTextureManager->LoadTexture(mAssetsDir + "Background_800x600.png");

	//mPlayerShip = mWorld->SpawnEntity<Ship>(mScreenWidth/2, mScreenHeight/2, 48,
		//64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));

	Ship* enemyShip = mWorld->SpawnEntity<Ship>(mScreenWidth, mScreenHeight, 48,
		64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));
	enemyShip->Init(Ship::Type::STATIONARY);
	enemyShip->Init_b2(mWorld->Getb2World(), false);

	/*
	Ship* enemyShip2 = mWorld->SpawnEntity<Ship>(mScreenWidth/2, mScreenHeight/2, 48,
		64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));
	enemyShip2->Init(Ship::Type::NON_STATIONARY);

	enemyShip->SetTarget(enemyShip2->GetPosition(true));
	*/

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
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);
	setArray.add(0.0f, mScreenWidth);
	setArray.add(0.0f, mScreenHeight);

	// GA population
	mGaPop = new GAPopulation();
	//mWorld->AddEntity(enemyShip);

	mGaPop->initialize();
	MyGenome* myGenome1 = new MyGenome(1, setArray, 0.0f, 0.0f, 48, 64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"), MyGenome::Evaluate);
	mGaPop->add(myGenome1);
	//mWorld->AddEntity(mGaPop->individual())
	//mWorld->AddEntity(myGenome);

	for (int i = 0; i < mGaPop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&mGaPop->individual(i);
		myGenome->initialize();
	}

	// GA
	mGA = new MyGA(*mGaPop);
	mGA->crossover(MyGenome::OnePointCrossover);
	mGA->nGenerations(100);
	mGA->pMutation(0.15f);
	mGA->pCrossover(0.95f);
	mGA->scoreFilename("GA_score.dat");
	mGA->scoreFrequency(10);
	mGA->flushFrequency(50);
	mGA->selectScores(GAStatistics::AllScores);
	mGA->Init(this, enemyShip);

	//delete mGaPop;

	//myGenome1->SetScore(999.0f);

	std::cout << "Initial genomes:\n";
	for (int i = 0; i < mGA->populationSize(); ++i)
	{
		MyGenome* genome = (MyGenome*)&mGA->population().individual(i);
		((Ship*)genome)->Init_b2(mWorld->Getb2World(), false);
		std::cout << genome->GetID() << ": ";

		for (int j = 0; j < genome->length(); ++j)
		{
			std::cout << genome->gene(j) << ", ";
		}

		std::cout << " Score: " << genome->score() << "\n";
	}

	// Evolve by explicitly calling the GA step function
	while (!mGA->done())
	{
		mGA->step(dt);
		//Update(dt);
		//Draw();
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

	float geneSumWinner = 0.0f;
	for (int i = 0; i < genome->length(); ++i)
	{
		geneSumWinner += genome->gene(i);
		std::cout << genome->gene(i) << "\n";
	}

	//std::cout << geneSum1 << ", " << geneSum2 << ", " << geneSumWinner << "\n";

	std::cout << "The best individual is genome " << genome->GetID() << "\n";
	std::cout << "Best of generation data is in file '" << mGA->scoreFilename() << "'\n";

	// Dump statistics
	//std::cout << ga.statistics() << "\n";

	return true;
}

void Game::Update(float dt)
{
	mB2World->Step(dt, mB2VelIterations, mB2PosIterations);

	mWorld->Update(dt);

	//if (!mPlayerShip->IsAlive())
		//mPlayerShip = nullptr;
}

void Game::Draw()
{
	SDL_SetRenderDrawColor(mSDL_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mSDL_Renderer);

	mBackground->Render(0, 0, mSDL_Renderer);

	mWorld->Draw(mSDL_Renderer);

	mB2World->DrawDebugData();

	// DEBUG
	/*
	if (mDebugText->CreateFromText("Angle (Deg): " + std::to_string(fmod(GetPlayerShip()->GetAngle(false), 360.0f)), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 0, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosX (px): " + std::to_string(GetPlayerShip()->GetPosition(false).x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 20, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosY (px): " + std::to_string(GetPlayerShip()->GetPosition(false).y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 40, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosX (m): " + std::to_string(GetPlayerShip()->GetPosition(true).x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 60, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosY (m): " + std::to_string(GetPlayerShip()->GetPosition(true).y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 80, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("VelX: " + std::to_string(GetPlayerShip()->GetLinearVelocity().x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 100, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("VelY: " + std::to_string(GetPlayerShip()->GetLinearVelocity().y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 120, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointX (m): " + std::to_string(GetPlayerShip()->GetCurrentWaypoint().x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 140, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointY (m): " + std::to_string(GetPlayerShip()->GetCurrentWaypoint().y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 160, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointX (px): " + std::to_string(GetPlayerShip()->GetCurrentWaypoint().x*Box2dHelper::PixelsPerMeter), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 180, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointY (px): " + std::to_string(GetPlayerShip()->GetCurrentWaypoint().y*Box2dHelper::PixelsPerMeter), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 200, mSDL_Renderer);
	}
	*/

	SDL_RenderPresent(mSDL_Renderer);
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
