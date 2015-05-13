#ifndef MYGA_H_
#define MYGA_H_

// Example: http://lancet.mit.edu/galib-2.4/examples/ex22.C

#include <ga/garandom.h>
#include <ga/GASStateGA.h>

#include "MyGenome.h"

typedef int(*CrossoverFunc)(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);

class MyGA : public GASteadyStateGA
{
public:

	MyGA(const GAGenome& genome);
	MyGA(const GAPopulation& pop);
	virtual ~MyGA() {}
	virtual void step();
	MyGA& operator++() { step(); return *this; }
	void crossover(CrossoverFunc func);

protected:
	CrossoverFunc mCrossoverFunc;
};

#endif