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
 * Get the value specified by id from the symbols table, recursing to
 * process embedded symbols as needed.
 *
 * @param	id		ID of symbol to retrieve.
 * @return	Value of requested symbol;
 * @return	Empty string when symbol not found.
 * @author	Isaac W. Foraker
 *
 */
std::string SymbolMap::get(const std::string& id)
{
	// If this is an enclosed id, strip ${};
	if (id[0] == '$')
		return get(id.substr(2, id.size()-3));

	// Return symbol if no embedded id.
	if (id.find('$') == std::string::npos)
	{
		SymbolTable::const_iterator it(symmap.find(id));
		if (it == symmap.end())
			return "";
		return (*it).second;
	}

	// When an id contains an embedded ${id}, recurse and build new id.
	Buffer buf(id.c_str(), id.size());
	Parser p(buf, &symmap);
	std::string newid(p.run());
	return get(newid);
}


/**
 *
 * TODO: document function
 *
 * @param	id			ID of the symbol to be pushed.
 * @param	value		Value of the symbol to be pushed.
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void SymbolMap::set(const std::string& id, const std::string& value)
{
	if (id[0] == '$')
		symmap[id.substr(2, id.size()-3)] = value;
	else
		symmap[id] = value;
}


/**
 *
 * Check whether the specified id exists in the symbol table.
 *
 * @param	id			ID of symbol to retrieve.
 * @return	true if symbol exists;
 * @return	false if symbol does not exist.
 * @author	Isaac W. Foraker
 *
 */
bool SymbolMap::exists(const std::string& id)
{
	// If this is an enclosed id, strip ${};
	if (id[0] == '$')
		return symmap.find(id.substr(2, id.size()-3)) != symmap.end();
	return symmap.find(id) != symmap.end();
}

/**
 *
 * Copy a symbol table into the symbol map, replacing any existing
 * entries.
 *
 * @param	symtab		Table to be copied.
 * @return	Const reference to the local symbol table.
 * @author	Isaac W. Foraker
 *
 */
const SymbolTable& SymbolMap::operator=(const SymbolTable& symtab)
{
	return symmap = symtab;
}

/**
 *
 * Push a symbol (id and value) onto the symbol stack.
 *
 * @param	id			ID of the symbol to be pushed.
 * @param	value		Value of the symbol to be pushed.
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void SymbolStack::push(const std::string& id, const std::string& value)
{
	Symbol_t sym;
	sym.id = id;
	sym.value = value;
	symstack.push(sym);
}


/**
 *
 * TODO: document function
 *
 * @param	symmap		Symbol Map to receive symbols.
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void SymbolStack::popall(SymbolMap& symmap)
{
	while (!symstack.empty())
	{
		Symbol_t& sym = symstack.top();
		symstack.pop();
		symmap.set(sym.id, sym.value);
	}
}

} // end namespace TPTLib
