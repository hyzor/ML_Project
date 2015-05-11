#ifndef MYGENOME_H_
#define MYGENOME_H_

#include <ga/ga.h>
#include <ga/GAGenome.h>
#include <ga/GARealGenome.h>
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
	MyGenome(int id, float(*objectiveFunc)(GAGenome&, GAPopulation&, bool**));
	MyGenome(const MyGenome& orig);
	virtual ~MyGenome();

	MyGenome& operator=(const GAGenome& orig);

	virtual GAGenome* clone(CloneMethod) const;
	virtual void copy(const GAGenome& orig);

	// Custom functions for MyGenome
	int GetID() const;
public:
	// Custom data for MyGenome
protected:
	int mID;

	GAPopulation* mGaPop;
	bool** mMatches;
	float(*mObjectiveFunc)(GAGenome&, GAPopulation&, bool**);
};

#endif