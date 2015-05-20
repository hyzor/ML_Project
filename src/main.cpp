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

#define INSTANTIATE_REAL_GENOME true

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <vector>
#include <ctime>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL_ttf/SDL_ttf.h>
#include <Box2D/Box2D.h>

#include "MyGA.h"
#include "Ship.h"
#include "SDL_Wrapper.h"
#include "World.h"
#include "TextureManager.h"
#include "MyB2DebugDraw.h"
#include "MyB2ContactListener.h"
#include "MyGenome.h"

#include "Game.h"

/*
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
*/

// Directories
static const std::string dir_assets = "assets/";
static const std::string dir_fonts = dir_assets + "fonts/";
static const std::string dir_textures = dir_assets;

// Window
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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
	srand(time(NULL));

	GARandomSeed();

	bool gameIsRunning = false;

	// SDL
	SDL_Window* window = nullptr;
	SDL_Event sdlEvent;
	SDL_Renderer* renderer = nullptr;

	TextureManager* textureManager = nullptr;
	MyB2DebugDraw* debugDraw = nullptr;
	MyB2ContactListener* contactListener = nullptr;

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

	// Game variables
	float dt = 1.0f / 10.0f;

	debugDraw = new MyB2DebugDraw(renderer);

	debugDraw->SetFlags(b2Draw::e_shapeBit
		//| b2Draw::e_aabbBit
		| b2Draw::e_jointBit
		| b2Draw::e_centerOfMassBit);

	contactListener = new MyB2ContactListener();

	// Box2D
	b2Vec2 gravity(0.0f, 0.0f);
	b2World* box2Dworld = new b2World(gravity);
	//box2Dworld->SetDebugDraw(debugDraw);
	box2Dworld->SetContactListener(contactListener);

	Game* game = new Game(window, renderer, WINDOW_WIDTH, WINDOW_HEIGHT, box2Dworld, textureManager, font);
	game->Init(dir_assets, dir_fonts, dir_textures, dt);
	gameIsRunning = true;

	vec2 curMouseClickPos;

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
					game->GetPlayerShip()->ActivateEventTrigger(Ship::THRUST_FORWARD, true);
					break;
				case SDLK_s:
					game->GetPlayerShip()->ActivateEventTrigger(Ship::THRUST_BACKWARD, true);
					break;
				case SDLK_a:
					game->GetPlayerShip()->ActivateEventTrigger(Ship::TORQUE_LEFT, true);
					break;
				case SDLK_d:
					game->GetPlayerShip()->ActivateEventTrigger(Ship::TORQUE_RIGHT, true);
					break;
				case SDLK_r:
					if (game->GetPlayerShip())
						game->GetPlayerShip()->Reset();
					break;
				case SDLK_e:
					game->GetPlayerShip()->ActivateEventTrigger(Ship::STRAFE_RIGHT, true);
					break;
				case SDLK_q:
					game->GetPlayerShip()->ActivateEventTrigger(Ship::STRAFE_LEFT, true);
					break;
				case SDLK_SPACE:
					game->GetPlayerShip()->ActivateEventTrigger(Ship::SHOOT, true);
					break;
				case SDLK_LSHIFT:
					game->GetPlayerShip()->ActivateEventTrigger(Ship::STABILIZE, true);
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
						game->GetPlayerShip()->ActivateEventTrigger(Ship::THRUST_FORWARD, false);
						break;
					case SDLK_s:
						game->GetPlayerShip()->ActivateEventTrigger(Ship::THRUST_BACKWARD, false);
						break;
					case SDLK_a:
						game->GetPlayerShip()->ActivateEventTrigger(Ship::TORQUE_LEFT, false);
						break;
					case SDLK_d:
						game->GetPlayerShip()->ActivateEventTrigger(Ship::TORQUE_RIGHT, false);
						break;
					case SDLK_e:
						game->GetPlayerShip()->ActivateEventTrigger(Ship::STRAFE_RIGHT, false);
						break;
					case SDLK_q:
						game->GetPlayerShip()->ActivateEventTrigger(Ship::STRAFE_LEFT, false);
						break;
					case SDLK_SPACE:
						game->GetPlayerShip()->ActivateEventTrigger(Ship::SHOOT, false);
						break;
					case SDLK_LSHIFT:
						game->GetPlayerShip()->ActivateEventTrigger(Ship::STABILIZE, false);
						break;
				}
			}
		}

		// Update game
		game->Update(dt);

		// Draw game
		//game->Draw();

		// Present final canvas
		//SDL_RenderPresent(renderer);
	}

	if (game)
	{
		delete game;
		game = nullptr;
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

	TextureManager::Destroy();
	textureManager = nullptr;

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