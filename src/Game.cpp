#include "Game.h"

#include "MyGA.h"
#include "MyGA_Simple.h"

#include <sstream>
#include <iostream>
#include <fstream>

#include <ga/GARealGenome.C>

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

bool Game::Init(std::string assetsDir, std::string fontsDir, std::string texturesDir)
{
	// World
	mWorld = World::GetInstance();
	mWorld->Init(mB2World, mScreenWidth, mScreenHeight);

	mBackground = mTextureManager->LoadTexture(mAssetsDir + "Background_stars_800x600.png");

	enemyShip = mWorld->SpawnEntity<Ship>(mScreenWidth - (48 / 2), mScreenHeight - (64 / 2), 48,
		64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));
	enemyShip->Init(Ship::Type::STATIONARY);
	enemyShip->Init_b2(mWorld->Getb2World(), false, Entity::Type::SHIP, 0.0f);

	Entity* obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.25f, mScreenHeight *0.25f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.5f, mScreenHeight *0.5f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.75f, mScreenHeight *0.75f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	return true;
}

void Game::RunGA(std::string assetsDir, std::string fontsDir, std::string texturesDir,
	double dt_fixed, float speedup, bool steadyState,
	int generations, int populationSize, int crossover, int mutatorType, float pCross, float pMut, int iteration, bool doDraw)
{
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

	int popSize = populationSize;

	MyGenome::Mutator mutatorFunc = (GAGenome::Mutator)0;

	if (mutatorType == Game::MUTATOR_REAL_SWAP)
		mutatorFunc = GARealSwapMutator;
	else if (mutatorType == Game::MUTATOR_REAL_GAUSSIAN)
		mutatorFunc = GARealGaussianMutator;
	else
		mutatorFunc = GARealUniformMutator;

	MyGenome* myGenome1;
	for (int i = 0; i < popSize; i++)
	{
		myGenome1 = new MyGenome(1, setArray, 48.0f*0.5f, 64.0f*0.5f, 48, 64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"), MyGenome::Evaluate, mutatorFunc);
		mGaPop->add(myGenome1);

	}

	for (int i = 0; i < mGaPop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&mGaPop->individual(i);
		myGenome->initialize();
	}

	if (steadyState)
	{
		mGA = new MyGA(*mGaPop);

		if (crossover == Crossover::CROSSOVER_REAL_ONEPOINT)
			((MyGA*)mGA)->crossover(GARealOnePointCrossover);
		else if (crossover == Crossover::CROSSOVER_REAL_TWOPOINT)
			((MyGA*)mGA)->crossover(GARealTwoPointCrossover);
		else
			((MyGA*)mGA)->crossover(GARealBlendCrossover);
	}
	else
	{
		mGA = new MyGA_Simple(*mGaPop);

		if (crossover == Crossover::CROSSOVER_REAL_ONEPOINT)
			((MyGA_Simple*)mGA)->crossover(GARealOnePointCrossover);
		else if (crossover == Crossover::CROSSOVER_REAL_TWOPOINT)
			((MyGA_Simple*)mGA)->crossover(GARealTwoPointCrossover);
		else
			((MyGA_Simple*)mGA)->crossover(GARealBlendCrossover);
	}

	mGA->selector(*new GARouletteWheelSelector);
	mGA->nGenerations(generations);
	mGA->pMutation(pMut);
	mGA->pCrossover(pCross);
	mGA->scoreFilename("GA_score.dat");
	mGA->scoreFrequency(1);
	//mGA->flushFrequency(generations);
	mGA->selectScores(GAStatistics::AllScores);
	mGA->recordDiversity(gaTrue);

	if (steadyState)
	{
		((MyGA*)mGA)->Init(this, enemyShip);
		((MyGA*)mGA)->nReplacement(popSize - 1); // if GASteadyStateGA
	}
	else
	{
		((MyGA_Simple*)mGA)->Init(this, enemyShip);
		((MyGA_Simple*)mGA)->elitist(gaTrue); // if GASimpleGA
	}
	
	//std::cout << "Initial genomes:\n";
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

		/*
		std::cout << genome->GetID() << ": ";

		for (int j = 0; j < genome->length(); ++j)
		{
			std::cout << genome->gene(j) << ", ";
		}

		std::cout << " Score: " << genome->score() << "\n";
		*/
	}
	

	// Evolve by explicitly calling the GA step function
	while (!mGA->done())
	{
		if (steadyState)
			((MyGA*)mGA)->step(dt_fixed, speedup, doDraw);
		else
			((MyGA_Simple*)mGA)->step(dt_fixed, speedup, doDraw);
	}

	/*
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
	*/


	/*
	std::cout << "\n";

	// Dump the results of the GA to the screen.
	MyGenome* genome = (MyGenome*)&mGA->statistics().bestIndividual();

	for (int i = 0; i < genome->length(); ++i)
	{
		std::cout << genome->gene(i) << "\n";
	}
	*/

	//std::cout << "The best individual is genome " << genome->GetID() << "\n";
	//std::cout << "Best of generation data is in file '" << mGA->scoreFilename() << "'\n";

	MyGenome* genome = (MyGenome*)&mGA->statistics().bestIndividual();

	std::stringstream sstm;

	sstm << (steadyState ? "SteadyState_" : "Elitism_");

	if (crossover == Crossover::CROSSOVER_REAL_ONEPOINT)
		sstm << "CROSS-OnePoint_";
	else if (crossover == Crossover::CROSSOVER_REAL_TWOPOINT)
		sstm << "CROSS-TwoPoint_";
	else
		sstm << "CROSS-Blend_";

	if (mutatorType == MutationTypes::MUTATOR_REAL_SWAP)
		sstm << "MUTATOR-Swap_";
	else if (mutatorType == MutationTypes::MUTATOR_REAL_UNIFORM)
		sstm << "MUTATOR-Uniform_";
	else
		sstm << "MUTATOR-Gaussian_";

	sstm << "NumGen-" << mGA->nGenerations() << "_PopSize-" << mGA->populationSize() << "_pMut-" << pMut << "_pCross-" << pCross << "_It-" << iteration << ".txt";

	std::string str = sstm.str();
	const char* c = str.c_str();

	std::ofstream file(c);

	// Dump statistics
	file << mGA->statistics() << "\n";
	file << "Best genome:\n";

	for (int i = 0; i < genome->length(); ++i)
	{
		file << genome->gene(i) << "\n";
	}

	file << "\nDiversities: \n";

	if (steadyState)
	{
		for (unsigned int i = 0; i < ((MyGA*)mGA)->GetDiversities().size(); ++i)
		{
			file << ((MyGA*)mGA)->GetDiversities()[i] << "\n";
		}
	}
	else
	{
		for (unsigned int i = 0; i < ((MyGA_Simple*)mGA)->GetDiversities().size(); ++i)
		{
			file << ((MyGA_Simple*)mGA)->GetDiversities()[i] << "\n";
		}
	}

	file.close();

	//std::cout << mGA->statistics() << "\n";
	std::cout << "\nFINISHED! Flushing to file:\n" << str << "\n";

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


double Game::RunGenomeFromGeneSet(std::vector<float>& geneSet, float dt_fixed, float speedup, bool doDraw)
{
	GARealAlleleSetArray setArray;

	for (unsigned int i = 0; i < geneSet.size(); ++i)
	{
		setArray.add(0.0f, 0.0f);
	}

	MyGenome* genome = new MyGenome(1, setArray, 48.0f*0.5f, 64.0f*0.5f, 48, 64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"), MyGenome::Evaluate);

	for (unsigned int i = 0; i < geneSet.size(); ++i)
	{
		genome->gene(i, geneSet[i]);
	}

	MyGenome::Init(*genome);
	genome->initialize();
	genome->Init_b2(GetWorld()->Getb2World(), false, Entity::Type::SHIP, genome->gene(genome->length() - 1));
	genome->Init_SDL();
	genome->SetType(Entity::Type::SHIP);
	genome->Reset();
	((Ship*)genome)->Reset();
	genome->Setb2BodyType(b2_dynamicBody);
	genome->SetCollisionEnabled(true);

	return MyGA::ObjectiveFunction(genome, dt_fixed, std::chrono::system_clock::now(), speedup, this, enemyShip, doDraw);
}

void Game::DoPhysicsStep(float dt, int b2VelIterations, int b2PosIterations)
{
	mB2World->Step(dt, b2VelIterations, b2PosIterations);
}
