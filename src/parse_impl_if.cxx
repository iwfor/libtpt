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
void Parser::Impl::parse_if(std::ostream* os)
{
	ParamList pl;

	if (getparamlist(pl))
		return;

	if (pl.empty())
	{
		recorderror("Syntax error, @if requires an expression");
		return;
	}

	int64_t lwork;
	lwork = str2num(pl[0]);
	if (lwork)	// and non-zero value is true
	{
		parse_block(os);
//		tok = lex.getloosetoken();
//		if (tok.type == token_else)
//		{
//			std::string ignore;
//			std::stringstream ignorestr(ignore);
//		}
//		else
//		{
//			// unget token
//		}
	}
	else
	{
		std::string ignore;
		std::stringstream ignorestr(ignore);
		parse_block(&ignorestr);
	}
}

} // end namespace TPTLib
