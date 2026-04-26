
#include "TNG.h"
#include "..\..\GeneticLibrary\source\GlobalRandomGenerator.h"
#include "..\..\GeneticLibrary\source\Algorithm.h"
#include "..\..\GeneticLibrary\source\FPEqual.h"

namespace TNG
{

	TngNode::~TngNode()
	{
		if( _left )
			delete _left;

		if( _right )
			delete _right;
	}

	TngNode* TngNode::Copy() const
	{
		TngNode* root = new TngNode( *this );

		if( _left )
		{
			root->_left = _left->Copy();
			root->_left->_parent = root;
		}

		if( _right )
		{
			root->_right = _right->Copy();
			root->_right->_parent = root;
		}

		return root;
	}

	void TngNode::Detach()
	{
		if( _parent )
		{
			if( _parent->_left == this )
				_parent->_left = NULL;
			else
				_parent->_right = NULL;

			_parent = NULL;
		}
	}

	bool TngNode::IsAncestor(const TngNode* node) const
	{
		if( node != this )
		{
			for( const TngNode* current = _parent; current; current = current->_parent )
			{
				if( current == node )
					return true;
			}
		}

		return false;
	}

	int GACALL TngAdd(int a, int b) { return a + b; }

	int GACALL TngSub(int a, int b) { return a - b; }

	int GACALL TngMul(int a, int b) { return a * b; }

	int GACALL TngDiv(int a, int b) { return !b || a % b ? a : a / b; }

	typedef int (GACALL *TngOp)(int, int);

	TngOp TngOps[] = { TngAdd, TngSub, TngMul, TngDiv };

	inline int GACALL TngOpExec(TngNodeType nodeType,
		int a,
		int b) { return TngOps[ nodeType - 1 ]( a, b ); }

	char TngOpsChar[] = { '+', '-', '*', '/' };

	void TngToStringHelper(std::string& str,
		const TngNode* node,
		const int* values)
	{
		TngNodeType type = node->_type;
		const TngNode* parent =  node->_parent;

		if( type == TNT_NUMBER )
		{
			char numStr[ 10 ];
			itoa( values[ node->_value ], numStr, 10 );
			str += numStr;
			return;
		}

		str += "( ";
		TngToStringHelper( str, node->_left, values );
		str += TngOpsChar[ type - 1 ];
		TngToStringHelper( str, node->_right, values );
		str += " )";
	}

	std::string GACALL TngToString(const TngChromosome& chromosome)
	{
		std::string str;
		const TngNode* root = chromosome.GetRoot();
		if( root )
			TngToStringHelper( str, root, ( (const TngConfigBlock&)chromosome.GetConfigBlock() ).GetNumbers() );

		return str;
	}

	class TngNodeValueBuffer
	{

	public:

		struct TngNodeValue
		{
			int _value;
			TngNode* _node;
		};

	private:

		TngNodeValue _buffer[ 2 * TNG_NUMBER_COUNT - 1 ];

		int _count;

	public:

		TngNodeValueBuffer() : _count(0) { }

		inline void GACALL Add(TngNode* node,
			int value,
			bool sort)
		{
			int index = _count++ - 1;

			if( sort )
			{
				for( ; index >= 0 && _buffer[ index ]._value > value; index-- )
					_buffer[ index + 1 ] = _buffer[ index ];
			}

			TngNodeValue& entry = _buffer[ index + 1 ];
			entry._value = value;
			entry._node = node;
		}

		inline void GACALL Add(const TngNodeValueBuffer& buffer)
		{
			for( int i = buffer._count - 1; i >= 0; i-- )
				_buffer[ _count++ ] = buffer._buffer[ i ];
		}

		inline TngNodeValue& GACALL GetLast() { return _buffer[ _count - 1 ]; }

		inline int GACALL GetCount() const { return _count; }

		inline TngNodeValue& GACALL operator [](int index) { return _buffer[ index ]; }

	};

	TngNode* GACALL TngReduceTree(TngNode* node,
		TngNodeValueBuffer& parentBuffer,
		const int* numbers)
	{
		if( node->_type == TNT_NUMBER )
		{
			parentBuffer.Add( node, numbers[ node->_value ], false );
			return NULL;
		}

		TngNodeValueBuffer buffer;

		TngNode* replacement = NULL;

		replacement = TngReduceTree( node->_left, buffer, numbers );
		if( replacement )
		{
			TngNode* remove = node->_left;

			replacement->Detach();
			node->_left = replacement;
			replacement->_parent = node;
			delete remove;
		}

		int left = buffer.GetLast()._value;

		replacement = TngReduceTree( node->_right, buffer, numbers );
		if( replacement )
		{
			TngNode* remove = node->_right;

			replacement->Detach();
			node->_right = replacement;
			replacement->_parent = node;
			delete remove;
		}

		int right = buffer.GetLast()._value;

		int result = TngOpExec( node->_type, left, right );

		for( int i = buffer.GetCount() - 1; i >= 0; i-- )
		{
			if( buffer[ i ]._value == result )
			{
				TngReduceTree( buffer[ i ]._node, parentBuffer, numbers );
				return buffer[ i ]._node;
			}
		}

		parentBuffer.Add( buffer );
		parentBuffer.Add( node, result, false );

		return NULL;
	}

	void GACALL TngReduceTree(TngChromosome& chromosome,
		const int* numbers)
	{
		TngNodeValueBuffer buffer;
		TngNode* newRoot = TngReduceTree( chromosome.GetRoot(), buffer, numbers );
		if( newRoot )
		{
			newRoot->Detach();
			chromosome.SetRoot( newRoot );
		}
	}

	void GACALL TngNormalizeTree(TngNode* node,
		TngNodeValueBuffer& parentBuffer,
		int previousOp,
		const int* numbers)
	{
		TngNodeValueBuffer buffer;
		bool sorted = previousOp == TNT_TIMES || previousOp == TNT_PLUS;

		switch( node->_type )
		{

		case TNT_NUMBER:
			parentBuffer.Add( new TngNode( *node ), numbers[ node->_value ], sorted );
			break;

		case TNT_TIMES:
		case TNT_PLUS:

			if( previousOp == node->_type )
			{
				TngNormalizeTree( node->_left, parentBuffer, node->_type, numbers );
				TngNormalizeTree( node->_right, parentBuffer, node->_type, numbers );
			}
			else
			{
				TngNormalizeTree( node->_left, buffer, node->_type, numbers );
				TngNormalizeTree( node->_right, buffer, node->_type, numbers );

				int value;
				TngNode *parent = NULL, *root = NULL;
				for( int i = buffer.GetCount() - 1; i > 0; i-- )
				{
					TngNode* newNode = new TngNode( *node );

					if( parent )
					{
						parent->_left = newNode;
						newNode->_parent = parent;
						value = TngOpExec( node->_type, value, buffer[ i ]._value );
					}
					else
					{
						root = newNode;
						value = buffer[ i ]._value;
					}

					newNode->_right = buffer[ i ]._node;
					buffer[ i ]._node->_parent = newNode;

					parent = newNode;
				}

				parent->_left = buffer[ 0 ]._node;
				buffer[ 0 ]._node->_parent = parent;
				parentBuffer.Add( root, TngOpExec( node->_type, value, buffer[ 0 ]._value ), sorted );
			}

			break;

		case TNT_MINUS:
		case TNT_OVER:

			TngNormalizeTree( node->_left, buffer, node->_type, numbers );
			TngNormalizeTree( node->_right, buffer, node->_type, numbers );

			TngNode* newNode = new TngNode( *node );

			newNode->_left = buffer[ 0 ]._node;
			newNode->_right = buffer[ 1 ]._node;

			newNode->_left->_parent = newNode;
			newNode->_right->_parent = newNode;

			parentBuffer.Add( newNode, TngOpExec( node->_type, buffer[ 0 ]._value, buffer[ 1 ]._value ), sorted );

			break;
		}
	}

	void GACALL TngNormalizeTree(TngChromosome& chromosome,
		const int* numbers)
	{
		TngNodeValueBuffer buffer;
		TngNormalizeTree( chromosome.GetRoot(), buffer, -1, numbers );

		chromosome.SetRoot( buffer[ 0 ]._node );
	}

	Chromosome::GaChromosomePtr TngChromosome::MakeNewFromPrototype() const
	{
		int count = GaGlobalRandomIntegerGenerator->Generate( 1, TNG_NUMBER_COUNT );

		TngNode* freeNodes[ TNG_NUMBER_COUNT ];
		for( int i = count - 1; i >= 0; i-- )
			freeNodes[ i ] = new TngNode( TNT_NUMBER, i );

		while( count > 1 )
		{
			int merge[ 2 ];

			merge[ 0 ] = GaGlobalRandomIntegerGenerator->Generate( 0, count - 1 );
			merge[ 1 ] = GaGlobalRandomIntegerGenerator->Generate( 0, count - 1 );

			if( merge[ 1 ] > merge[ 0 ] )
			{
				int t = merge[ 0 ];
				merge[ 0 ] = merge[ 1 ];
				merge[ 1 ] = t;
			}
			else if( merge[ 1 ] == merge[ 0 ] )
			{
				if( merge[ 1 ] < count - 1 )
					merge[ 1 ]++;
				else
					merge[ 0 ]--;
			}

			TngNode* merged = new TngNode( (TngNodeType)GaGlobalRandomIntegerGenerator->Generate( TNT_PLUS, TNT_OVER ) );
			merged->_left = freeNodes[ merge[ 0 ] ];
			merged->_right = freeNodes[ merge[ 1 ] ];

			merged->_left->_parent = merged;
			merged->_right->_parent = merged;

			freeNodes[ merge[ 0 ] ] = merged;
			for( int i = merge[ 1 ]; i < count - 1; i++ )
				freeNodes[ i ] = freeNodes[ i + 1 ];

			count--;
		}

		TngChromosome* newChromosome = new TngChromosome( (TngConfigBlock*)_configBlock );
		newChromosome->SetRoot( freeNodes[ 0 ] );

		const int* numbers = ( (TngConfigBlock*)_configBlock )->GetNumbers();
		TngReduceTree( *newChromosome, numbers );
		TngNormalizeTree( *newChromosome, numbers );

		return newChromosome;
	}

	bool CompareTngNodes(const TngNode* n1,
		const TngNode* n2,
		const int* numbers)
	{
		if( n1->_type != n2->_type )
			return false;

		if( n1->_type == TNT_NUMBER )
			return numbers[ n1->_value ] == numbers[ n2->_value ];

		return CompareTngNodes( n1->_left, n2->_left, numbers ) && CompareTngNodes( n1->_right, n2->_right, numbers );
	}

	bool TngChromosome::operator ==(const Chromosome::GaChromosome& c) const
	{
		return CompareTngNodes( _root, ( (const TngChromosome&)c )._root, ( (const TngConfigBlock*)_configBlock )->GetNumbers() );
	}

	void GACALL TngIdentifyFreeValues(std::vector<bool>& freeValues,
		TngNode* node,
		TngNode* removed)
	{
		if( node == removed )
			return;

		if( node->_type != TNT_NUMBER )
		{
			TngIdentifyFreeValues( freeValues, node->_left, removed );
			TngIdentifyFreeValues( freeValues, node->_right, removed );
		}
		else
			freeValues[ node->_value ] = false;
	}

	void GACALL TngIdentifyDuplicateValues(std::vector<bool>& freeValues,
		std::vector<TngNode*>& duplicateValue,
		TngNode* node,
		int* count)
	{
		if( node->_type != TNT_NUMBER )
		{
			TngIdentifyDuplicateValues( freeValues, duplicateValue, node->_left, count );
			TngIdentifyDuplicateValues( freeValues, duplicateValue, node->_right, count );
		}
		else
		{
			if( !freeValues[ node->_value ] )
				duplicateValue[ ( *count )++ ] = node;

			freeValues[ node->_value ] = false;
		}
	}

	void GACALL TngAdoptDuplicateValues(std::vector<bool>& freeValues,
		std::vector<TngNode*>& duplicateValue,
		int count)
	{
		for( int j = count - 1; j >= 0; j-- )
		{
			int original = duplicateValue[ j ]->_value;
			for( int i = TNG_NUMBER_COUNT - 1; i >= 0; i-- )
			{
				if( freeValues[ i ] )
				{
					duplicateValue[ j ]->_value = i;
					freeValues[ i ] = false;

					break;
				}
			}
		}
	}

	void GACALL TngReplaceNode(TngChromosome& chromosome,
		TngNode* oldNode,
		TngNode* newNode)
	{
		std::vector<bool> freeValue( TNG_NUMBER_COUNT, true );
		TngIdentifyFreeValues( freeValue, chromosome.GetRoot(), oldNode );

		int count = 0;
		std::vector<TngNode*> duplicateValue( TNG_NUMBER_COUNT );
		TngIdentifyDuplicateValues( freeValue, duplicateValue, newNode, &count );
		TngAdoptDuplicateValues( freeValue, duplicateValue, count );

		TngNode* parent = oldNode->_parent;

		if( parent )
		{
			if( parent->_left == oldNode )
				parent->_left = newNode;
			else
				parent->_right = newNode;

			newNode->_parent = parent;
			delete oldNode;
		}
		else
			chromosome.SetRoot( newNode );
	}

	TngNode* GACALL TngGetRandomNode(TngNode* node,
		int* index)
	{
		if( ( *index )-- == 0 )
			return node;

		TngNode* ret = NULL;
		if( node->_left )
		{
			ret = TngGetRandomNode( node->_left, index );

			if( !ret )
				ret = TngGetRandomNode( node->_right, index );
		}

		return ret;
	}

	TngNode* GACALL TngGetRandomNode(TngNode* root,
		bool skipRoot)
	{
		int index = GaGlobalRandomIntegerGenerator->Generate( skipRoot ? 1 : 0, root->GetCount() - 1 );
		return TngGetRandomNode( root, &index );
	}

	void GACALL TngSelectNodes(TngChromosome& chromosome1,
		TngChromosome& chromosome2,
		TngNode** oldNode,
		TngNode** newNode)
	{
		TngNode *node1 = NULL, *node2 = NULL;
		int freeCount1 = TNG_NUMBER_COUNT - ( chromosome1.GetCodeSize() + 1 ) / 2, freeCount2 = TNG_NUMBER_COUNT - ( chromosome1.GetCodeSize() + 1 ) / 2;
		int count1 = 0, count2 = 0;

		do
		{
			node1 = TngGetRandomNode( chromosome1.GetRoot(), false );
			node2 = TngGetRandomNode( chromosome2.GetRoot(), false );
		} while( freeCount1 + ( node1->GetCount() + 1 ) / 2 < ( node2->GetCount() + 1 ) / 2 );

		*oldNode = node1;
		*newNode = node2->Copy();
	}

	Chromosome::GaChromosomePtr TngCrossover::operator()(const Chromosome::GaChromosome* parent1,
		const Chromosome::GaChromosome* parent2) const
	{
		TngChromosome* offspring = new TngChromosome( (const TngChromosome&)*parent1, false );

		const int* numbers = ( (TngConfigBlock&)( (TngChromosome&)*offspring ).GetConfigBlock() ).GetNumbers();

		TngNode *oldNode, *newNode;

		TngSelectNodes( *offspring, (TngChromosome&)*parent2, &oldNode, &newNode );
		TngReplaceNode( *offspring, oldNode, newNode );
		TngReduceTree( *offspring, numbers );
		TngNormalizeTree( *offspring, numbers );

		return offspring;
	}

	void GACALL TngMutation::operator ()(Chromosome::GaChromosome* chromosome) const
	{
		const int* numbers = ( (TngConfigBlock&)( (TngChromosome&)*chromosome ).GetConfigBlock() ).GetNumbers();
		TngChromosome* tng = (TngChromosome*)chromosome;

		if( GaGlobalRandomBoolGenerator->Generate( 0.5 ) || tng->GetCodeSize() < 2 )
		{
			TngNode* node = TngGetRandomNode( tng->GetRoot(), false );
			if( node->_type == TNT_NUMBER )
			{
				std::vector<bool> freeValue( TNG_NUMBER_COUNT, true );
				TngIdentifyFreeValues( freeValue, tng->GetRoot(), node );

				for( int i = TNG_NUMBER_COUNT - 1; i >= 0; i-- )
				{
					if( freeValue[ i ] && node->_value != i )
					{
						node->_value = i;
						break;
					}
				}
			}
			else
				node->_type = (TngNodeType)GaGlobalRandomIntegerGenerator->Generate( TNT_PLUS, TNT_OVER );
		}
		else
		{
			TngNode* node1 = NULL;
			TngNode* node2 = NULL;

			do
			{
				node1 = TngGetRandomNode( tng->GetRoot(), true );
				node2 = TngGetRandomNode( tng->GetRoot(), true );
			} while( node1 == node2 || node1->IsConnected( node2 ) );

			TngNode* parent1 = node1->_parent;
			TngNode* parent2 = node2->_parent;

			if( parent1->_left == node1 )
				parent1->_left = node2;
			else
				parent1->_right = node2;

			if( parent2->_left == node2 )
				parent2->_left = node1;
			else
				parent2->_right = node1;

			node1->_parent = parent2;
			node2->_parent = parent1;
		}

		TngReduceTree( (TngChromosome&)*chromosome, numbers );
		TngNormalizeTree( (TngChromosome&)*chromosome, numbers );
	}

	int GACALL TngCalculateValue(const TngNode* node,
		const int* values)
	{
		if( node->_type == TNT_NUMBER )
			return values[ node->_value ];

		return TngOpExec( node->_type, TngCalculateValue( node->_left, values ), TngCalculateValue( node->_right, values ) );
	}

	float TngFitnessOperation::operator ()(const Chromosome::GaChromosome* chromosome) const
	{
		const TngConfigBlock& block = (const TngConfigBlock&)( (const TngChromosome*)chromosome )->GetConfigBlock();

		int value = abs( block.GetTargetNumber() - TngCalculateValue( ( (const TngChromosome*)chromosome )->GetRoot(), block.GetNumbers() ) );
		//value *= ( (const TngChromosome*)chromosome )->GetRoot()->GetCount();
		return 1.0f / ( 1 + (float)( value ) );
	}

	bool TngStopCriteria::Evaluate(const Algorithm::GaAlgorithm& algorithm,
			const Algorithm::GaStopCriteriaParams& parameters) const
	{
		const TngStopCriteriaParams& p = (const TngStopCriteriaParams&)parameters;

		unsigned int elapsed = ( p.GetCurrentTime() - p.GetStartTime() ) / CLOCKS_PER_SEC;
		return elapsed >= p.GetExecutionTime() || Common::FloatsEqual( algorithm.GetAlgorithmStatistics().GetValue( Common::GSV_BEST_FITNESS ).GetCurrent(), 1.0f );
	}

}
