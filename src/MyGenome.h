#ifndef MYGENOME_H_
#define MYGENOME_H_

#include <ga/GAGenome.h>
#include "MyEntity.h"

// Custom genome class which is derived from a custom entity class
// and the default GAGenome class
class MyGenome : public MyEntity, public GAGenome
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
	MyGenome(const MyGenome& orig);
	virtual ~MyGenome();

	MyGenome& operator=(const GAGenome& orig);

	virtual GAGenome* clone(CloneMethod) const;
	virtual void copy(const GAGenome& orig);

	// Custom functions for MyGenome
public:

	// Custom data for MyGenome
protected:

};

#endif