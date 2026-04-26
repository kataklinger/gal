
#include "Parsing.h"

namespace Parsing
{
	
	int AddOp(int a, int b) { return a + b; }
	int SubOp(int a, int b) { return a - b; }
	int MulOp(int a, int b) { return a * b; }
	int DivOp(int a, int b) { return !b ? throw ArithmeticException() : a / b; }

	typedef int (*OpPfn)(int, int);

	OpPfn Ops[] = { AddOp, SubOp, MulOp, DivOp };

	inline int OpExec(int operation,
		int a,
		int b) { return Ops[ operation ]( a, b ); }

	LexSymbol Lexer::Get()
	{
		int c = GetChar();
		switch( c )
		{
		case '(': return LexSymbol( LEX_ST_PARENS_OPEN, GetPosition() );
		case ')': return LexSymbol( LEX_ST_PARENS_CLOSE, GetPosition() );
		case '+': return LexSymbol( LEX_ST_OPERATOR, LEX_OT_PLUS, GetPosition() );
		case '-': return LexSymbol( LEX_ST_OPERATOR, LEX_OT_MINUS, GetPosition() );
		case '*': return LexSymbol( LEX_ST_OPERATOR, LEX_OT_TIMES, GetPosition() );
		case '/': return LexSymbol( LEX_ST_OPERATOR, LEX_OT_OVER, GetPosition() );
		case -1: return LexSymbol( LEX_ST_END, GetPosition() );

		default:
			if( !IsDigit( c ) ) throw SyntaxException( GetPosition() );

			int pos = GetPosition(), num = ToNum( c );
			while( IsDigit( PeekChar() ) )
				num = num * 10 + ToNum( GetChar() );

			return LexSymbol( LEX_ST_NUMBER, num, pos );
		}
	}

	int Lexer::GetChar()
	{
		while( isspace( PeekChar() ) ) _position++;
		return _position >= _input.length() ? -1 : _input[ _position++ ];
	}

	int Parser::Parse(int count/* = 0*/,
			const int* allowedNumbers/* = NULL*/)
	{
		for( int i = count - 1; i >= 0; i-- )
			_allowedNumber[ allowedNumbers[ i ] ]++;

		while( 1 )
		{
			LexSymbol symbol = _lexer.Get();

			switch( symbol._type )
			{

			case LEX_ST_END: return Calculate( symbol._position );
			case LEX_ST_PARENS_OPEN: OpenParens( symbol._position ); break;
			case LEX_ST_PARENS_CLOSE: CloseParens( symbol._position ); break;
			case LEX_ST_NUMBER: StoreNumber( symbol._value, symbol._position ); break;
			case LEX_ST_OPERATOR: StoreOperator( symbol._value, symbol._position ); break;

			}
		}

		throw SyntaxException( -1 );
	}

	void Parser::StoreNumber(int value,
		int position)
	{
		if( !_stack.empty() && _stack.rbegin()->_operator < 0 ) throw SyntaxException( position );
		if( _allowedNumber.size() > 0 && --_allowedNumber[ value ] < 0 ) throw InputException( value );

		_stack.push_back( Entry( value ) );
	}

	void Parser::StoreOperator(int op,
		int position)
	{
		if( _stack.empty() ) throw SyntaxException( position );

		Entry& entry = *_stack.rbegin();
		if( entry._operator >= 0 ) throw SyntaxException( position );

		entry._operator = op;
		entry._priority = op == LEX_OT_TIMES || op == LEX_OT_OVER ? _currentPriority + 1 : _currentPriority;
	}

	void Parser::OpenParens(int position)
	{
		if( _stack.size() > 0 && _stack.rbegin()->_operator < 0 )  throw SyntaxException( position );
		_currentPriority += 2;
	}

	void Parser::CloseParens(int position)
	{
		if( _currentPriority < 2 || _stack.rbegin()->_operator >= 0 ) throw SyntaxException( position );
		_currentPriority -= 2;
	}

	int Parser::Calculate(int position)
	{
		if( _stack.size() == 0 ) return 0;
		if( _currentPriority > 0 || _stack.rbegin()->_operator >= 0 )  throw SyntaxException( position );

		while( _stack.size() > 1 )
		{
			int position = 0;
			for( size_t i = 0; i < _stack.size(); i++ )
			{
				if( _stack[ i ]._priority > _stack[ position ]._priority )
					position = i;
			}

			Entry& current = _stack[ position ];
			Entry& next= _stack[ position + 1 ];

			current._value = OpExec( current._operator, current._value, next._value );
			current._operator = next._operator;
			current._priority = next._priority;

			_stack.erase( _stack.begin() + position + 1 );
		}

		return _stack[ 0 ]._value;
	}

} // Parsing
