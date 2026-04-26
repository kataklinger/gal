
#ifndef __TNG_PARSING_H__
#define __TNG_PARSING_H__

#include <string>
#include <vector>
#include <hash_map>

namespace Parsing
{

	enum LexSymbolType
	{
		LEX_ST_PARENS_OPEN,
		LEX_ST_PARENS_CLOSE,
		LEX_ST_NUMBER,
		LEX_ST_OPERATOR,
		LEX_ST_END
	};

	enum LexOperatorType
	{
		LEX_OT_PLUS,
		LEX_OT_MINUS,
		LEX_OT_TIMES,
		LEX_OT_OVER
	};

	struct LexSymbol
	{
		LexSymbolType _type;
		int _value;
		int _position;

		LexSymbol(LexSymbolType type,
			int value,
			int position) : _type(type), _value(value), _position(position) { }

		LexSymbol(LexSymbolType type,
			int position) : _type(type), _value(-1), _position(position) { }

	};

	struct SyntaxException
	{
		int _charPosition;
		SyntaxException(int charPosition) : _charPosition(charPosition) { }
	};

	struct ArithmeticException { };

	struct InputException
	{
		int _value;
		InputException(int value) : _value(value) { }
	};

	class Lexer
	{

	private:

		std::string _input;
		std::string::size_type _position;

	public:

		Lexer(const std::string& input) : _input(input), _position(0) { }

		LexSymbol Get();

	private:

		int GetChar();

		inline int PeekChar() { return _position >= _input.length() ? -1 : _input[ _position ]; }

		inline int GetPosition() const { return _position - 1; }

		inline bool IsDigit(int c) { return c >= '0' && c <= '9'; }

		inline int ToNum(int c) { return  c - '0'; }

	};

	class Parser
	{

	private:

		typedef stdext::hash_map<int, int> AllowedNumbers;

		struct Entry
		{
			int _value;
			int _operator;
			int _priority;

			Entry(int value) : _value(value), _operator(-1), _priority(-1) { }
		};

		Lexer _lexer;
		AllowedNumbers _allowedNumber;
		std::vector<Entry> _stack;
		int _currentPriority;

	public:


		Parser(const std::string& input) : _lexer(input), _currentPriority(0) { }

		int Parse(int count = 0,
			const int* allowedNumbers = NULL);

	private:

		void StoreNumber(int value,
			int position);

		void StoreOperator(int op,
			int position);

		void OpenParens(int position);

		void CloseParens(int position);

		int Calculate(int position);

	};

} // Parsing

#endif // __TNG_PARSING_H__
