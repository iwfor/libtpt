/*
 * symbols.cxx
 *
 * $Id$
 *
 */

// Disable warnings for long debug symbols
#ifdef _MSC_VER
#pragma warning(disable:4786)
#endif

#include <tptlib/symbols.h>
#include <tptlib/parse.h>

#include "symbols_impl.h"
#include "vars.h"		// Default variables

#include <iostream>
#include <cstdio>

namespace TPTLib {

/**
 *
 * Construct an instance of the Symbols class.
 *
 */
Symbols::Symbols() :
	imp(new Impl)
{
	for (unsigned i=0; i<numbuiltins; ++i)
		imp->symmap[tptlib_builtins[i].id].push_back(tptlib_builtins[i].value);
}


Symbols::Symbols(const Symbols& s) :
	imp (new Impl(s.imp->symmap))
{
}

/*
 * Destruct an instance of the Symbols class.
 *
 */
Symbols::~Symbols()
{
	delete imp;
}


/*
 *
 *
 */
Symbols& Symbols::operator=(const Symbols& sym)
{
	imp->symmap = sym.imp->symmap;
	return *this;
}


/**
 *
 * Get the value specified by id from the symbols table, recursing to
 * process embedded symbols as needed.  If the specified id points to an
 * array symbol, only the first element of the array will be returned.
 *
 * @param	id		ID of symbol to retrieve.
 * @param	sym		Symbol array to receive value(s).
 * @return	false on success;
 * @return	true if symbol is invalid.
 * @author	Isaac W. Foraker
 *
 */
bool Symbols::get(const SymbolKeyType& id, SymbolValueType& outval) const
{
	SymbolKeyType realid;
	unsigned index;
	outval.erase();
	// getrealid should return true
	if (imp->getrealid(id, realid, index))
		return true;

	SymbolTable::const_iterator it(imp->symmap.find(realid));
	if (it != imp->symmap.end())
	{
		if (index < (*it).second.size())
			outval = (*it).second[index];
	}
	return false;
}

/**
 *
 * Get the array of values specified by id from the symbols table,
 * recursing to process embedded symbols as needed.
 *
 * @param	id		ID of symbol to retrieve.
 * @param	sym		Symbol array to receive value(s).
 * @return	false on success;
 * @return	true if symbol is invalid.
 * @author	Isaac W. Foraker
 *
 */
bool Symbols::get(const SymbolKeyType& id, SymbolArrayType& outval) const
{
	SymbolKeyType realid;
	unsigned index;
	// getrealid should return true and index must be zero
	if (imp->getrealid(id, realid, index) || (index > 0))
	{
		outval.resize(1);
		outval[0].erase();
		return false;
	}

	SymbolTable::const_iterator it(imp->symmap.find(realid));
	if (it == imp->symmap.end())
	{
		// otherwise just return a blank
		outval.clear();
		outval.push_back("");
		return false;
	}
	outval = (*it).second;
	return true;
}


/*
 * Set a symbol's value
 *
 * @param	id			ID of the symbol to be pushed.
 * @param	value		Value of the symbol to be pushed.
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void Symbols::set(const SymbolKeyType& id, const SymbolValueType& value)
{
	SymbolKeyType realid;
	unsigned index;
	if (imp->getrealid(id, realid, index))
		return;
	if (index >= maxarraysize)
		return;
	if (imp->symmap[realid].size() < index+1)
		imp->symmap[realid].resize(index+1);
//std::cout << "Setting " << realid << "[" << index << "] to " << value << std::endl;
	imp->symmap[realid][index] = value;
}


/*
 * Set a symbol's array values
 *
 * @param	id			ID of the symbol to be pushed.
 * @param	value		Array values of the symbol to be pushed.
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void Symbols::set(const SymbolKeyType& id, const SymbolArrayType& value)
{
	SymbolKeyType realid;
	unsigned index;
	imp->getrealid(id, realid, index);
	imp->symmap[realid] = value;
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
bool Symbols::exists(const SymbolKeyType& id) const
{
	SymbolKeyType realid;
	unsigned index;
	imp->getrealid(id, realid, index);
	return imp->symmap.find(realid) != imp->symmap.end();
}


/**
 *
 * Check if a symbol is empty.
 *
 * @param	id			ID of symbol to retrieve.
 * @return	true if symbol is empty;
 * @return	false if symbol is not empty.
 * @author	Isaac W. Foraker
 *
 */
bool Symbols::empty(const SymbolKeyType& id) const
{
	SymbolKeyType realid;
	unsigned index;
	imp->getrealid(id, realid, index);
	SymbolTable::iterator it(imp->symmap.find(realid));
	if (it == imp->symmap.end())
		return true;
	if (index >= (*it).second.size())
		return true;
	return (*it).second[index].empty();
}


/**
 *
 * Check if a symbol is a multielement array
 *
 */
bool Symbols::isarray(const SymbolKeyType& id) const
{
	SymbolKeyType realid;
	unsigned index;
	imp->getrealid(id, realid, index);

	SymbolTable::iterator it(imp->symmap.find(realid));
	if (it == imp->symmap.end())
		return true;
	if (index >= (*it).second.size())
		return true;
	return (*it).second.size() > 1;
}


/**
 *
 * Check size of a symbol array.
 *
 */
unsigned Symbols::size(const SymbolKeyType& id) const
{
	SymbolKeyType realid;
	unsigned index;
	imp->getrealid(id, realid, index);

	SymbolTable::iterator it(imp->symmap.find(realid));
	if (it == imp->symmap.end())
		return true;
	if (index >= (*it).second.size())
		return true;
	return (*it).second.size();
}


/*
 * Determine the real final ID of a given ID, reducing
 * ${} and [].
 *
 * TODO: Parse expression in []
 *
 */
bool Symbols::Impl::getrealid(const SymbolKeyType& id,
							  SymbolKeyType& realkey, unsigned& index)
{
	if (id[0] == '$')
		return getrealid(id.substr(2, id.size()-3), realkey, index);
/*
	else if (id.find('$') != SymbolKeyType::npos)
	{
		// When id contains embedded ${id}, recurse to build new id.
		// Note: This is really inefficient, and is only here for
		// compatibility.
		Buffer buf(id.c_str(), id.size());
		Parser p(buf, this);
		SymbolKeyType newid(p.run());
		return getrealid(newid, realkey, index);
	}
*/

	// Check for array ref
	size_t obracket(id.find('['));
	if (obracket != SymbolKeyType::npos)
	{
		// There is an array index here
		size_t cbracket(id.find(']'));
		if (cbracket == SymbolKeyType::npos)
		{
			// syntax error (lex should prevent this)
			return true;
		}
		index = atoi(id.substr(obracket, cbracket-obracket).c_str());
		realkey = id.substr(0, obracket);
	}
	else
	{
		index = 0;
		realkey = id;
	}

	return false;
}

/**
 *
 * For debugging purposes
 *
 */
void Symbols::dump()
{
	SymbolTable::const_iterator it(imp->symmap.begin());
	SymbolTable::const_iterator end(imp->symmap.end());

	for (; it != end; ++it)
	{
		if ((*it).second.empty())
			std::cout << (*it).first << " is empty" << std::endl;
		else
			std::cout << (*it).first << " = " << (*it).second[0] << std::endl;
	}
}


} // end namespace TPTLib
