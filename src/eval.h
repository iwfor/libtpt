/*
 * eval.h
 *
 * $Id$
 *
 */

#include <tptlib/symbols.h>

#include <string>

namespace TPTLib {

std::string eval(const std::string& expr, const Symbols* sym=0);


} // end namespace TPTLib
