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
	Symbols copy;
	if (sym)
		copy = *sym;
	Parser p(buf, &copy);
	std::string temp = p.run();
	return temp;
}


} // end namespace TPTLib
