#include "MyGenome.h"

#include <ga/GARealGenome.C>

MyGenome::MyGenome(int id, GARealAlleleSetArray& setArray, float posX, float posY,
	int width, int height, int health, int damage, float angle,
	SDL_Wrapper::Texture* texture, GAGenome::Evaluator f /*= (GAGenome::Evaluator)0*/,
	GAGenome::Mutator fMut /*= (GAGenome::Mutator)0*/)
	: Ship(posX, posY, width, height, health, damage, angle, false, texture), GARealGenome(setArray, f)
{
	mID = id;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
	mScore = 0.0f;

	mutator(fMut);
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
	MyGenome* newGenome = new MyGenome(*this);
	newGenome->mb2Body = nullptr;
	newGenome->mSdlClipRect = nullptr;
	newGenome->mSdlCenterPoint = nullptr;

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

	myGenome->ClearWaypoints();

	for (int i = 0; i < myGenome->length()-1; i += 2)
	{
		myGenome->AddWaypoint(b2Vec2(myGenome->gene(i)*Box2dHelper::Units, myGenome->gene(i+1)*Box2dHelper::Units));
	}

	if (myGenome->mWaypoints.size() > 0)
		myGenome->mCurWaypoint = myGenome->mWaypoints.front();
}

int MyGenome::Mutate(GAGenome& genome, float probability)
{
	//return mMutatorFunc(genome, probability);
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
	MyGenome* _parent1MyGenome = (MyGenome*)&_parent1;
	MyGenome* _parent2MyGenome = (MyGenome*)&_parent2;

	int numOffspring = 0;

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