/*
 * eval.cxx
 *
 * Evaluate a string with the Parser.
 *
 * $Id$
 *
 */

#include "eval.h"

#include <tptlib/symbols.h>
#include <tptlib/parse.h>

#include <iostream>

namespace TPTLib {

std::string eval(const std::string& expr, const Symbols* sym)
{
	Buffer buf(expr.c_str(), expr.size());
	if (sym)
	{
		Symbols copy(*sym);
		Parser p(buf, &copy);
		return p.run();
	}
	else
	{
		Parser p(buf);
		return p.run();
	}
}


} // end namespace TPTLib
