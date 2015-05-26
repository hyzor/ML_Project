#ifndef MYGA2_H_
#define MYGA2_H_

// Example: http://lancet.mit.edu/galib-2.4/examples/ex22.C

#include <ga/garandom.h>
#include <ga/GASimpleGA.h>

#include <Box2D/Box2D.h>

#include <chrono>

#include "MyGenome.h"
#include "Ship.h"

class Game;

typedef int(*CrossoverFunc)(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);

class MyGA2 : public GASimpleGA
{
public:

	MyGA2(const GAGenome& genome);
	MyGA2(const GAPopulation& pop);
	virtual ~MyGA2() {}
	virtual void step(float dt_fixed, float& speedup);
	//MyGA& operator++() { step(0.0f); return *this; }
	void crossover(CrossoverFunc func);
	void Init(Game* game, Ship* enemyShip);

	double ObjectiveFunction(GAGenome* genome, double dt_fixed, std::chrono::system_clock::time_point& time_now, float speedup);

	CrossoverFunc mCrossoverFunc;

protected:
	Ship* mEnemyShip;
	Game* mGame;
	//CrossoverFunc mCrossoverFunc;
};



#endif