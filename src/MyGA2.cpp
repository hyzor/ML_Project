#include "MyGA2.h"

#include "Game.h"

MyGA2::MyGA2(const GAGenome& genome)
	: GASimpleGA(genome)
{}

MyGA2::MyGA2(const GAPopulation& pop)
	: GASimpleGA(pop)
{}

void MyGA2::step(float dt_fixed, float& speedup)
{
	//std::cout << "Generation " << generation() << "\n";

	//mPlayerShip = mWorld->SpawnEntity<Ship>(mScreenWidth / 2, mScreenHeight / 2, 48,
	//64, 5, 1, 0.0f, mTextureManager->LoadTexture("Ship.png"));

	//int i;
	int i, mut, c1, c2;
	GAGenome *mom, *dad;          /// tmp holders for selected genomes

	GAPopulation *tmppop;		/// Swap the old population with the new pop.
	tmppop = oldPop;		/// When we finish the ++ we want the newly
	oldPop = pop;			/// generated population to be current (for
	pop = tmppop;			/// references to it from member functions).

	/// Generate the individuals in the temporary population from individuals in
	/// the main population.

	for (i = 0; i < pop->size() - 1; i += 2) 	/// takes care of odd population
	{
		mom = &(oldPop->select());
		dad = &(oldPop->select());
		stats.numsel += 2;		/// keep track of number of selections

		c1 = c2 = 0;
		if (GAFlipCoin(pCrossover()))
		{
			stats.numcro += (mCrossoverFunc)(*mom, *dad,
				&pop->individual(i), &pop->individual(i + 1));
			c1 = c2 = 1;
		}
		else
		{
			pop->individual(i).copy(*mom);
			pop->individual(i + 1).copy(*dad);
		}
		stats.nummut += (mut = pop->individual(i).mutate(pMutation()));
		if (mut > 0)
		{
			c1 = 1;
		}
		stats.nummut += (mut = pop->individual(i + 1).mutate(pMutation()));
		if (mut > 0)
		{
			c2 = 1;
		}

		stats.numeval += c1 + c2;
	}
	if (pop->size() % 2 != 0) 	/// do the remaining population member
	{
		mom = &(oldPop->select());
		dad = &(oldPop->select());
		stats.numsel += 2;		/// keep track of number of selections

		c1 = 0;
		if (GAFlipCoin(pCrossover()))
		{
			stats.numcro += (*scross)(*mom, *dad, &pop->individual(i), (GAGenome*)0);
			c1 = 1;
		}
		else
		{
			if (GARandomBit())
			{
				pop->individual(i).copy(*mom);
			}
			else
			{
				pop->individual(i).copy(*dad);
			}
		}
		stats.nummut += (mut = pop->individual(i).mutate(pMutation()));
		if (mut > 0)
		{
			c1 = 1;
		}

		stats.numeval += c1;
	}

	stats.numrep += pop->size();

	for (int i = 0; i < pop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&pop->individual(i);
		MyGenome::Init(*myGenome);
		myGenome->Init_b2(mGame->GetWorld()->Getb2World(), false, Entity::Type::SHIP, myGenome->gene(myGenome->length() - 1));
		myGenome->Init_SDL();
		myGenome->SetType(Entity::Type::SHIP);
		myGenome->Reset();

		myGenome->SetCollisionEnabled(false);
		((Ship*)myGenome)->Reset();
		myGenome->Setb2BodyType(b2_staticBody);
		//myGenome->mCurMatchesWon = 0;
		//myGenome->score(0.0f);
	}

	// Our objective function, "population based"
	for (int i = 0; i < pop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&pop->individual(i);
		myGenome->Setb2BodyType(b2_dynamicBody);
		myGenome->SetCollisionEnabled(true);

		float score = ObjectiveFunction(myGenome, dt_fixed, std::chrono::system_clock::now(), speedup);
		std::cout << "Finished generation " << generation() << " game " << i << " (Score: " << score << ")\n";
		myGenome->SetScore(score);
	}

	// Now evaluate the population
	pop->evaluate(gaTrue);

	pop->diversity();
	pop->scale();


	if (el && oldPop->best().score() > pop->best().score())
		oldPop->replace(pop->replace(&(oldPop->best()), GAPopulation::WORST), GAPopulation::BEST);

	//std::cout << pop->div() << "\n";


	// Finally we update the stats by one generation
	stats.update(*pop);

	//std::cout << statistics() << "\n";
}

void MyGA2::crossover(CrossoverFunc func)
{
	mCrossoverFunc = func;
}

void MyGA2::Init(Game* game, Ship* enemyShip)
{
	mGame = game;
	mEnemyShip = enemyShip;
}

double MyGA2::ObjectiveFunction(GAGenome* genome, double dt_fixed, std::chrono::system_clock::time_point& time_now, float speedup)
{
	MyGenome* myGenome = (MyGenome*)genome;
	mGame->GetWorld()->AddEntity((Ship*)myGenome);

	SDL_Event sdlEvent;

	bool gameIsRunning = true;

	//std::cout << "Running generation " << generation() << " game " << i << "...\n";

	int iterations = 0;
	double time = 0.0;

	std::chrono::system_clock::time_point time_start = std::chrono::system_clock::now();

	double accumulator = 0.0;

	//auto time_new = std::chrono::system_clock::now();
	std::chrono::duration<double> time_since_start;

	int physicsIterations = 0;

	while (gameIsRunning && time < 25.0 && myGenome->IsAlive() && mEnemyShip->IsAlive())
	{
		mGame->GetWorld()->Flush();

		auto time_new = std::chrono::system_clock::now();
		time_since_start = time_new - time_start;
		std::chrono::duration<double> frame_diff = time_new - time_now;
		double frametime = frame_diff.count();

		//frametime = std::min(dt_fixed*speedup, 0.25f);

		//frametime = std::chrono::duration_cast<std::chrono::milliseconds>(frame_diff).count();
		frametime *= (double)speedup;

		if (frametime > 0.5)
			frametime = 0.5;

		time_now = time_new;

		accumulator += frametime;

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

		while (accumulator >= dt_fixed)
		{
			mEnemyShip->SetTarget(myGenome->GetPosition(true));
			mGame->UpdateWorld(dt_fixed);

			mGame->DoPhysicsStep(dt_fixed, Box2dHelper::b2VelIterations, Box2dHelper::b2PosIterations);
			physicsIterations++;

			accumulator -= dt_fixed;
		}

		const double alpha = accumulator / dt_fixed;

		mGame->GetWorld()->Getb2World()->ClearForces();

		//std::cout << myGenome->GetHealth() << "\n";
		//mEnemyShip->SetPosition(b2Vec2(800 * Box2dHelper::Units, 600 * Box2dHelper::Units));

		mGame->Draw(alpha);
		mGame->DoPostProcessing();
		//mEnemyShip->RotateTo_Torque(myGenome->GetPosition(true), dt);

		time += frametime;

		iterations++;
	}

	std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();

	std::chrono::duration<double> frame_diff = end_time - time_start;

	std::cout << "Time elapsed (s): " << frame_diff.count() << "\n";

	std::cout << "Total frame time: " << time << "\n";

	std::cout << myGenome->GetHealth() << " " << mEnemyShip->GetHealth() << "\n";

	double score = (double)(myGenome->GetHealth_Init() + myGenome->GetHealth() - mEnemyShip->GetHealth()) + ((float)iterations * 0.01f);

	if (score < 0.0)
		score = 0.0;

	//myGenome->SetScore(score);

	myGenome->SetCollisionEnabled(false);
	((Ship*)myGenome)->Reset();
	mEnemyShip->Reset();
	myGenome->Setb2BodyType(b2_staticBody);

	mGame->GetWorld()->Flush();

	mGame->GetWorld()->RemoveEntity((Ship*)myGenome);

	return score;
}
