#include "MyGA.h"

void MyGA::step()
{
	int i;
	GAGenome* parent1;
	GAGenome* parent2;

	for (i = 0; i < tmpPop->size(); ++i)
	{
		// Select two parents
		parent1 = &(pop->select());
		parent2 = &(pop->select());
		stats.numsel += 2;

		// Perform crossover with some probability
		if (GAFlipCoin(pCrossover()))
		{
			stats.numcro += mCrossoverFunc(*parent1, *parent2, &tmpPop->individual(i));
		}

		// If no crossover, copy from parent1 with a probability of
		// 50% (random bit means 1 or 0)...
		else if (GARandomBit())
		{
			tmpPop->individual(i).copy(*parent1);
		}

		// ...or copy from parent2 if GARandomBit() == 0
		else
		{
			tmpPop->individual(i).copy(*parent2);
		}

		// Perform mutation and keep track of it in stats
		stats.nummut += tmpPop->individual(i).mutate(pMutation());
	}

	// Add all the individuals in the temp population to the
	// actual population
	for (i = 0; i < tmpPop->size(); ++i)
	{
		pop->add(tmpPop->individual(i));
	}

	// Get info about the current population (for next time)
	// and scale the population
	pop->evaluate();
	pop->scale();

	for (i = 0; i < tmpPop->size(); ++i)
	{
		pop->destroy(GAPopulation::WORST, GAPopulation::SCALED);
	}

	// Finally we update the stats by one generation
	stats.update(*pop);
}

void MyGA::crossover(CrossoverFunc func)
{
	mCrossoverFunc = func;
}