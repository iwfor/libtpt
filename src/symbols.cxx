/*
 * symbols.cxx
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac*nospam*@tazthecat.net)
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
#include "eval.h"

#include <iostream>
#include <cstdio>
#include <cstring>

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
	if (imp->getrealid(id, realid, index, *this))
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
	if (imp->getrealid(id, realid, index, *this) || (index > 0))
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
	if (imp->getrealid(id, realid, index, *this))
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
	imp->getrealid(id, realid, index, *this);
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
	imp->getrealid(id, realid, index, *this);
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
	imp->getrealid(id, realid, index, *this);
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
	imp->getrealid(id, realid, index, *this);

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
	imp->getrealid(id, realid, index, *this);

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
							  SymbolKeyType& realkey, unsigned& index,
							  const Symbols& parent)
{
	if (id[0] == '$')
		return getrealid(id.substr(2, id.size()-3), realkey, index, parent);
/*
	else if (id.find('$') != SymbolKeyType::npos)
	{
		// When id contains embedded ${id}, recurse to build new id.
		// Note: This is really inefficient, and is only here for
		// compatibility.
		Buffer buf(id.c_str(), id.size());
		Symbols copy(parent);
		Parser p(buf, &copy);
		SymbolKeyType newid(p.run());
		return getrealid(newid, realkey, index, parent);
	}
*/

	// Check for array ref
	size_t obracket(id.find('['));
	if (obracket != SymbolKeyType::npos)
	{
		// There is an array index here, so count brackets until close
		unsigned level = 1;
		size_t cbracket=obracket+1;
		while (id[cbracket] && level)
		{
			if (id[cbracket] == '[')
				++level;
			else if (id[cbracket] == ']')
				--level;
			++cbracket;
		}

		std::string temp(id.substr(obracket+1, cbracket-obracket-2));
		if (isnumber(temp.c_str()))
		{
			// if temp is just a number, just get the number
			index = std::atoi(temp.c_str());
		}
		else
		{
			// otherwise instantiate a parser to handle expression
			std::string expr("@eval(");
			expr+= temp;
			expr+= ")";
			index = atoi(eval(expr, &parent).c_str());
		}
		realkey = id.substr(0, obracket);
	}
	else
	{
		index = 0;
		realkey = id;
	}

	return false;
}


bool Symbols::Impl::isnumber(const char* str)
{
	while (*str)
	{
		if ((*str < '0') || (*str > '9'))
			return false;
		++str;
	}
	return true;
}


/**
 *
 * For debugging purposes
 *
 */
void Symbols::dump()
{
	SymbolTable::const_iterator it(imp->symmap.begin()),
		end(imp->symmap.end());

	for (; it != end; ++it)
	{
		if ((*it).second.empty())
			std::cout << (*it).first << " is empty" << std::endl;
		else
			std::cout << (*it).first << " = " << (*it).second[0] << std::endl;
	}
}


} // end namespace TPTLib
