
#ifndef __PR_TNG_H__
#define __PR_TNG_H__

#include "..\..\GeneticLibrary\source\ChromosomeOperations.h"
#include "..\..\GeneticLibrary\source\AlgorithmOperations.h"
#include <atlstr.h>
#include <time.h>

namespace TNG
{

	const int TNG_NUMBER_COUNT = 6;

	enum TngNodeType
	{
		TNT_NUMBER,
		TNT_PLUS,
		TNT_MINUS,
		TNT_TIMES,
		TNT_OVER
	};

	struct TngNode
	{

		TngNodeType _type;

		int _value;

		TngNode* _left;

		TngNode* _right;

		TngNode* _parent;

		TngNode() : _type(TNT_NUMBER),
			_value(-1),
			_left(NULL),
			_right(NULL),
			_parent(NULL) { }

		TngNode(TngNodeType type) : _type(type),
			_value(-1),
			_left(NULL),
			_right(NULL),
			_parent(NULL) { }

		TngNode(TngNodeType type,
			int value) : _type(type),
			_value(value),
			_left(NULL),
			_right(NULL),
			_parent(NULL) { }

		TngNode(const TngNode& rhs) : _type(rhs._type),
			_value(rhs._value),
			_left(NULL),
			_right(NULL),
			_parent(NULL) { }

		~TngNode();

		TngNode* Copy() const;

		void Detach();

		bool IsAncestor(const TngNode* node) const;

		bool IsConnected(const TngNode* node) const { return IsAncestor( node ) || node->IsAncestor( this ); }

		int GACALL GetCount() const { return 1 + ( _type != TNT_NUMBER ? _left->GetCount() + _right->GetCount() : 0 ); }

		inline bool GACALL operator ==(const TngNode& rhs) const{ return _type == rhs._type && _value == rhs._value; }

		inline bool GACALL operator !=(const TngNode& rhs) const { return !operator ==( rhs ); }

	};

	class TngConfigBlock : public Chromosome::GaChromosomeOperationsBlock
	{

	private:

		int _numbers[ TNG_NUMBER_COUNT ];

		int _targetNumber;

	public:

		TngConfigBlock(Chromosome::GaCrossoverOperation* crossoverOperation,
			Chromosome::GaMutationOperation* mutationOperation,
			Chromosome::GaFitnessOperation* fitnessOperation,
			Chromosome::GaFitnessComparator* fitnessComparator,
			Chromosome::GaChromosomeParams* parameters) : GaChromosomeOperationsBlock(crossoverOperation, mutationOperation, fitnessOperation, fitnessComparator, parameters) { }

		TngConfigBlock(const int* numbers,
			int targetNumber,
			Chromosome::GaCrossoverOperation* crossoverOperation,
			Chromosome::GaMutationOperation* mutationOperation,
			Chromosome::GaFitnessOperation* fitnessOperation,
			Chromosome::GaFitnessComparator* fitnessComparator,
			Chromosome::GaChromosomeParams* parameters) : GaChromosomeOperationsBlock(crossoverOperation, mutationOperation, fitnessOperation, fitnessComparator, parameters),
			_targetNumber(targetNumber) { SetNumbers( numbers ); }

		TngConfigBlock(const TngConfigBlock& rhs) : GaChromosomeOperationsBlock(rhs),
			_targetNumber(rhs._targetNumber) { SetNumbers( rhs._numbers ); }

		inline void GACALL SetNumbers(const int* numbers)
		{
			for( int i = TNG_NUMBER_COUNT - 1; i >= 0; i-- )
				_numbers[ i ] = numbers[ i ];
		}

		inline int* GetNumbers() { return _numbers; }

		inline const int* GetNumbers() const { return _numbers; }

		inline void GACALL SetTargetNumber(int number) { _targetNumber = number; }

		inline int GACALL GetTargetNumber() const { return _targetNumber; }

	};

	class TngChromosome : public Chromosome::GaDynamicOperationChromosome
	{

	private:

		TngNode* _root;

		TngNode* _backup;

	public:

		TngChromosome(TngConfigBlock* configBlock) : GaDynamicOperationChromosome(configBlock),
			_root(NULL),
			_backup(NULL) { }

		TngChromosome(const TngChromosome& c, bool setupOnly) : GaDynamicOperationChromosome(c, setupOnly),
			_root(NULL),
			_backup(NULL) 
		{
			if( !setupOnly && c._root )
				_root = c._root->Copy();
		}

		virtual ~TngChromosome()
		{
			if( _root )
				delete _root;
		}

		virtual Chromosome::GaChromosomePtr GACALL MakeCopy(bool setupOnly) const { return new TngChromosome( *this, setupOnly ); }

		virtual Chromosome::GaChromosomePtr GACALL MakeNewFromPrototype() const;

		virtual void GACALL PreapareForMutation() { _backup = _root->Copy(); }

		virtual void GACALL AcceptMutation()
		{
			if( _backup )
			{
				delete _backup;
				_backup = NULL;
			}
		}

		virtual void GACALL RejectMutation()
		{
			if( _root )
				delete _root;

			_root = _backup;
			_backup = NULL;
		}

		inline void GACALL SetRoot(TngNode* root)
		{
			if( _root )
				delete _root;

			_root = root;
		}

		virtual int GACALL GetCodeSize(void) const { return _root ? _root->GetCount() : 0; }

		inline TngNode* GACALL GetRoot() { return _root; }

		inline const TngNode* GACALL GetRoot() const { return _root; }

		virtual bool GACALL operator ==(const Chromosome::GaChromosome& c) const;

	};

	std::string GACALL TngToString(const TngChromosome& chromosome);

	class TngCrossover : public Chromosome::GaCrossoverOperation
	{

	public:

		virtual Chromosome::GaChromosomePtr GACALL operator()(const Chromosome::GaChromosome* parent1,
			const Chromosome::GaChromosome* parent2) const;

		virtual Common::GaParameters* GACALL MakeParameters() const { return NULL; }

		virtual bool GACALL CheckParameters(const Common::GaParameters& parameters) const { return true; }

	};

	class TngMutation : public Chromosome::GaMutationOperation
	{

	public:

		virtual void GACALL operator ()(Chromosome::GaChromosome* chromosome) const;

		virtual Common::GaParameters* GACALL MakeParameters() const { return NULL; }

		virtual bool GACALL CheckParameters(const Common::GaParameters& parameters) const { return true; }

	};

	int GACALL TngCalculateValue(const TngNode* node, const int* values);

	class TngFitnessOperation : public Chromosome::GaFitnessOperation
	{

	public:

		virtual float GACALL operator ()(const Chromosome::GaChromosome* chromosome) const;

		virtual Common::GaParameters* GACALL MakeParameters() const { return NULL; }

		virtual bool GACALL CheckParameters(const Common::GaParameters& parameters) const { return true; }

	};

	class TngStopCriteriaParams : public Algorithm::GaStopCriteriaParams
	{

	private:

		unsigned int _startTime;

		unsigned int _executionTime;

	public:

		TngStopCriteriaParams() : _startTime(0),
			_executionTime(0) { }

		TngStopCriteriaParams(unsigned int startTime,
			unsigned int executionTime) : _startTime(startTime),
			_executionTime(executionTime) { }

		TngStopCriteriaParams(unsigned int executionTime) : _executionTime(executionTime) { _startTime = GetCurrentTime(); }

		virtual GaParameters* GACALL Clone() const { return new TngStopCriteriaParams( *this ); }

		unsigned int GACALL GetCurrentTime() const { return clock(); }

		inline void GACALL SetStartTime(unsigned int time) { _startTime = time; }

		inline unsigned int GACALL GetStartTime() const { return _startTime; }

		inline void GACALL SetExecutionTime(unsigned int time) { _executionTime = time; }

		inline unsigned int GACALL GetExecutionTime() const { return _executionTime; }

	};

	class TngStopCriteria : public Algorithm::GaStopCriteria
	{

	public:

		virtual bool GACALL Evaluate(const Algorithm::GaAlgorithm& algorithm,
			const Algorithm::GaStopCriteriaParams& parameters) const;

		virtual GaParameters* GACALL MakeParameters() const { return new TngStopCriteriaParams(); }

		virtual bool GACALL CheckParameters(const GaParameters& parameters) const
		{
			return ( (const TngStopCriteriaParams&)parameters ).GetStartTime() > 0 && ( (const TngStopCriteriaParams&)parameters ).GetExecutionTime() > 0;
		}

	};

}

#endif // __PR_TNG_H__
