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

#define PROB_MUTATION 0.01
#define PROB_CROSSOVER 1.0
#define NUM_GENERATIONS 100
#define POPULATION_SIZE 30
#define FREQ_SCORE 10
#define FREQ_FLUSH 50

#define FILENAME_SCORE "GA_score.dat"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <ga/ga.h>
#include <SDL/SDL.h>

#include "MyGA.h"

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

	// SDL
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	SDL_Surface* helloWorld = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;

	window = SDL_CreateWindow("GA Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if (!window)
		return -1;

	surface = SDL_GetWindowSurface(window);
	if (!surface)
		return -1;

	helloWorld = SDL_LoadBMP("assets/hello_world.bmp");
	if (!helloWorld)
		return -1;

	SDL_BlitSurface(helloWorld, NULL, surface, NULL);
	SDL_UpdateWindowSurface(window);

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

	std::cin.get();

	// Free SDL stuff
	SDL_FreeSurface(helloWorld);
	helloWorld = NULL;
	SDL_DestroyWindow(window);
	window = NULL;
	SDL_Quit();

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
