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

//#include <ga/GARealGenome.C>

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

#include "SDL_Wrapper.h"
#include "World.h"
#include "TextureManager.h"
#include "MyB2DebugDraw.h"
#include "MyB2ContactListener.h"

#include "Game.h"

#define INSTANTIATE_REAL_GENOME true

// Directories
static const std::string dir_assets = "assets/";
static const std::string dir_fonts = dir_assets + "fonts/";
static const std::string dir_textures = dir_assets;

// Window
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main(int argc, char **argv)
{
	bool useCmdArguments = false;

	float arg_speedup = 20.0f;
	bool arg_doDraw = true;
	bool arg_isGenomeTestRun = false;
	std::string arg_fileName_Genes = "";

	int arg_numGenerations = 5;
	int arg_populationSize = 10;

	if (useCmdArguments && argc < 7)
	{
		std::cout << "Usage: " << argv[0] << " SPEEDUP DRAW_FLAG RUN_NUM_GENERATIONS RUN_POPSIZE TESTRUN_FLAG TESTRUN_GENE_FILE" << "\n";
		std::cout << "Press ENTER to exit...\n";
		std::cin.get();
		return -1;
	}

	if (useCmdArguments)
	{
		arg_speedup = (float)std::atoi(argv[1]);
		arg_doDraw = (std::atoi(argv[2]) != 0);
		arg_numGenerations = std::atoi(argv[3]);
		arg_populationSize = std::atoi(argv[4]);
		arg_isGenomeTestRun = (std::atoi(argv[5]) != 0);
		arg_fileName_Genes = argv[6];
	}

	srand(time(NULL));

	GARandomSeed();

	bool gameIsRunning = false;

	// SDL
	SDL_Window* window = nullptr;
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

	bool steadyState = true;
	int generations = arg_numGenerations;
	int populationSize = arg_populationSize;
	int crossover = Game::CROSSOVER_REAL_ONEPOINT;
	float pMutate = 1.0f;
	float pCrossover = 1.0f;
	int mutatorType = Game::MUTATOR_REAL_GAUSSIAN;


	float mScreenWidth = 800.0f;
	float mScreenHeight = 600.0f;

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
	
	// Test genomes by running similar code as below
	if (arg_isGenomeTestRun)
	{
		std::cout << "Running gene test\n";
		std::vector<float> genes;
		std::ifstream infile(arg_fileName_Genes);

		float curGene = 0.0f;
		double score = 0.0;

		if (infile.is_open())
		{
			while (infile >> curGene)
			{
				genes.push_back(curGene);
			}
		}
		else
		{
			std::cout << "Could not open file " << arg_fileName_Genes << "!\n";
			std::cout << "Using random genes instead\n";

			for (unsigned int i = 0; i < 9; i += 2)
			{
				genes.push_back(rand() % 800);
				genes.push_back(rand() % 600);
			}
			genes.push_back(0.5f);

		}

		score = game->RunGenomeFromGeneSet(genes, dt_fixed, arg_speedup, arg_doDraw);

		std::cout << "Gene test score: " << score << "\n";
	}
	else
	{
		unsigned int numIterations = 3;

		std::cout << "Running normal version. Information:\n-----------------------------------------------\n";
		std::cout << "Speedup: " << arg_speedup << "\n";
		std::cout << "Generations: " << generations << "\nPopulation size: " << populationSize << "\n";
		std::cout << "Crossover functions: " << "One point, two point & blend\n";
		std::cout << "Mutation functions: " << "Swap, uniform & Gaussian\n";
		std::cout << "Iterations: " << numIterations << "\n";
		std::cout << "-----------------------------------------------\n";

		for (unsigned int i = 0; i < 3; ++i)
		{
			for (unsigned int j = 0; j < 3; ++j)
			{
				for (unsigned int k = 0; k < numIterations; ++k)
				{
					game->RunGA(dir_assets, dir_fonts, dir_textures, dt_fixed, arg_speedup, steadyState, generations, populationSize, i, j, pMutate, pCrossover, k, arg_doDraw);
				}
			}
		}

		steadyState = false;
		for (unsigned int i = 0; i < 3; ++i)
		{
			for (unsigned int j = 0; j < 3; ++j)
			{
				for (unsigned int k = 0; k < numIterations; ++k)
				{
					game->RunGA(dir_assets, dir_fonts, dir_textures, dt_fixed, arg_speedup, steadyState, generations, populationSize, i, j, pMutate, pCrossover, k, arg_doDraw);
				}
			}
		}
	}

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
