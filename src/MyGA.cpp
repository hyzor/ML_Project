#include "MyGA.h"

#include "Game.h"

MyGA::MyGA(const GAGenome& genome)
	: GASteadyStateGA(genome)
{}

MyGA::MyGA(const GAPopulation& pop)
	: GASteadyStateGA(pop)
{}

void MyGA::step(float dt_fixed, float& speedup, bool doDraw)
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
		//MyGenome* newGenome = (MyGenome*)&tmpPop->individual(i);

		pop->add(tmpPop->individual(i));
	}

	for (int i = 0; i < pop->size(); ++i)
	{
		MyGenome* myGenome = (MyGenome*)&pop->individual(i);
		MyGenome::Init(*myGenome);
		myGenome->Init_b2(mGame->GetWorld()->Getb2World(), false, Entity::Type::SHIP, myGenome->gene(myGenome->length()-1));
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

		float score = ObjectiveFunction(myGenome, dt_fixed, std::chrono::system_clock::now(), speedup, mGame, mEnemyShip, doDraw);
		//std::cout << "Finished generation " << generation() << " game " << i << " (Score: " << score << ")\n";
		myGenome->SetScore(score);
	}

	// Now evaluate the population
	pop->evaluate(gaTrue);

	pop->diversity();
	pop->scale();

	//std::cout << pop->div() << "\n";

	for (int i = 0; i < tmpPop->size(); ++i)
	{
		//pop->destroy(GAPopulation::WORST, GAPopulation::RAW);
		MyGenome* myGenome = (MyGenome*)&pop->worst();
		mGame->GetWorld()->Getb2World()->DestroyBody(myGenome->Getb2Body());
		pop->destroy(GAPopulation::WORST, GAPopulation::RAW);
	}

	// Finally we update the stats by one generation
	stats.update(*pop);

	mDiversities.push_back(pop->div());

	//std::cout << statistics() << "\n";
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

double MyGA::ObjectiveFunction(GAGenome* genome, double dt_fixed, std::chrono::system_clock::time_point& time_now, float speedup, Game* game, Ship* enemyShip, bool doDraw)
{
	MyGenome* myGenome = (MyGenome*)genome;
	game->GetWorld()->AddEntity((Ship*)myGenome);

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
	double frametime_accumulator = 0.0;

	while (gameIsRunning && time < 25.0 && myGenome->IsAlive() && enemyShip->IsAlive())
	{
		game->GetWorld()->Flush();

		auto time_new = std::chrono::system_clock::now();
		time_since_start = time_new - time_start;
		std::chrono::duration<double> frame_diff = time_new - time_now;
		double frametime = frame_diff.count();

		frametime *= (double)speedup;

		if (frametime > 0.75)
		{
			frametime_accumulator += frametime - 0.75;
			frametime = 0.75;
		}
			

		//frametime = std::min(dt_fixed*speedup, 0.25f);

		//frametime = std::chrono::duration_cast<std::chrono::milliseconds>(frame_diff).count();

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
			enemyShip->SetTarget(myGenome->GetPosition(true));
			game->UpdateWorld(dt_fixed);

			game->DoPhysicsStep(dt_fixed, Box2dHelper::b2VelIterations, Box2dHelper::b2PosIterations);
			physicsIterations++;

			accumulator -= dt_fixed;
		}

		const double alpha = accumulator / dt_fixed;

		game->GetWorld()->Getb2World()->ClearForces();

		//std::cout << myGenome->GetHealth() << "\n";
		//mEnemyShip->SetPosition(b2Vec2(800 * Box2dHelper::Units, 600 * Box2dHelper::Units));

		if (doDraw)
			game->Draw(alpha);

		game->DoPostProcessing();
		//mEnemyShip->RotateTo_Torque(myGenome->GetPosition(true), dt);

		time += frametime;

		iterations++;
	}

	std::chrono::system_clock::time_point end_time = std::chrono::system_clock::now();

	std::chrono::duration<double> frame_diff = end_time - time_start;

	//std::cout << "Time elapsed (s): " << frame_diff.count() << "\n";

	//std::cout << "Total frame time: " << time << "\n";

	//std::cout << myGenome->GetHealth() << " " << enemyShip->GetHealth() << "\n";

	//float score_time = ((float)iterations*speedup) * 0.01f;
	float score_time = (float)time * 0.1f;

	double score = (double)(myGenome->GetHealth_Init() + myGenome->GetHealth() - enemyShip->GetHealth()) + score_time;

	if (score < 0.0)
		score = 0.0;

	//myGenome->SetScore(score);

	myGenome->SetCollisionEnabled(false);
	((Ship*)myGenome)->Reset();
	enemyShip->Reset();
	myGenome->Setb2BodyType(b2_staticBody);

	game->GetWorld()->Flush();

	game->GetWorld()->RemoveEntity((Ship*)myGenome);

	return score;
}

const std::vector<float>& MyGA::GetDiversities() const
{
	return mDiversities;
}
