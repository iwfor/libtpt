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

namespace TPTLib {


/*
 * Reinventing the wheel?  Not all libraries support these functions
 *
 */
int64_t Parser::Impl::str2num(const char* str)
{
	int64_t value = 0;
	bool isneg(false);

	if (*str == '-')
	{
		++str;
		isneg = true;
	}
	while (*str)
	{
		value*= 10;
		value+= (*str) - '0';
		++str;
	}
	if (isneg)
		value = -value;
	return value;
}


void Parser::Impl::num2str(int64_t value, std::string& str)
{
	char buf[32];

	str.erase();
	if (!value)
		str = "0";
	else
	{
		int i=0;
		if (value < 0)
		{
			str = '-';
			value = -value;
		}
		while (value)
		{
			buf[i++] = static_cast<char>(value % 10) + '0';
			value/= 10;
		}
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

	left.type = token_string;
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
		Token<> right = lex.getstricttoken(), nextop;
		// we have left and operation, but no right
		nextop = parse_level2(right);
		int64_t lwork = str2num(left.value), rwork = str2num(right.value);
		if (op.value == "&&")
			lwork = lwork && rwork;
		else if (op.value == "||")
			lwork = lwork || rwork;
		else if (op.value == "^^")
			lwork = !lwork ^ !rwork;
		num2str(lwork, left.value);
		// read the next operator/terminator
		op = nextop;
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
		left.type = token_string;
	}
	else
		right = parse_level6(left);
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
		// create copy of left.value so key is not overwritten
		symbols.get(std::string(left.value), left.value);
		left.type = token_string;
		break;
	case token_integer:
		left.type = token_string;
		break;
	case token_empty:
		left = parse_empty();
		break;
	case token_rand:
		left = parse_rand();
		break;
	case token_usermacro:
		// TEST
		{
			std::stringstream tempstr;
			user_macro(left.value, &tempstr);
			left.value = tempstr.str();
			left.type = token_string;
		}
		break;
	case token_eval:	// alias for concat
	case token_concat:
		left = parse_concat();
		break;
	case token_length:
		left = parse_length();
		break;
	case token_uc:
		left = parse_uc();
		break;
	case token_lc:
		left = parse_lc();
		break;
	case token_size:
		left = parse_size();
		break;
	case token_substr:
		left = parse_substr();
		break;
	case token_string:
		// string is okay
		break;
	default:
		// This may be a unary operator or parenthesis
		break;
	}
	// Return next available token
	return lex.getstricttoken();
}

} // end namespace TPTLib
