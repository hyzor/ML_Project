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
#include <ga/ga.h>

#include <SDL/SDL.h>

using namespace std;

float objective(GAGenome &);

// Source: http://lancet.mit.edu/galib-2.4/examples/ex22.C
class SharedOverlapGA : public GASteadyStateGA
{
public:
	//GADefineIdentity("SharedOverlapGA", 200);

	SharedOverlapGA(const GAGenome& g) : GASteadyStateGA(g) {}
	virtual ~SharedOverlapGA() {}
	virtual void Step();
	SharedOverlapGA & operator++() { step(); return *this; }
	void Crossover();
};

int main(int argc, char **argv)
{
    cout << "Example 9\n\n";
    cout << "This program finds the maximum value in the function\n";
    cout << "  y = - x1^2 - x2^2\n";
    cout << "with the constraints\n";
    cout << "     -5 <= x1 <= 5\n";
    cout << "     -5 <= x2 <= 5\n";
    cout << "\n\n";
    cout.flush();

	// SDL
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	SDL_Surface* helloWorld = NULL;

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
		return -1;

	window = SDL_CreateWindow("GA Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);

	surface = SDL_GetWindowSurface(window);

	helloWorld = SDL_LoadBMP("assets/hello_world.bmp");

	SDL_BlitSurface(helloWorld, NULL, surface, NULL);

	SDL_UpdateWindowSurface(window);

/// See if we've been given a seed to use (for testing purposes).  When you
/// specify a random seed, the evolution will be exactly the same each time
/// you use that seed number.

    unsigned int seed = 0;
    for(int i = 1; i < argc; i++)
    {
        if(strcmp(argv[i++], "seed") == 0)
        {
            seed = atoi(argv[i]);
        }
    }

/// Declare variables for the GA parameters and set them to some default values.

    int popsize  = 30;
    int ngen     = 100;
    float pmut   = 0.01;
    float pcross = 0.6;

/// Create a phenotype for two variables.  The number of bits you can use to
/// represent any number is limited by the type of computer you are using.  In
/// this case, we use 16 bits to represent a floating point number whose value
/// can range from -5 to 5, inclusive.  The bounds on x1 and x2 can be applied
/// here and/or in the objective function.

    GABin2DecPhenotype map;
    map.add(16, -5, 5);
    map.add(16, -5, 5);

/// Create the template genome using the phenotype map we just made.

    GABin2DecGenome genome(map, objective);

/// Now create the GA using the genome and run it.  We'll use sigma truncation
/// scaling so that we can handle negative objective scores.

    GASimpleGA ga(genome);
    GASigmaTruncationScaling scaling;
    ga.populationSize(popsize);
    ga.nGenerations(ngen);
    ga.pMutation(pmut);
    ga.pCrossover(pcross);
    ga.scaling(scaling);
    ga.scoreFilename("bog.dat");
    ga.scoreFrequency(10);
    ga.flushFrequency(50);
    ga.evolve(seed);

/// Dump the results of the GA to the screen.

    genome = ga.statistics().bestIndividual();
    cout << "the ga found an optimum at the point (";
    cout << genome.phenotype(0) << ", " << genome.phenotype(1) << ")\n\n";
    cout << "best of generation data are in '" << ga.scoreFilename() << "'\n";

	cout << ga.statistics() << "\n";

	cin.get();

	SDL_FreeSurface(helloWorld);
	helloWorld = NULL;

	SDL_DestroyWindow(window);
	window = NULL;

	SDL_Quit();

    return 0;
}


/// This objective function tries to maximize the value of the function
///
///                  y = -(x1*x1 + x2*x2)
///
float
objective(GAGenome & c)
{
    GABin2DecGenome & genome = (GABin2DecGenome &)c;

    float y;
    y = -genome.phenotype(0) * genome.phenotype(0);
    y -= genome.phenotype(1) * genome.phenotype(1);
    return y;
}
