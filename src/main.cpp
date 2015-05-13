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
#include "MyGenome.h"

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

//int MyCrossoverFunc(const GAGenome& genome1, const GAGenome& genome2, GAGenome* result);
//float MyObjectiveFunc(GAPopulation& pop);

int main(int argc, char **argv)
{
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
	TTF_Font* font = nullptr;

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

	textureManager = TextureManager::GetInstance();
	textureManager->Init(renderer, dir_assets);

	gfx_text_debug = new SDL_Wrapper::Texture();

	gfx_background = TextureManager::GetInstance()->LoadTexture("Background.png");

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
	Ship* myShip = world->SpawnEntity<Ship>(WINDOW_WIDTH/2, WINDOW_HEIGHT/2, ENTITY_INIT_WIDTH,
		ENTITY_INIT_HEIGHT, ENTITY_INIT_HP, ENTITY_INIT_DMG, 0.0f, textureManager->LoadTexture("Ship.png"));

	GAPopulation* gaPop = new GAPopulation();
	MyGenome* myGenome1 = new MyGenome(10);
	MyGenome* myGenome2 = new MyGenome(1);
	gaPop->initialize();
	gaPop->add(myGenome1);
	gaPop->add(myGenome2);

	for (int i = 0; i < gaPop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&gaPop->individual(i);
		myGenome->initialize();
	}

	/*
	myShip->AddWaypoint(b2Vec2(0.0f, 0.0f));
	myShip->AddWaypoint(b2Vec2((0.0f)*Box2dHelper::Units, (WINDOW_HEIGHT / 2.0f)*Box2dHelper::Units));
	myShip->AddWaypoint(b2Vec2((WINDOW_WIDTH / 2.0f)*Box2dHelper::Units, (WINDOW_HEIGHT / 2.0f)*Box2dHelper::Units));
	myShip->AddWaypoint(b2Vec2(WINDOW_WIDTH*Box2dHelper::Units, 0.0f));
	myShip->AddWaypoint(b2Vec2(0.0f, WINDOW_HEIGHT*Box2dHelper::Units));
	*/

	//myShip->AddWaypoint(b2Vec2((-200.0f)*Box2dHelper::Units, (WINDOW_HEIGHT / 2)*Box2dHelper::Units));
	//myShip->AddWaypoint(b2Vec2((WINDOW_WIDTH / 2)*Box2dHelper::Units, (-20.0f)*Box2dHelper::Units));
	//myShip->AddWaypoint(b2Vec2((WINDOW_WIDTH / 2)*Box2dHelper::Units, (-10.0f)*Box2dHelper::Units));

	//myShip->AddMovementPattern(Ship::Attributes::MOVEMENT_CIRCLE, true);

	//Ship* enemyShip = world->SpawnEntity<Ship>(WINDOW_WIDTH / 3, WINDOW_HEIGHT / 2, ENTITY_INIT_WIDTH,
		//ENTITY_INIT_HEIGHT, ENTITY_INIT_HP, ENTITY_INIT_DMG, MathHelper::DegreesToRadians(90.0f), textureManager->LoadTexture("Ship.png"));

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

	// Create ga with the initial population
	MyGA* myGA = new MyGA(*gaPop);

    //GASimpleGA ga(genome);
	//ga.objectiveFunction(MyObjectiveFunc);
	myGA->crossover(MyGenome::Cross);
    //GASigmaTruncationScaling scaling;
	//myGA->populationSize(POPULATION_SIZE);
	myGA->nGenerations(NUM_GENERATIONS);
	myGA->pMutation(PROB_MUTATION);
	myGA->pCrossover(PROB_CROSSOVER);
    //ga.scaling(scaling);
	myGA->scoreFilename(FILENAME_SCORE);
	myGA->scoreFrequency(FREQ_SCORE);
	myGA->flushFrequency(FREQ_FLUSH);
	myGA->selectScores(GAStatistics::AllScores);
    //ga.evolve(seed);

	// Evolve by explicitly calling the GA step function
	while (!myGA->done())
	{
		myGA->step();
		//genome = ga.statistics().bestIndividual();
		//std::cout << genome.phenotype(0) << ", " << genome.phenotype(1) << "\n";
	}

	// Dump the results of the GA to the screen.
	MyGenome* genome = (MyGenome*)&myGA->statistics().bestIndividual();
	std::cout << "The best individual is genome " << genome->GetID() << "\n";
	std::cout << "Best of generation data is in file '" << myGA->scoreFilename() << "'\n";

	// Dump statistics
	//std::cout << ga.statistics() << "\n";

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
					//if (enemyShip)
						//enemyShip->Reset();
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
		//myShip->MoveTo({ (float)curMouseClickPos.x*Box2dHelper::Units, (float)curMouseClickPos.y*Box2dHelper::Units }, 20.0f*Box2dHelper::Units, dt);

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

		if (gfx_text_debug->CreateFromText("WaypointX (m): " + std::to_string(myShip->GetCurrentWaypoint().x), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 140, renderer);
		}

		if (gfx_text_debug->CreateFromText("WaypointY (m): " + std::to_string(myShip->GetCurrentWaypoint().y), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 160, renderer);
		}

		if (gfx_text_debug->CreateFromText("WaypointX (px): " + std::to_string(myShip->GetCurrentWaypoint().x*Box2dHelper::PixelsPerMeter), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 180, renderer);
		}

		if (gfx_text_debug->CreateFromText("WaypointY (px): " + std::to_string(myShip->GetCurrentWaypoint().y*Box2dHelper::PixelsPerMeter), { 255, 255, 255 }, font, renderer))
		{
			gfx_text_debug->Render(0, 200, renderer);
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

	if (gaPop)
	{
		gaPop->destroy();
		delete gaPop;
		gaPop = nullptr;
	}

	if (myGA)
	{
		delete myGA;
		myGA = nullptr;
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

/*
int MyCrossoverFunc(const GAGenome& genome1, const GAGenome& genome2, GAGenome* result, GAGenome* result2)
{
	MyGenome myGenome1 = (MyGenome&)genome1;
	MyGenome myGenome2 = (MyGenome&)genome2;

	myGenome1.cr

	return 1;
}
*/

/*
float MyObjectiveFunc(GAGenome& genome, GAPopulation& pop)
{
	MyGenome* myGenome = static_cast<MyGenome*>(&c);
	float score = 0.0f;

	for (int i = 0; i < pop.size(); ++i)
	{
		MyGenome* genome = static_cast<MyGenome*>(&pop.individual(i));

		if (matches[myGenome->GetID()][i] == false)
		{
			if (myGenome->GetID() > genome->GetID())
			{
				score += 1.0f;
			}
		}
	}

	return score;
}
*/