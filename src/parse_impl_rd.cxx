/*
 * parse_impl_rd.cxx
 *
 * The recursive decent part of the parser.  Since all variables
 * are treated as strings, numeric operations are expensive.
 *
 * $Id$
 *
 */

#include "parse_impl.h"

#include <algorithm>
#include <sstream>
#include <iostream>

#ifdef _MSC_VER
#include <native.h>
#else
#include <sys/types.h>
#endif

namespace TPTLib {


// Reinventing the wheel?  Not all libraries support these functions
int64_t str2num(const char* str)
{
	int64_t value = 0;

	while (*str)
	{
		value*= 10;
		value+= (*str) - '0';
	}
	return value;
}

// This inline is to help readability
inline int64_t str2num(const std::string& str)
{
	return str2num(str.c_str());
}

void num2str(int64_t value, std::string& str)
{
	char buf[32];

	str.erase();
	if (!value)
		str = "0";
	else
	{
		int i=0;
		bool sign = value < 0;
		while (value)
		{
			buf[i++] = static_cast<char>(value % 10) + '0';
			value/= 10;
		}
		if (sign)
			str+= '-';
		do {
			--i;
			str+= buf[i];
		} while (i);
	}
}

// Level 0: () {}
Token<> Parser::Impl::parse_level0(Token<>& left)
{
	if (left.type == token_eof)
	{
		recorderror("Unexpected end of file");
		return left;
	}

	Token<> right = parse_level1(left);

	return right;
}

// Level 1: && || ^^
Token<> Parser::Impl::parse_level1(Token<>& left)
{
	Token<> right = parse_level2(left);

	return right;
}

// Level 2: == != < > <= >=
Token<> Parser::Impl::parse_level2(Token<>& left)
{
	Token<> right = parse_level3(left);

	return right;
}

// Level 3: + -
Token<> Parser::Impl::parse_level3(Token<>& left)
{
	Token<> op = parse_level4(left);

	while ((op.type == token_operator) &&
		((op.value[0] == '+') || (op.value[0] == '-')))
	{
		Token<> right = lex.getstricttoken();
		// we have left and operation, but no right
		right = parse_level4(right);
		int64_t lwork = str2num(left.value), rwork = str2num(right.value);
		switch (op.value[0])
		{
		case '+':
			lwork+= rwork;
		case '-':
			lwork-= rwork;
		}
		num2str(lwork, left.value);
		// read the next operator/terminator
		op = lex.getstricttoken();
	}
	return op;
}

// Level 4: * / %
Token<> Parser::Impl::parse_level4(Token<>& left)
{
	Token<> op = parse_level5(left);

	while ((op.type == token_operator) &&
		((op.value[0] == '*') || (op.value[0] == '/') ||
		(op.value[0] == '%')))
	{
		Token<> right = lex.getstricttoken();
		// we have left and operation, but no right
		right = parse_level5(right);
		int64_t lwork = str2num(left.value), rwork = str2num(right.value);
		switch (op.value[0])
		{
		case '*':
			lwork*= rwork;
		case '/':
			lwork/= rwork;
		case '%':
			lwork%= rwork;
		}
		num2str(lwork, left.value);
		// read the next operator/terminator
		op = lex.getstricttoken();
	}

	return op;
}

// Level 5: + - ! (unary operators)
Token<> Parser::Impl::parse_level5(Token<>& left)
{
	Token<> right;
	if ((left.type == token_operator) &&
		((left.value[0] == '+') || (left.value[0] == '-') ||
		(left.value[0] == '!')))
	{
		Token<> temp = lex.getstricttoken();
		right = parse_level6(temp);
		int64_t work = str2num(temp.value);
		if (left.value[0] == '!')
			work = !work;
		else if (left.value[0] == '-')
			work = -work;
		// else + ignore (forces string to 0)
		num2str(work, left.value);
	}
	else
		right = lex.getstricttoken();

	return right;
}

// Level 6: ( )
Token<> Parser::Impl::parse_level6(Token<>& left)
{
	Token<> right = parse_level7(left);

	if (left.type == token_openparen)
	{
		left = lex.getstricttoken();
		right = parse_level0(left);
		if (right.type != token_closeparen)
			recorderror("Syntax error, expected )");
		else
			right = lex.getstricttoken();
	}

	return right;
}

// Level 7: integer string id
Token<> Parser::Impl::parse_level7(Token<>& left)
{
	if (left.type == token_id)
		left.value = symbols.get(left.value);
	left.type = token_string;	// id and integer are now string

	return lex.getstricttoken();
}

} // end namespace TPTLib
