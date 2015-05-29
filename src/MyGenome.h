#ifndef MYGENOME_H_
#define MYGENOME_H_

#include <ga/GARealGenome.h>
#include "Ship.h"

// Custom genome class which is derived from a custom entity class
// and the default GAGenome class
class MyGenome : public Ship, public GARealGenome
{
public:
	// Default genome functions/operators definitions
public:
	static void Init(GAGenome&);
	static int Mutate(GAGenome&, float);
	static float Compare(const GAGenome&, const GAGenome&);
	static float Evaluate(GAGenome&);
	static int Cross(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);

public:
	MyGenome(int id, GARealAlleleSetArray& setArray, float posX, float posY,
		int width, int height, int health, int damage, float angle,
		SDL_Wrapper::Texture* texture, GAGenome::Evaluator f = (GAGenome::Evaluator)0, 
		GAGenome::Mutator fMut = (GAGenome::Mutator)0);

	virtual ~MyGenome();

	MyGenome& operator=(const GAGenome& orig);

	virtual GAGenome* clone(CloneMethod) const;
	virtual void copy(const GAGenome& orig);

	void SetScore(float score);
	float GetScore() const;

	// Custom functions for MyGenome
	int GetID() const;
	GABoolean IsEvaluated() const;

	void Reset();
	
public:
	// Custom data for MyGenome
	int mTotalMatchesWon;
	int mCurMatchesWon;
	float mScore;

protected:
	int mID;
};

#endif