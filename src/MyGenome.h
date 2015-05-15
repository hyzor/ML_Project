#ifndef MYGENOME_H_
#define MYGENOME_H_

#include <ga/GARealGenome.h>
#include "Ship.h"

/*
/// We must also specialize the allele set so that the alleles are handled
/// properly.  Be sure to handle bounds correctly whether we are discretized
/// or continuous.  Handle the case where someone sets stupid bounds that
/// might cause an infinite loop for exclusive bounds.
template <> float
GAAlleleSet<float>::allele() const
{
	float value = 0.0;
	if (core->type == GAAllele::ENUMERATED)
	{
		value = core->a[GARandomInt(0, core->sz - 1)];
	}
	else if (core->type == GAAllele::DISCRETIZED)
	{
		float n = (core->a[1] - core->a[0]) / core->a[2];
		int m = (int)n;
		if (core->lowerb == GAAllele::EXCLUSIVE)
		{
			m -= 1;
		}
		if (core->upperb == GAAllele::EXCLUSIVE)
		{
			m -= 1;
		}
		value = core->a[0] + GARandomInt(0, (int)m) * core->a[2];
		if (core->lowerb == GAAllele::EXCLUSIVE)
		{
			value += core->a[2];
		}
	}
	else
	{
		if (core->a[0] == core->a[1] &&
			core->lowerb == GAAllele::EXCLUSIVE &&
			core->upperb == GAAllele::EXCLUSIVE)
		{
			value = core->a[0];
		}
		else
		{
			do
			{
				value = GARandomFloat(core->a[0], core->a[1]);
			} while ((core->lowerb == GAAllele::EXCLUSIVE && value == core->a[0]) ||
				(core->upperb == GAAllele::EXCLUSIVE && value == core->a[1]));
		}
	}
	return value;
}
*/

// Custom genome class which is derived from a custom entity class
// and the default GAGenome class
class MyGenome : public Ship, public GARealGenome
{
	// Default genome functions/operators definitions
public:
	static void Init(GAGenome&);
	static int Mutate(GAGenome&, float);
	static float Compare(const GAGenome&, const GAGenome&);
	static float Evaluate(GAGenome&);
	static int Cross(const GAGenome&, const GAGenome&, GAGenome*, GAGenome*);

public:
	MyGenome(int id, GARealAlleleSetArray& setArray, GAGenome::Evaluator f = (GAGenome::Evaluator)0);
	//MyGenome(int id, GARealAlleleSetArray& setArray);
	//MyGenome(const MyGenome& orig);
	//MyGenome& operator=(const GAGenome& arr);
	virtual ~MyGenome();

	MyGenome& operator=(const GAGenome& orig);

	virtual GAGenome* clone(CloneMethod) const;
	virtual void copy(const GAGenome& orig);

	// Custom functions for MyGenome
	int GetID() const;
	GABoolean IsEvaluated() const;
	//GARealAlleleSetArray* GetAlleleSetArray() const;

	//GA1DArrayAlleleGenome<float>* Get1DArrayAlleleGenome() const;
	
public:
	// Custom data for MyGenome
	int mTotalMatchesWon;
	int mCurMatchesWon;

protected:
	int mID;

	//GARealAlleleSetArray* mAlleleSetArray;
	//GA1DArrayAlleleGenome<float>* m1DArrayAlleleGenome;
};

#endif