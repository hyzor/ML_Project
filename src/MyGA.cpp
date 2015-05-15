#include "MyGA.h"

MyGA::MyGA(const GAGenome& genome)
	: GASteadyStateGA(genome)
{}

MyGA::MyGA(const GAPopulation& pop)
	: GASteadyStateGA(pop)
{}

void MyGA::step()
{
	//std::cout << "Generation " << generation() << "\n";
	for (int i = 0; i < pop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&pop->individual(i);
		myGenome->mCurMatchesWon = 0;
		//myGenome->score(0.0f);
	}

	//int i;
	GAGenome* parent1;
	GAGenome* parent2;
	for (int i = 0; i < tmpPop->size(); ++i)
	{
		// Select two parents
		parent1 = &(pop->select());
		parent2 = &(pop->select());
		stats.numsel += 2;

		// Perform crossover with some probability
		if (GAFlipCoin(pCrossover()))
		{
			stats.numcro += mCrossoverFunc(*parent1, *parent2, &tmpPop->individual(i), nullptr);
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
	for (int i = 0; i < tmpPop->size(); ++i)
	{
		pop->add(tmpPop->individual(i));
	}

	// Get info about the current population (for next time)
	// and scale the population
	//pop->evaluate();

	// Our objective function, "population based"
	for (int i = 0; i < pop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&pop->individual(i);

		int k;
		float geneSum = 0.0f;
		float geneSum2 = 0.0f;
		for (k = 0; k < myGenome->length(); ++k)
		{
			geneSum += myGenome->gene(k);
		}

		/*
		if (myGenome->IsEvaluated())
		{
			continue;
		}
		*/

		for (int j = i; j < pop->size(); ++j)
		{
			if (i != j)
			{
				MyGenome* myGenome2 = (MyGenome*)&pop->individual(j);
				//std::cout << "Match: " << myGenome->GetID() << " vs " << myGenome2->GetID() << "\n";
				float geneSum2 = 0.0f;

				for (k = 0; k < myGenome2->length(); ++k)
				{
					geneSum2 += myGenome2->gene(k);
				}

				if (geneSum > geneSum2)
				{
					//myGenome->score(myGenome->score() + 1.0f);
					myGenome->mTotalMatchesWon++;
					myGenome->mCurMatchesWon++;
					//myGenome->score();
				}
				else if (geneSum < geneSum2)
				{
					//myGenome2->score(myGenome2->score() + 1.0f);
					myGenome2->mTotalMatchesWon++;
					myGenome2->mCurMatchesWon++;
				}
			}
		}
	}

	// Now evaluate the population
	pop->evaluate(gaTrue);

	pop->scale();

	for (int i = 0; i < tmpPop->size(); ++i)
	{
		//pop->destroy(GAPopulation::WORST, GAPopulation::RAW);
	}

	// Finally we update the stats by one generation
	stats.update(*pop);
}

void MyGA::crossover(CrossoverFunc func)
{
	mCrossoverFunc = func;
}