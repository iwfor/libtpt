/*
 * parse_impl_if.cxx
 *
 * Parse If Implementation
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
 *
 */
bool Parser::Impl::parse_ifexpr(std::ostream* os)
{
	ParamList pl;

	if (getparamlist(pl))
		return false;

	if (pl.empty())
	{
		recorderror("Syntax error, @if requires an expression");
		return false;
	}
	else if (pl.size() > 1)
		recorderror("Warning: extra parameters in @if ignored");

	int64_t lwork;
	Token<> tok;
	lwork = str2num(pl[0]);	// lwork = result of if expression

	if (lwork)	// if true, then TPT if was true
		parse_block(os);
	else
	{
		std::string ignore;
		std::stringstream ignorestr(ignore);
		parse_block(&ignorestr);
	}

	return !!lwork;
}

void Parser::Impl::parse_if(std::ostream* os)
{
	Token<> tok;

	std::string ignore;
	std::stringstream ignorestr(ignore);

	bool done;
	done = parse_ifexpr(os);
	tok = lex.getloosetoken();

	while (tok.type == token_elsif)
	{
		if (done)
			parse_ifexpr(&ignorestr);
		else
			done = parse_ifexpr(os);
		tok = lex.getloosetoken();
	}

	if (tok.type == token_else)
	{
		if (done)
			parse_block(&ignorestr);
		else
			parse_block(os);
	}
	else
	{
		// unget token
		lex.unget(tok);
	}
}

} // end namespace TPTLib
