#include "MyGenome.h"

/// We must also specialize the allele set so that the alleles are handled
/// properly.  Be sure to handle bounds correctly whether we are discretized
/// or continuous.  Handle the case where someone sets stupid bounds that
/// might cause an infinite loop for exclusive bounds.
#ifndef MYALLELE_H_
#define MYALLELE_H_
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
#endif

MyGenome::MyGenome()
	: GAGenome(Init, Mutate, Compare)
{
	evaluator(Evaluate);
	crossover(Cross);
	mID = -1;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
}

MyGenome::MyGenome(int id, GARealAlleleSetArray& setArray)
	: GAGenome(Init, Mutate, Compare)
{
	evaluator(Evaluate);
	crossover(Cross);
	mID = id;
	mTotalMatchesWon = 0;
	mCurMatchesWon = 0;
	mAlleleSetArray = &setArray;
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
	mAlleleSetArray = origMyGenome->GetAlleleSetArray();
	m1DArrayAlleleGenome = origMyGenome->Get1DArrayAlleleGenome();

	// Copy parts of Ship
}

MyGenome::~MyGenome()
{
}

void MyGenome::Init(GAGenome& genome)
{
	// TODO: Implement functionality

	MyGenome* myGenome = (MyGenome*)&genome;

	myGenome->m1DArrayAlleleGenome = new GA1DArrayAlleleGenome<float>(*myGenome->mAlleleSetArray, nullptr);

	//child.resize(GAGenome::ANY_SIZE); // let chrom resize if it can
	myGenome->m1DArrayAlleleGenome->resize(GAGenome::ANY_SIZE);
	for (int i = myGenome->m1DArrayAlleleGenome->length() - 1; i >= 0; i--)
	{
		//child.gene(i, child.alleleset(i).allele());
		myGenome->m1DArrayAlleleGenome->gene(i, myGenome->m1DArrayAlleleGenome->alleleset(i).allele());
	}

	std::cout << "Gene:\n";
	for (int i = 0; i < myGenome->m1DArrayAlleleGenome->length(); ++i)
	{
		std::cout << myGenome->m1DArrayAlleleGenome->gene(i) << "\n";
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

	float score = myGenome->mCurMatchesWon;

	//myGenome->_evaluated = gaTrue;

	return score;
}

int MyGenome::Cross(const GAGenome& _parent1, const GAGenome& _parent2,
	GAGenome* _result1, GAGenome* _result2)
{
	// TODO: Fix so that offsprings randomly inherit attributes, etc,
	// from both parents



	//std::cout << setArray.set(0) << "\n";
	//std::cout << setArray.set(0).lower() << "\n";
	//std::cout << setArray.set(0).upper() << "\n";

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

GABoolean MyGenome::IsEvaluated() const
{
	return _evaluated;
}

GARealAlleleSetArray* MyGenome::GetAlleleSetArray() const
{
	return mAlleleSetArray;
}

GA1DArrayAlleleGenome<float>* MyGenome::Get1DArrayAlleleGenome() const
{
	return m1DArrayAlleleGenome;
}
