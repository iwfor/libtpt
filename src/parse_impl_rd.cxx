/*
 * parse_impl_rd.cxx
 *
 * The recursive descent part of the parser.  Since all variables
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
		str++;
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

/*
 * The recursive descent parser will attempt to parse an expression
 * until it reaches a token it does not recognize.  The final token
 * will usually be a comma ',', or close parenthesis ')'.
 *
 */

// Level 0: Get things rolling
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
	Token<> op = parse_level2(left);

	while ((op.type == token_operator) &&
		((op.value == "&&") ||
		(op.value == "||") ||
		(op.value == "^^")))
	{
		Token<> right = lex.getstricttoken();
		// we have left and operation, but no right
		right = parse_level2(right);
		int64_t lwork = str2num(left.value), rwork = str2num(right.value);
		if (op.value == "&&")
			lwork = lwork && rwork;
		else if (op.value == "||")
			lwork = lwork || rwork;
		else if (op.value == "^^")
			lwork = !lwork ^ !rwork;
		num2str(lwork, left.value);
		// read the next operator/terminator
		op = lex.getstricttoken();
	}

	return op;
}

// Level 2: == != < > <= >=
Token<> Parser::Impl::parse_level2(Token<>& left)
{
	Token<> op = parse_level3(left), nextop;

	while (op.type == token_relop)
	{
		Token<> right = lex.getstricttoken();
		// we have left and operation, but no right
		nextop = parse_level3(right);
		int64_t lwork = str2num(left.value), rwork = str2num(right.value);
		if (op.value == "==")
			lwork = lwork == rwork;
		else if (op.value == "!=")
			lwork = lwork != rwork;
		else if (op.value == "<")
			lwork = lwork < rwork;
		else if (op.value == ">")
			lwork = lwork > rwork;
		else if (op.value == "<=")
			lwork = lwork <= rwork;
		else if (op.value == ">=")
			lwork = lwork >= rwork;
		num2str(lwork, left.value);
		// read the next operator/terminator
		op = nextop;
	}

	return op;
}

// Level 3: + -
Token<> Parser::Impl::parse_level3(Token<>& left)
{
	Token<> op = parse_level4(left), nextop;

	while ((op.type == token_operator) &&
		((op.value[0] == '+') || (op.value[0] == '-')))
	{
		Token<> right = lex.getstricttoken();
		// we have left and operation, but no right
		nextop = parse_level4(right);
		int64_t lwork = str2num(left.value), rwork = str2num(right.value);
		switch (op.value[0])
		{
		case '+':
			lwork+= rwork;
			break;
		case '-':
			lwork-= rwork;
			break;
		}
		num2str(lwork, left.value);
		// read the next operator/terminator
		op = nextop;
	}
	return op;
}

// Level 4: * / %
Token<> Parser::Impl::parse_level4(Token<>& left)
{
	Token<> op = parse_level5(left), nextop;

	while ((op.type == token_operator) &&
		((op.value[0] == '*') || (op.value[0] == '/') ||
		(op.value[0] == '%')))
	{
		Token<> right = lex.getstricttoken();
		// we have left and operation, but no right
		nextop = parse_level5(right);
		int64_t lwork = str2num(left.value), rwork = str2num(right.value);
		switch (op.value[0])
		{
		case '*':
			lwork*= rwork;
			break;
		case '/':
			lwork/= rwork;
			break;
		case '%':
			lwork%= rwork;
			break;
		}
		num2str(lwork, left.value);
		// read the next operator/terminator
		op = nextop;
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
		right = parse_level6(left);
//		right = lex.getstricttoken();

	return right;
}

// Level 6: ( )
Token<> Parser::Impl::parse_level6(Token<>& left)
{
	Token<> right = parse_level7(left);

	if (left.type == token_openparen)
	{
		left = right;
		right = parse_level0(left);
		if (right.type != token_closeparen)
			recorderror("Syntax error, expected )");
		else
			// get token after close paren
			right = lex.getstricttoken();
	}

	return right;
}

// Level 7: literals $id @macro
Token<> Parser::Impl::parse_level7(Token<>& left)
{
	switch (left.type) {
	case token_id:
		left.value = symbols.get(left.value);
		left.type = token_string;
		break;
	case token_integer:
		left.type = token_string;
		break;
	case token_include:
	case token_macro:
	case token_set:
	case token_if:
	case token_else:
	case token_foreach:
	case token_while:
	case token_next:
	case token_last:
		{
			std::string err(left.value);
			err+= " not allowed in expression";
			recorderror(err);
		}
		break;
	case token_empty:
		// TODO
		break;
	case token_rand:
		// TODO
		break;
	case token_usermacro:
		// TODO
		break;
	case token_concat:
		// TODO
		break;
	case token_string:
		// string is okay
		break;
	default:
		break;
	}

	return lex.getstricttoken();
}

} // end namespace TPTLib
