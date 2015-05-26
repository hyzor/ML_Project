#include "Game.h"

#include "MyGA.h"
#include "MyGA2.h"

#include <sstream>
#include <iostream>
#include <fstream>

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

bool Game::Init(std::string assetsDir, std::string fontsDir, std::string texturesDir)
{
	mDebugText = new SDL_Wrapper::Texture();
	
	// World
	mWorld = World::GetInstance();
	mWorld->Init(mB2World, mScreenWidth, mScreenHeight);

	mBackground = mTextureManager->LoadTexture(mAssetsDir + "Background_stars_800x600.png");

	//mPlayerShip = mWorld->SpawnEntity<Ship>(mScreenWidth/2, mScreenHeight/2, 48,
	//64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));

	enemyShip = mWorld->SpawnEntity<Ship>(mScreenWidth - (48 / 2), mScreenHeight - (64 / 2), 48,
		64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));
	enemyShip->Init(Ship::Type::STATIONARY);
	enemyShip->Init_b2(mWorld->Getb2World(), false, Entity::Type::SHIP, 0.0f);

	Entity* obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.25f, mScreenHeight *0.25f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);
	//enemyShip->SetType(Entity::Type::SHIP);

	obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.5f, mScreenHeight *0.5f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	obstacle = mWorld->SpawnEntity<Entity>(mScreenWidth *0.75f, mScreenHeight *0.75f, 64, 64, 1, 2, 0.0f, true, mTextureManager->LoadTexture("Obstacle.png"));
	obstacle->Init_b2(mWorld->Getb2World(), false, Entity::Type::STATIC);

	/*
	Ship* enemyShip2 = mWorld->SpawnEntity<Ship>(mScreenWidth/2, mScreenHeight/2, 48,
	64, 5, 1, 0.0f, false, mTextureManager->LoadTexture("Ship.png"));
	enemyShip2->Init(Ship::Type::NON_STATIONARY);

	enemyShip->SetTarget(enemyShip2->GetPosition(true));
	*/

	return true;
}

void Game::RunGA(std::string assetsDir, std::string fontsDir, std::string texturesDir, double dt_fixed, float speedup, bool steadyState, int generations, int populationSize, int crossover, float pCross, float pMut)
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

	/*
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
	*/

	// GA population
	mGaPop = new GAPopulation();
	//mWorld->AddEntity(enemyShip);

	mGaPop->initialize();

	int popSize = populationSize;

	MyGenome* myGenome1;
		for (int i = 0; i < popSize; i++){
			myGenome1 = new MyGenome(1, setArray, 48.0f*0.5f, 64.0f*0.5f, 48, 64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"), MyGenome::Evaluate);
			mGaPop->add(myGenome1);
		}
	//mWorld->AddEntity(mGaPop->individual())
	//mWorld->AddEntity(myGenome);

	for (int i = 0; i < mGaPop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&mGaPop->individual(i);
		myGenome->initialize();
	}

	// GA
	//mGA = new MyGA(*mGaPop);

	bool ss = false;
	if (ss){
		mGA = new MyGA(*mGaPop);
		if (crossover == Crossover::ONEPOINT)
			((MyGA*)mGA)->crossover(MyGenome::OnePointCrossover);
		if (crossover == Crossover::TWOPOINT)
			((MyGA*)mGA)->crossover(MyGenome::TwoPointCrossover);


	}
	else{

		mGA = new MyGA2(*mGaPop);
		if (crossover == Crossover::ONEPOINT)
			((MyGA2*)mGA)->crossover(MyGenome::OnePointCrossover);
		if (crossover == Crossover::TWOPOINT)
			((MyGA2*)mGA)->crossover(MyGenome::TwoPointCrossover);

	}
	mGA->nGenerations(generations);
	mGA->pMutation(pMut);
	mGA->pCrossover(pCross);
	mGA->scoreFilename("GA_score.dat");
	mGA->scoreFrequency(1);
	mGA->flushFrequency(1);
	mGA->selectScores(GAStatistics::AllScores);
	mGA->recordDiversity(gaTrue);
	
	if (ss){
		((MyGA*)mGA)->Init(this, enemyShip);
		((MyGA*)mGA)->nReplacement(popSize - 1); // if GASteadyStateGA
	}
	else{
		((MyGA2*)mGA)->Init(this, enemyShip);
		((MyGA2*)mGA)->elitist(gaTrue); // if GASimpleGA

		}
	
	std::cout << "Initial genomes:\n";
	for (int i = 0; i < mGA->populationSize(); ++i)
	{
		MyGenome* genome = (MyGenome*)&mGA->population().individual(i);
		//((Ship*)genome)->Init_b2(mWorld->Getb2World(), false, Entity::Type::SHIP);

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
	/*
	testGenome->ClearWaypoints();
	testGenome->AddWaypoint(b2Vec2(200.0f*Box2dHelper::Units, 500.0f*Box2dHelper::Units));
	testGenome->AddWaypoint(b2Vec2(600.0f*Box2dHelper::Units, 480.0f*Box2dHelper::Units));
	testGenome->AddWaypoint(b2Vec2(600.0f*Box2dHelper::Units, 200.0f*Box2dHelper::Units));
	testGenome->AddWaypoint(b2Vec2(200.0f*Box2dHelper::Units, 200.0f*Box2dHelper::Units));
	*/

	/*
	testGenome->ClearWaypoints();
	testGenome->AddWaypoint(b2Vec2(789.99*Box2dHelper::Units, 361.261*Box2dHelper::Units));
	testGenome->AddWaypoint(b2Vec2(443.48*Box2dHelper::Units, 347.585*Box2dHelper::Units));
	testGenome->AddWaypoint(b2Vec2(755.339*Box2dHelper::Units, 101.808*Box2dHelper::Units));
	testGenome->AddWaypoint(b2Vec2(702.99*Box2dHelper::Units, 148.048*Box2dHelper::Units));
	testGenome->AddWaypoint(b2Vec2(426.601*Box2dHelper::Units, 187.851*Box2dHelper::Units));
	testGenome->Init_b2(mWorld->Getb2World(), false, Entity::Type::SHIP, 0.1);
	auto currentTime = std::chrono::system_clock::now();

	float dt_test = 1.0f / 60.0f;
	double score = mGA->ObjectiveFunction(testGenome, dt_fixed, std::chrono::system_clock::now(), 10.0f);
	std::cout << "Score: " << score << " dt: " << dt_fixed << "\n";
	*/


	/*
	MyGenome::Init(*testGenome);
	testGenome->Reset();
	((Ship*)testGenome)->Reset();

	testGenome->Setb2BodyType(b2_dynamicBody);
	testGenome->SetCollisionEnabled(true);
	
	testGenome->ClearWaypoints();
	
	

	dt_test = 1.0f / 60.0f;
	score = mGA->ObjectiveFunction(testGenome, dt_fixed, std::chrono::system_clock::now(), 10.0f);
	std::cout << "Score: " << score << " dt: " << dt_fixed << "\n";

	testGenome->Setb2BodyType(b2_staticBody);
	testGenome->SetCollisionEnabled(false);
	*/
	

	// Evolve by explicitly calling the GA step function
	while (!mGA->done())
	
	{
		if (ss)
			((MyGA*)mGA)->step(dt_fixed, speedup);
		else
			((MyGA2*)mGA)->step(dt_fixed, speedup);

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

	for (int i = 0; i < genome->length(); ++i)
	{
		std::cout << genome->gene(i) << "\n";
	}

	std::cout << "The best individual is genome " << genome->GetID() << "\n";
	std::cout << "Best of generation data is in file '" << mGA->scoreFilename() << "'\n";


	std::stringstream sstm;
	sstm << "results-" << mGA->nGenerations() << "-gen-" << mGA->populationSize() << "-popsize-"<< "-pMut-" << pMut << "-pCross-" << pCross <<  ".txt";
	std::string str = sstm.str();
	const char * c = str.c_str();

	std::ofstream file(c);



	// Dump statistics
	file << mGA->statistics() << "\n";
	file << "Best genome: ";

	for (int i = 0; i < genome->length()-1; ++i)
	{
		file << genome->gene(i) << ", ";
	}
	file << genome->gene(genome->length());
	file.close();



	std::cout << mGA->statistics() << "\n";



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
	//mB2World->Step(dt, mB2VelIterations, mB2PosIterations);
	//DoPhysicsStep(dt, mB2VelIterations, mB2PosIterations);

	mWorld->Update(dt);

	//if (!mPlayerShip->IsAlive())
		//mPlayerShip = nullptr;
}

void Game::Draw(double alpha)
{
	SDL_SetRenderDrawColor(mSDL_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mSDL_Renderer);

	mBackground->Render(0, 0, mSDL_Renderer);

	mWorld->Draw(mSDL_Renderer, alpha);

	//mB2World->DrawDebugData();

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


void Game::DoPhysicsStep(float dt, int b2VelIterations, int b2PosIterations)
{
	mB2World->Step(dt, b2VelIterations, b2PosIterations);
}
