/*
 * parse_impl_set.cxx
 *
 * Parser Implementation
 *
 * $Id$
 *
 */

#include "parse_impl.h"

#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPTLib {


void Parser::Impl::parse_set()
{
	Token<> tok;
	// Set has to be manually parsed since the first parameter is the
	// identifier to be set.
	tok = lex.getstricttoken();
	if (tok.type != token_openparen)
	{
		recorderror("Syntax error, expected open parenthesis", &tok);
		return;
	}

	// tok holds the current token and nexttok holds the token returned
	// by the parser.
	tok = lex.getstricttoken();
	if (tok.type != token_id)
	{
		recorderror("Syntax error, first parameter must be ID", &tok);
		return;
	}
	std::string id(tok.value);

	tok = lex.getstricttoken();
	// If this is a close parenthesis, then just clear the symbol
	if (tok.type == token_closeparen)
	{
		symbols.set(id, "");
		return;
	}
	if (tok.type != token_comma)
	{
		recorderror("Syntax error, expected comma (,)", &tok);
		return;
	}
	tok = lex.getstricttoken();
	// If this is a close parenthesis, then just clear the symbol
	if (tok.type == token_closeparen)
	{
		symbols.set(id, "");
		return;
	}
	// Parse the expression
	Token<> nexttok = parse_level0(tok);
	symbols.set(id, tok.value);
	tok = nexttok;
	if (tok.type != token_closeparen)
		recorderror("Syntax error, expected close parenthesis", &tok);
}


/*
 * Only set the variable if it is empty.
 *
 */
void Parser::Impl::parse_setif()
{
	Token<> tok;
	// Set has to be manually parsed since the first parameter is the
	// identifier to be set.
	tok = lex.getstricttoken();
	if (tok.type != token_openparen)
	{
		recorderror("Syntax error, expected open parenthesis", &tok);
		return;
	}

	// tok holds the current token and nexttok holds the token returned
	// by the parser.
	tok = lex.getstricttoken();
	if (tok.type != token_id)
	{
		recorderror("Syntax error, first parameter must be ID", &tok);
		return;
	}
	std::string id(tok.value);

	tok = lex.getstricttoken();
	// If this is a close parenthesis, then just clear the symbol
	if (tok.type == token_closeparen)
	{
		symbols.set(id, "");
		return;
	}
	if (tok.type != token_comma)
	{
		recorderror("Syntax error, expected comma (,)", &tok);
		return;
	}
	tok = lex.getstricttoken();
	// If this is a close parenthesis, then just clear the symbol
	if (tok.type == token_closeparen)
	{
		symbols.set(id, "");
		return;
	}
	// Parse the expression
	Token<> nexttok = parse_level0(tok);
	if (!symbols.empty(id))
		symbols.set(id, tok.value);
	tok = nexttok;
	if (tok.type != token_closeparen)
		recorderror("Syntax error, expected close parenthesis", &tok);
}

} // end namespace TPTLib
