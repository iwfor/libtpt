/*
 * eval.h
 *
 * $Id$
 *
 */

#include <libtpt/symbols.h>

#include <string>

namespace TPT {

std::string eval(const std::string& expr, const Symbols* sym=0);


} // end namespace TPT
