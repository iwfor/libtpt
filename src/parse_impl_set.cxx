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
	std::string id;
	ParamList pl;

	if (getidparamlist(id, pl))
		return;

	if (pl.empty())
		symbols.set(id, "");
	else
	{
		if (pl.size() > 1)
		{
			// this is an array
		}
		else
			symbols.set(id, pl[0]);
	}
}


/*
 * Only set the variable if it is empty.
 *
 */
void Parser::Impl::parse_setif()
{
	std::string id;
	ParamList pl;

	if (getidparamlist(id, pl))
		return;

	if (pl.empty())
	{
		if (!symbols.empty(id))
			symbols.set(id, "");
	}
	else
	{
		if (pl.size() > 1)
		{
			// this is an array
			if (!symbols.empty(id))
				symbols.set(id, pl);
		}
		else
		{
			if (!symbols.empty(id))
				symbols.set(id, pl[0]);
		}
	}
}

} // end namespace TPTLib
