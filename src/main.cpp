/**
  @file ex9.C
  @brief find the maximum value of a continuous function with two variables.
  @author Matthew Wall  
  @date 10-Apr-1995
  
  Copyright 1995-1996 Massachusetts Institute of Technology.

  Sample program that illustrates how to use a GA to find the maximum value
of a continuous function in two variables.  This program uses a binary-to-
decimal genome.
*/

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>

#include <ga/ga.h>
#include <SDL/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <Box2D/Box2D.h>

#include "MyGA.h"
#include "Ship.h"
#include "SDL_Wrapper.h"
#include "World.h"
#include "TextureManager.h"
#include "DebugDraw.h"
#include "ContactListener.h"

/*
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/

// Directories
static const std::string dir_assets = "assets/";
static const std::string dir_fonts = dir_assets + "fonts/";

// Window
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

// GA defines
#define PROB_MUTATION 0.01
#define PROB_CROSSOVER 1.0
#define NUM_GENERATIONS 100
#define POPULATION_SIZE 30
#define FREQ_SCORE 10
#define FREQ_FLUSH 50
#define FILENAME_SCORE "GA_score.dat"

// Entity attributes
#define ENTITY_INIT_HP 5
#define ENTITY_INIT_DMG 1
#define ENTITY_INIT_SHOOTFREQ 0.1
#define ENTITY_INIT_WIDTH 48
#define ENTITY_INIT_HEIGHT 64

int MyCrossoverFunc(const GAGenome& genome1, const GAGenome& genome2, GAGenome* result);
float MyObjectiveFunc(GAGenome &);

int main(int argc, char **argv)
{
    std::cout << "Example 9\n\n";
	std::cout << "This program finds the maximum value in the function\n";
	std::cout << "  y = - x1^2 - x2^2\n";
	std::cout << "with the constraints\n";
	std::cout << "     -5 <= x1 <= 5\n";
	std::cout << "     -5 <= x2 <= 5\n";
	std::cout << "\n\n";
	std::cout.flush();

	bool gameIsRunning = true;

	// SDL
	SDL_Window* window = nullptr;
	SDL_Event sdlEvent;
	SDL_Renderer* renderer = nullptr;

	// Textures
	SDL_Wrapper::Texture* gfx_ship = nullptr;
	SDL_Wrapper::Texture* gfx_background = nullptr;
	SDL_Wrapper::Texture* gfx_text_debug = nullptr;
	SDL_Wrapper::Texture* gfx_projectile = nullptr;

	TextureManager* textureManager = nullptr;
	DebugDraw* debugDraw = nullptr;
	ContactListener* contactListener = nullptr;

	// Fonts
	TTF_Font* font;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return -1;
	}

	window = SDL_CreateWindow("GA Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
	if (!window)
	{
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (!renderer)
	{
		return -1;
	}

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	// Initialize SDL PNG loading
	int imgFlags = IMG_INIT_PNG;
	if (!(IMG_Init(imgFlags) & imgFlags))
	{
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return -1;
	}

	// Initialize SDL ttf
	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
		return -1;
	}

	// Open ttf font
	font = TTF_OpenFont((dir_fonts + "OpenSans-Regular.ttf").c_str(), 12);

	/*
	screenSurface = SDL_GetWindowSurface(window);
	if (!screenSurface)
		return -1;
		*/

	//background = SDL_Wrapper::LoadSurface(dir_assets + "Background.bmp", screenSurface);
	//gfx_entity = SDL_Wrapper::LoadSurface(dir_assets + "Ship.bmp", screenSurface);
	/*
	gfx_ship = new SDL_Wrapper::Texture();
	gfx_ship->LoadFromFile(dir_assets + "Ship.png", renderer);

	gfx_background = new SDL_Wrapper::Texture();
	gfx_background->LoadFromFile(dir_assets + "Background.png", renderer);

	gfx_text_debug = new SDL_Wrapper::Texture();

	gfx_projectile = new SDL_Wrapper::Texture();
	gfx_projectile->LoadFromFile(dir_assets + "Projectile.png", renderer);
	*/

	textureManager = TextureManager::GetInstance();
	textureManager->Init(renderer, dir_assets);

	gfx_text_debug = new SDL_Wrapper::Texture();

	//textureManager->LoadTexture("Ship.png");
	gfx_background = TextureManager::GetInstance()->LoadTexture("Background.png");
	//textureManager->LoadTexture("Projectile.png");


	// Game variables
	float dt = 1.0f / 60.0f;

	debugDraw = new DebugDraw(renderer);

	debugDraw->SetFlags(b2Draw::e_shapeBit
		//| b2Draw::e_aabbBit
		| b2Draw::e_jointBit
		| b2Draw::e_centerOfMassBit);

	contactListener = new ContactListener();

	// Box2D
	b2Vec2 gravity(0.0f, 0.0f);
	b2World* box2Dworld = new b2World(gravity);
	box2Dworld->SetDebugDraw(debugDraw);
	box2Dworld->SetContactListener(contactListener);
	int box2D_velocityIterations = 6;
	int box2D_positionIterations = 2;

	// World
	World* world = World::GetInstance();
	world->Init(box2Dworld);

	// Entity
	//Ship* myShip = new Ship(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, ENTITY_INIT_WIDTH, ENTITY_INIT_HEIGHT, ENTITY_INIT_HP, ENTITY_INIT_DMG, gfx_entity, &box2D_world);

	Ship* myShip = world->SpawnEntity<Ship>(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, ENTITY_INIT_WIDTH,
		ENTITY_INIT_HEIGHT, ENTITY_INIT_HP, ENTITY_INIT_DMG, MathHelper::DegreesToRadians(90.0f), textureManager->LoadTexture("Ship.png"));

	Ship* enemyShip = world->SpawnEntity<Ship>(WINDOW_WIDTH / 3, WINDOW_HEIGHT / 2, ENTITY_INIT_WIDTH,
		ENTITY_INIT_HEIGHT, ENTITY_INIT_HP, ENTITY_INIT_DMG, MathHelper::DegreesToRadians(90.0f), textureManager->LoadTexture("Ship.png"));

	vec2 curMouseClickPos;

	// See if we've been given a seed to use (for testing purposes).  When you
	// specify a random seed, the evolution will be exactly the same each time
	// you use that seed number.
    unsigned int seed = 0;
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i++], "seed") == 0)
        {
            seed = atoi(argv[i]);
        }
    }

	// Create a phenotype for two variables.  The number of bits you can use to
	// represent any number is limited by the type of computer you are using.  In
	// this case, we use 16 bits to represent a floating point number whose value
	// can range from -5 to 5, inclusive.  The bounds on x1 and x2 can be applied
	// here and/or in the objective function.
    GABin2DecPhenotype map;
    map.add(16, -5, 5);
    map.add(16, -5, 5);

	// Create the template genome using the phenotype map we just made.
	GABin2DecGenome genome(map, MyObjectiveFunc);

	// Now create the GA using the genome and run it.  We'll use sigma truncation
	// scaling so that we can handle negative objective scores.
	MyGA ga(genome);
    //GASimpleGA ga(genome);
	ga.crossover(MyCrossoverFunc);
    GASigmaTruncationScaling scaling;
    ga.populationSize(POPULATION_SIZE);
    ga.nGenerations(NUM_GENERATIONS);
    ga.pMutation(PROB_MUTATION);
    ga.pCrossover(PROB_CROSSOVER);
    ga.scaling(scaling);
	ga.scoreFilename(FILENAME_SCORE);
    ga.scoreFrequency(FREQ_SCORE);
    ga.flushFrequency(FREQ_FLUSH);
	ga.selectScores(GAStatistics::AllScores);
    //ga.evolve(seed);

	// Evolve by explicitly calling the GA step function
	while (!ga.done())
	{
		ga.step();
		genome = ga.statistics().bestIndividual();
		std::cout << genome.phenotype(0) << ", " << genome.phenotype(1) << "\n";
	}

	// Dump the results of the GA to the screen.
    genome = ga.statistics().bestIndividual();
	std::cout << "the ga found an optimum at the point (";
	std::cout << genome.phenotype(0) << ", " << genome.phenotype(1) << ")\n\n";
	std::cout << "best of generation data are in '" << ga.scoreFilename() << "'\n";

	std::cout << ga.statistics() << "\n";

	while (gameIsRunning)
	{
		while (SDL_PollEvent(&sdlEvent) != 0)
		{
			if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) 
			{
				SDL_GetMouseState(&curMouseClickPos.x, &curMouseClickPos.y);
			}

			if (sdlEvent.type == SDL_QUIT)
			{
				gameIsRunning = false;
			}
			else if (sdlEvent.type == SDL_KEYDOWN)
			{
				switch (sdlEvent.key.keysym.sym)
				{
				case SDLK_w:
					std::cout << "Key event: 'w' pressed\n";
					myShip->ActivateEventTrigger(Ship::THRUST_FORWARD, true);
					break;
				case SDLK_s:
					std::cout << "Key event: 's' pressed\n";
					myShip->ActivateEventTrigger(Ship::THRUST_BACKWARD, true);
					break;
				case SDLK_a:
					std::cout << "Key event: 'a' pressed\n";
					myShip->ActivateEventTrigger(Ship::TORQUE_LEFT, true);
					break;
				case SDLK_d:
					std::cout << "Key event: 'd' pressed\n";
					myShip->ActivateEventTrigger(Ship::TORQUE_RIGHT, true);
					break;
				case SDLK_r:
					std::cout << "Key event: 'r' pressed\n";
					if (myShip)
						myShip->Reset();
					if (enemyShip)
						enemyShip->Reset();
					break;
				case SDLK_e:
					std::cout << "Key event: 'e' pressed\n";
					myShip->ActivateEventTrigger(Ship::STRAFE_RIGHT, true);
					break;
				case SDLK_q:
					std::cout << "Key event: 'q' pressed\n";
					myShip->ActivateEventTrigger(Ship::STRAFE_LEFT, true);
					break;
				case SDLK_SPACE:
					std::cout << "Key event: 'space' pressed\n";
					myShip->ActivateEventTrigger(Ship::SHOOT, true);
					break;
				case SDLK_LSHIFT:
					std::cout << "Key event: 'lshift' pressed\n";
					myShip->ActivateEventTrigger(Ship::STABILIZE, true);
					break;
				case SDLK_ESCAPE:
					gameIsRunning = false;
					break;
				}
			}
			else if (sdlEvent.type == SDL_KEYUP)
			{
				switch (sdlEvent.key.keysym.sym)
				{
					case SDLK_w:
						std::cout << "Key event: 'w' released\n";
						myShip->ActivateEventTrigger(Ship::THRUST_FORWARD, false);
						break;
					case SDLK_s:
						std::cout << "Key event: 's' released\n";
						myShip->ActivateEventTrigger(Ship::THRUST_BACKWARD, false);
						break;
					case SDLK_a:
						std::cout << "Key event: 'a' released\n";
						myShip->ActivateEventTrigger(Ship::TORQUE_LEFT, false);
						break;
					case SDLK_d:
						std::cout << "Key event: 'd' released\n";
						myShip->ActivateEventTrigger(Ship::TORQUE_RIGHT, false);
						break;
					case SDLK_e:
						std::cout << "Key event: 'e' released\n";
						myShip->ActivateEventTrigger(Ship::STRAFE_RIGHT, false);
						break;
					case SDLK_q:
						std::cout << "Key event: 'q' released\n";
						myShip->ActivateEventTrigger(Ship::STRAFE_LEFT, false);
						break;
					case SDLK_SPACE:
						std::cout << "Key event: 'space' released\n";
						myShip->ActivateEventTrigger(Ship::SHOOT, false);
						break;
					case SDLK_LSHIFT:
						std::cout << "Key event: 'lshift' released\n";
						myShip->ActivateEventTrigger(Ship::STABILIZE, false);
						break;
				}
			}
		}

		// Box2D world step
		box2Dworld->Step(dt, box2D_velocityIterations, box2D_positionIterations);

		// Update game logic
		myShip->MoveTo({ (float)curMouseClickPos.x*Box2dHelper::Units, (float)curMouseClickPos.y*Box2dHelper::Units }, 40.0f*Box2dHelper::Units, dt);

		world->Update(dt);

		// Render world
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(renderer);

		gfx_background->Render(0, 0, renderer);

		world->Draw(renderer);

		box2Dworld->DrawDebugData();

		// Debug text
		if (gfx_text_debug->CreateFromText("Angle (Deg): " + std::to_string(fmod(myShip->GetAngle(false), 360.0f)), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 0, renderer);
		}

		if (gfx_text_debug->CreateFromText("PosX (px): " + std::to_string(myShip->GetPosition(false).x), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 20, renderer);
		}

		if (gfx_text_debug->CreateFromText("PosY (px): " + std::to_string(myShip->GetPosition(false).y), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 40, renderer);
		}

		if (gfx_text_debug->CreateFromText("PosX (m): " + std::to_string(myShip->GetPosition(true).x), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 60, renderer);
		}

		if (gfx_text_debug->CreateFromText("PosY (m): " + std::to_string(myShip->GetPosition(true).y), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 80, renderer);
		}

		if (gfx_text_debug->CreateFromText("VelX: " + std::to_string(myShip->GetLinearVelocity().x), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 100, renderer);
		}

		if (gfx_text_debug->CreateFromText("VelY: " + std::to_string(myShip->GetLinearVelocity().y), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 120, renderer);
		}

		SDL_RenderPresent(renderer);
	}

	if (debugDraw)
	{
		delete debugDraw;
		debugDraw = nullptr;
	}

	if (contactListener)
	{
		delete contactListener;
		contactListener = nullptr;
	}

	world = nullptr;
	//world->Clear();
	World::Destroy();
	TextureManager::Destroy();

	// Free SDL stuff
	gfx_background = nullptr;
	delete gfx_text_debug;
	gfx_text_debug = nullptr;

	TTF_CloseFont(font);
	font = nullptr;

	SDL_DestroyWindow(window);
	window = nullptr;

	SDL_DestroyRenderer(renderer);
	renderer = nullptr;

	TTF_Quit();
	IMG_Quit();
	SDL_Quit();

	_CrtDumpMemoryLeaks();

    return 0;
}

int MyCrossoverFunc(const GAGenome& genome1, const GAGenome& genome2, GAGenome* result)
{
	//GA1DArrayGenome

	return 1;
}


/// This objective function tries to maximize the value of the function
///
///                  y = -(x1*x1 + x2*x2)
///
float MyObjectiveFunc(GAGenome & c)
{
    GABin2DecGenome & genome = (GABin2DecGenome &)c;

    float y;
    y = -genome.phenotype(0) * genome.phenotype(0);
    y -= genome.phenotype(1) * genome.phenotype(1);
    return y;
}
