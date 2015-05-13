#include "Game.h"


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

	mWorld = nullptr;
	mWorld->Clear();
	World::Destroy();
	mTextureManager = nullptr;
	//TextureManager::Destroy();
}

bool Game::Init(std::string assetsDir, std::string fontsDir, std::string texturesDir)
{
	mDebugText = new SDL_Wrapper::Texture();

	// World
	mWorld = World::GetInstance();
	mWorld->Init(mB2World, mScreenWidth, mScreenHeight);

	mBackground = mTextureManager->LoadTexture(mAssetsDir + "Background_800x600.png");

	mPlayerShip = mWorld->SpawnEntity<Ship>(mScreenWidth/2, mScreenHeight/2, 48,
		64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"));
	//mPlayerShip->AddMovementPattern(Ship::MOVEMENT_ZIGZAG, true);
	//mPlayerShip->Init(mWorld);

	// GA population
	mGaPop = new GAPopulation();
	MyGenome* myGenome1 = new MyGenome(10);
	MyGenome* myGenome2 = new MyGenome(1);
	mGaPop->initialize();
	mGaPop->add(myGenome1);
	mGaPop->add(myGenome2);

	for (int i = 0; i < mGaPop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&mGaPop->individual(i);
		myGenome->initialize();
	}

	// GA
	mGA = new MyGA(*mGaPop);
	mGA->crossover(MyGenome::Cross);
	mGA->nGenerations(100);
	mGA->pMutation(0.01);
	mGA->pCrossover(0.95);
	mGA->scoreFilename("GA_score.dat");
	mGA->scoreFrequency(10);
	mGA->flushFrequency(50);
	mGA->selectScores(GAStatistics::AllScores);

	// Evolve by explicitly calling the GA step function
	while (!mGA->done())
	{
		mGA->step();
	}

	// Dump the results of the GA to the screen.
	MyGenome* genome = (MyGenome*)&mGA->statistics().bestIndividual();
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
}

void Game::Draw()
{
	SDL_SetRenderDrawColor(mSDL_Renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(mSDL_Renderer);

	mBackground->Render(0, 0, mSDL_Renderer);

	mWorld->Draw(mSDL_Renderer);

	mB2World->DrawDebugData();

	// DEBUG
	if (mDebugText->CreateFromText("Angle (Deg): " + std::to_string(fmod(mPlayerShip->GetAngle(false), 360.0f)), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 0, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosX (px): " + std::to_string(mPlayerShip->GetPosition(false).x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 20, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosY (px): " + std::to_string(mPlayerShip->GetPosition(false).y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 40, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosX (m): " + std::to_string(mPlayerShip->GetPosition(true).x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 60, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("PosY (m): " + std::to_string(mPlayerShip->GetPosition(true).y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 80, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("VelX: " + std::to_string(mPlayerShip->GetLinearVelocity().x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 100, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("VelY: " + std::to_string(mPlayerShip->GetLinearVelocity().y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 120, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointX (m): " + std::to_string(mPlayerShip->GetCurrentWaypoint().x), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 140, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointY (m): " + std::to_string(mPlayerShip->GetCurrentWaypoint().y), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 160, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointX (px): " + std::to_string(mPlayerShip->GetCurrentWaypoint().x*Box2dHelper::PixelsPerMeter), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 180, mSDL_Renderer);
	}

	if (mDebugText->CreateFromText("WaypointY (px): " + std::to_string(mPlayerShip->GetCurrentWaypoint().y*Box2dHelper::PixelsPerMeter), { 255, 255, 255 }, mMainFont, mSDL_Renderer))
	{
		mDebugText->Render(0, 200, mSDL_Renderer);
	}
}

Ship* Game::GetPlayerShip() const
{
	return mPlayerShip;
}

World* Game::GetWorld() const
{
	return mWorld;
}
