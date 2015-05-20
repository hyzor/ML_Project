#include "MyGenome.h"

#include <ga/GARealGenome.C>

/*
MyGenome::MyGenome(int id, GARealAlleleSetArray& setArray, float posX, float posY, GAGenome::Evaluator f)
	//: GAGenome(Init, Mutate, Compare)
	: GARealGenome(setArray, f)
{
	//initializer(Init);
	//evaluator(Evaluate);
	//crossover(Cross);
	mID = id;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
}
*/

MyGenome::MyGenome(int id, GARealAlleleSetArray& setArray, float posX, float posY, 
	int width, int height, int health, int damage, float angle,
	SDL_Wrapper::Texture* texture, GAGenome::Evaluator f /*= (GAGenome::Evaluator)0*/)
	: Ship(posX, posY, width, height, health, damage, angle, false, texture), GARealGenome(setArray, f)
{
	mID = id;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
	mScore = 0.0f;
}

/*
MyGenome::MyGenome(int id, GARealAlleleSetArray& setArray)
	//: GAGenome(Init, Mutate, Compare)
{
	//evaluator(Evaluate);
	//crossover(Cross);
	mID = id;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
	mAlleleSetArray = &setArray;
}
*/

/*
MyGenome::MyGenome(const MyGenome& orig)
{
	copy(orig);
}
*/

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
	MyGenome* newGenome = new MyGenome(*this);
	newGenome->mb2Body = nullptr;
	newGenome->mSdlClipRect = nullptr;
	newGenome->mSdlCenterPoint = nullptr;

	//newGenome->copy(*this);

	//newGenome->mWaypoints = this->mWaypoints;

	return newGenome;
}

void MyGenome::copy(const GAGenome& orig)
{
	// Copy base genome parts
	GAGenome::copy(orig);
	GARealGenome::copy(orig);

	// Copy parts of MyGenome
	MyGenome* origMyGenome = (MyGenome*)&orig;
	mID = origMyGenome->GetID();
	mCurMatchesWon = 0;
	mTotalMatchesWon = 0;
	//mSdlClipRect = nullptr;
	//mSdlCenterPoint = nullptr;
	//mb2Body = nullptr;
	//mAlleleSetArray = new GARealAlleleSetArray(*origMyGenome->GetAlleleSetArray());
	//m1DArrayAlleleGenome = new GA1DArrayAlleleGenome<float>(*origMyGenome->Get1DArrayAlleleGenome());

	// Copy parts of Ship
	mWaypoints.clear();
	mWaypoints = origMyGenome->mWaypoints;
}

void MyGenome::SetScore(float score)
{
	mScore = score;
}

float MyGenome::GetScore() const
{
	return mScore;
}

MyGenome::~MyGenome()
{
}

void MyGenome::Init(GAGenome& genome)
{
	// TODO: Implement functionality

	MyGenome* myGenome = (MyGenome*)&genome;

	myGenome->mWaypoints.clear();

	for (int i = 0; i < myGenome->length(); i += 2)
	{
		myGenome->AddWaypoint(b2Vec2(myGenome->gene(i)*Box2dHelper::Units, myGenome->gene(i+1)*Box2dHelper::Units));
	}
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

	myGenome->_evaluated = gaTrue;

	return myGenome->GetScore();
}

int MyGenome::Cross(const GAGenome& _parent1, const GAGenome& _parent2,
	GAGenome* _result1, GAGenome* _result2)
{
	// TODO: Fix so that offsprings randomly inherit attributes, etc,
	// from both parents

	MyGenome* _parent1MyGenome = (MyGenome*)&_parent1;
	MyGenome* _parent2MyGenome = (MyGenome*)&_parent2;

	int numOffspring = 0;

	/*
	GA1DArrayAlleleGenome<float>* allele1 = _parent1MyGenome->Get1DArrayAlleleGenome();
	GA1DArrayAlleleGenome<float>* allele2 = _parent2MyGenome->Get1DArrayAlleleGenome();

	int numOffspring = 0;

	numOffspring = GA1DArrayGenome<float>::TwoPointCrossover((GAGenome)*_parent1MyGenome->Get1DArrayAlleleGenome(),
		(GAGenome)*_parent2MyGenome->Get1DArrayAlleleGenome(), _result1, _result2);
		*/

	//std::cout << setArray.set(0) << "\n";
	//std::cout << setArray.set(0).lower() << "\n";
	//std::cout << setArray.set(0).upper() << "\n";
	/*

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
	*/

	return numOffspring;
}

int MyGenome::GetID() const
{
	return mID;
}

GABoolean MyGenome::IsEvaluated() const
{
	return _evaluated;
}

void MyGenome::Reset()
{
	_evaluated = gaFalse;
	mCurMatchesWon = 0;
}