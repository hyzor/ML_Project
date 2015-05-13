#ifndef MYGENOME_H_
#define MYGENOME_H_

#include <ga/ga.h>
#include <ga/GAGenome.h>
#include <ga/GARealGenome.h>
#include <ga/GA1DArrayGenome.h>
#include "Ship.h"

// Custom genome class which is derived from a custom entity class
// and the default GAGenome class
class MyGenome : public Ship, public GAGenome
{
	// Default genome functions/operators definitions
public:
	static void Init(GAGenome&);
	static int Mutate(GAGenome&, float);
	static float Compare(const GAGenome&, const GAGenome&);
	static float Evaluate(GAGenome&);
	static int Cross(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);

public:
	MyGenome();
	MyGenome(int id, GARealAlleleSetArray& setArray);
	MyGenome(const MyGenome& orig);
	virtual ~MyGenome();

	MyGenome& operator=(const GAGenome& orig);

	virtual GAGenome* clone(CloneMethod) const;
	virtual void copy(const GAGenome& orig);

	// Custom functions for MyGenome
	int GetID() const;
	GABoolean IsEvaluated() const;
	GARealAlleleSetArray* GetAlleleSetArray() const;

	GA1DArrayAlleleGenome<float>* Get1DArrayAlleleGenome() const;
	
public:
	// Custom data for MyGenome
	int mTotalMatchesWon;
	int mCurMatchesWon;

protected:
	int mID;

	GARealAlleleSetArray* mAlleleSetArray;
	GA1DArrayAlleleGenome<float>* m1DArrayAlleleGenome;
};

#endif