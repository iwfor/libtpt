/*
 * symbols.cxx
 *
 * $Id$
 *
 */

#include <tptlib/symbols.h>
#include <tptlib/parse.h>

namespace TPTLib {

/**
 *
 * Get the specified symbol from the specified symbols table, recursing
 * to process embedded symbols as needed.
 *
 * @param	id		Symbol to retrieve.
 * @param	symtab	Symbol Table to search
 * @return	Value of requested symbol;
 * @return	Empty string when symbol not found.
 * @author	Isaac W. Foraker
 *
 */
std::string get_symbol(const std::string& id, const SymbolTable& symtab)
{
	// If this is an enclosed id, strip ${};
	if (id[0] == '$')
		return get_symbol(id.substr(2, id.size()-3), symtab);

	// Return symbol if no embedded id.
	if (id.find('$') == std::string::npos)
	{
		SymbolTable::const_iterator it(symtab.find(id));
		if (it == symtab.end())
			return "";
		return (*it).second;
	}

	// When an id contains an embedded ${id}, recurse and build new id.
	Buffer buf(id.c_str(), id.size());
	Parser p(buf, &symtab);
	std::string newid(p.run());
	return get_symbol(newid, symtab);
}

} // end namespace TPTLib
