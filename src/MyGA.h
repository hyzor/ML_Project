#ifndef MYGA_H_
#define MYGA_H_

// Example: http://lancet.mit.edu/galib-2.4/examples/ex22.C

#include <ga/garandom.h>
#include <ga/GASStateGA.h>

#include <Box2D/Box2D.h>

#include "MyGenome.h"
#include "Ship.h"

class Game;

typedef int(*CrossoverFunc)(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);

class MyGA : public GASteadyStateGA
{
public:

	MyGA(const GAGenome& genome);
	MyGA(const GAPopulation& pop);
	virtual ~MyGA() {}
	virtual void step(float dt);
	//MyGA& operator++() { step(0.0f); return *this; }
	void crossover(CrossoverFunc func);
	void Init(Game* game, Ship* enemyShip);

	float ObjectiveFunction(GAGenome* genome, float dt);

	CrossoverFunc mCrossoverFunc;

protected:
	Ship* mEnemyShip;
	Game* mGame;
	//CrossoverFunc mCrossoverFunc;
};

#endif