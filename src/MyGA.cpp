#include "MyGA.h"

#include "Game.h"

MyGA::MyGA(const GAGenome& genome)
	: GASteadyStateGA(genome)
{}

MyGA::MyGA(const GAPopulation& pop)
	: GASteadyStateGA(pop)
{}

void MyGA::step(float dt)
{
	//std::cout << "Generation " << generation() << "\n";

		//mPlayerShip = mWorld->SpawnEntity<Ship>(mScreenWidth / 2, mScreenHeight / 2, 48,
		//64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"));

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
			//stats.numcro += mCrossoverFunc(*parent1, *parent2, &tmpPop->individual(i), nullptr);
		}

		// If no crossover, copy from parent1 with a probability of
		// 50% (random bit means 1 or 0)...
		else if (GARandomBit())
		{
			//tmpPop->individual(i).copy(*parent1);
		}

		// ...or copy from parent2 if GARandomBit() == 0
		else
		{
			//tmpPop->individual(i).copy(*parent2);
		}

		// Perform mutation and keep track of it in stats
		stats.nummut += tmpPop->individual(i).mutate(pMutation());
	}

	// Add all the individuals in the temp population to the
	// actual population
	for (int i = 0; i < tmpPop->size(); ++i)
	{
		//pop->add(tmpPop->individual(i));
	}

	for (int i = 0; i < pop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&pop->individual(i);
		myGenome->Reset();
		MyGenome::Init(*myGenome);
		//myGenome->mCurMatchesWon = 0;
		//myGenome->score(0.0f);
	}

	SDL_Event sdlEvent;

	// Our objective function, "population based"
	for (int i = 0; i < pop->size(); ++i)
	{
		MyGenome* myGenome = &(MyGenome&)pop->individual(i);
		mGame->GetWorld()->AddEntity((Ship*)myGenome);

		bool gameIsRunning = true;

		std::cout << "Running generation " << generation() << " game " << i << "...\n";

		//int k;
		/*
		float geneSum = 0.0f;
		float geneSum2 = 0.0f;
		for (int k = 0; k < myGenome->length(); ++k)
		{
			geneSum += myGenome->gene(k);
		}
		*/

		int iterations = 0;

		while (gameIsRunning && iterations < 1000)
		{
			while (SDL_PollEvent(&sdlEvent) != 0)
			{
				if (sdlEvent.type == SDL_KEYDOWN)
				{
					switch (sdlEvent.key.keysym.sym)
					{
					case SDLK_ESCAPE:
						std::cout << "Aborting current game...\n";
						gameIsRunning = false;
						break;
					}
				}
			}

			//std::cout << myGenome->GetHealth() << "\n";

			mEnemyShip->SetTarget(myGenome->GetPosition(true));
			mGame->Update(dt);

			if (!myGenome->IsAlive() || !mEnemyShip->IsAlive())
			{
				break;
			}

			mGame->Draw();
			//mEnemyShip->RotateTo_Torque(myGenome->GetPosition(true), dt);

			//mGame->GetWorld()->Flush();

			iterations++;
		}

		std::cout << "Finished generation " << generation() << " game " << i << "\n";

		float score = myGenome->GetHealth() - mEnemyShip->GetHealth();

		myGenome->SetScore(score);

		((Ship*)myGenome)->Reset();
		mEnemyShip->Reset();

		mGame->GetWorld()->Flush();

		mGame->GetWorld()->RemoveEntity((Ship*)myGenome);
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

void MyGA::Init(Game* game, Ship* enemyShip)
{
	mGame = game;
	mEnemyShip = enemyShip;
}
