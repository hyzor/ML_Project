#include "MyGenome.h"

MyGenome::MyGenome()
	: GAGenome(Init, Mutate, Compare)
{
	evaluator(Evaluate);
	crossover(Cross);
}

MyGenome::MyGenome(const MyGenome& orig)
{
	copy(orig);
}

MyGenome& MyGenome::operator=(const GAGenome& orig)
{
	if (&orig != this)
	{
		copy(orig);
	}

	return *this;
}

GAGenome* MyGenome::clone(CloneMethod) const
{
	return new MyGenome(*this);
}

void MyGenome::copy(const GAGenome& orig)
{
	// Copy base genome parts
	GAGenome::copy(orig);

	// Copy parts of MyEntity

	// Copy parts of MyGenome
}

MyGenome::~MyGenome()
{
}

void MyGenome::Init(GAGenome&)
{
	// TODO: Implement functionality
}

int MyGenome::Mutate(GAGenome&, float)
{
	// TODO: Implement functionality

	return -1;
}

float MyGenome::Compare(const GAGenome&, const GAGenome&)
{
	// TODO: Implement functionality

	return -1;
}

float MyGenome::Evaluate(GAGenome&)
{
	// TODO: Implement functionality

	return -1;
}

int MyGenome::Cross(const GAGenome& _parent1, const GAGenome& _parent2,
	GAGenome* _result1, GAGenome* _result2)
{
	// TODO: Fix so that offsprings randomly inherit attributes etc
	// from both parents

	MyGenome& parent1 = (MyGenome&)_parent1;
	MyGenome& parent2 = (MyGenome&)_parent2;

	int numOffspring = 0;

	if (_result1)
	{
		MyGenome& result1 = (MyGenome&)* _result1;
		result1.copy(parent1); // Only copy parts from parent1 here
		result1.copy(parent2); // Only copy parts from parent2 here
		numOffspring++;
	}

	if (_result2)
	{
		MyGenome& result2 = (MyGenome&)* _result2;
		result2.copy(parent1); // Only copy parts from parent1 here
		result2.copy(parent2); // Only copy parts from parent2 here
		numOffspring++;
	}

	return numOffspring;
}