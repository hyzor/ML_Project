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

#include <chrono>

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
	//float dt = 1.0f / 10.0f;
	double dt_fixed = 1.0 / 60.0;
	float t = 0.0f;
	auto currentTime = std::chrono::system_clock::now();

	debugDraw = new MyB2DebugDraw(renderer);

	debugDraw->SetFlags(b2Draw::e_shapeBit
		//| b2Draw::e_aabbBit
		| b2Draw::e_jointBit
		| b2Draw::e_centerOfMassBit);

	contactListener = new MyB2ContactListener();

	// Box2D
	b2Vec2 gravity(0.0f, 0.0f);
	b2World* box2Dworld = new b2World(gravity);
	box2Dworld->SetDebugDraw(debugDraw);
	box2Dworld->SetContactListener(contactListener);

	Game* game = new Game(window, renderer, WINDOW_WIDTH, WINDOW_HEIGHT, box2Dworld, textureManager, font);
	game->Init(dir_assets, dir_fonts, dir_textures);


	float speedup = 10.0f;
	bool steadyState = true;
	int generations = 3;
	int populationSize = 10;
	int crossover = game->ONEPOINT;
	float pMutate = 0.15f;
	float pCrossover = 0.95f;
	
	game->RunGA(dir_assets, dir_fonts, dir_textures, dt_fixed, speedup, steadyState, generations, populationSize, crossover, pMutate, pCrossover);
	speedup = 20.0f;
	pCrossover = 0.8f;
	game->RunGA(dir_assets, dir_fonts, dir_textures, dt_fixed, speedup, steadyState, generations, populationSize, crossover, pMutate, pCrossover);

	gameIsRunning = true;


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