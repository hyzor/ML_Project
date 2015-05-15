#include "MyGenome.h"

#include <ga/GARealGenome.C>

MyGenome::MyGenome(int id, GARealAlleleSetArray& setArray, GAGenome::Evaluator f)
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
	return new MyGenome(*this);
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
	//mAlleleSetArray = new GARealAlleleSetArray(*origMyGenome->GetAlleleSetArray());
	//m1DArrayAlleleGenome = new GA1DArrayAlleleGenome<float>(*origMyGenome->Get1DArrayAlleleGenome());

	// Copy parts of Ship
}

MyGenome::~MyGenome()
{
}

void MyGenome::Init(GAGenome& genome)
{
	// TODO: Implement functionality

	MyGenome* myGenome = (MyGenome*)&genome;

	//myGenome->m1DArrayAlleleGenome = new GA1DArrayAlleleGenome<float>(*myGenome->mAlleleSetArray, nullptr);

	//GA1DArrayAlleleGenome<float>::UniformInitializer(genome);
	//UniformInitializer(genome);

	//child.resize(GAGenome::ANY_SIZE); // let chrom resize if it can
	/*
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
	*/
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

/*
GARealAlleleSetArray* MyGenome::GetAlleleSetArray() const
{
	return mAlleleSetArray;
}

GA1DArrayAlleleGenome<float>* MyGenome::Get1DArrayAlleleGenome() const
{
	return m1DArrayAlleleGenome;
}
*/