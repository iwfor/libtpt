/*
 * parse_impl.cxx
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


void Parser::Impl::parse_include(std::ostream* os)
{
	ParamList pl;
	if (getparamlist(pl))
		return;

	if (pl.size() != 1)
	{
		recorderror("Include takes exactly 1 parameter");
		return;
	}
	// Create another Impl which inherits symbols table
	// to process include
	Buffer buf(pl[0].c_str());
	if (!buf)
	{
		recorderror("File Error: Could not read " + pl[0]);
		return;
	}
	Impl incl(buf, symbols, macros);
	if (incl.pass1(os))
	{
		// copy incl's error list, if any
		ErrorList::iterator it(incl.errlist.begin()), end(incl.errlist.end());
		for (; it != end; ++it)
			errlist.push_back(*it);
	}
}


} // end namespace TPTLib
