#include "MyGenome.h"

MyGenome::MyGenome()
	: GAGenome(Init, Mutate, Compare)
{
	evaluator(Evaluate);
	crossover(Cross);
	mID = -1;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
}

MyGenome::MyGenome(int id)
	: GAGenome(Init, Mutate, Compare)
{
	evaluator(Evaluate);
	crossover(Cross);
	mID = id;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
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

	// Copy parts of MyGenome
	MyGenome* origMyGenome = (MyGenome*)&orig;
	mID = origMyGenome->GetID();

	// Copy parts of Ship
}

MyGenome::~MyGenome()
{
}

void MyGenome::Init(GAGenome& genome)
{
	// TODO: Implement functionality
}

int MyGenome::Mutate(GAGenome& genome, float probability)
{
	// TODO: Implement functionality

	return -1;
}

float MyGenome::Compare(const GAGenome& genome1, const GAGenome& genome2)
{
	// TODO: Implement functionality

	// Measure and return the "distance" of the two genomes
	// Used for measuring diversity

	return -1;
}

float MyGenome::Evaluate(GAGenome& genome)
{
	// TODO: Implement functionality

	// Return some "score" based on the properties of this genome

	MyGenome* myGenome = (MyGenome*)&genome;

	float score = myGenome->mCurMatchesWon;

	return score;
}

int MyGenome::Cross(const GAGenome& _parent1, const GAGenome& _parent2,
	GAGenome* _result1, GAGenome* _result2)
{
	// TODO: Fix so that offsprings randomly inherit attributes, etc,
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

int MyGenome::GetID() const
{
	return mID;
}