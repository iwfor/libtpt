/*
 * symbols.h
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002-2003 Isaac W. Foraker (isaac@tazthecat.net)
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

#ifndef include_libtpt_symbols_h
#define include_libtpt_symbols_h

#include <string>
#include <map>
#include <vector>
#include <stack>

namespace TPT {

// Forward declarations
class Symbols_Impl;
class Parser_Impl;
class Object;

// The maximum allowed size of an array
const unsigned maxarraysize = 65536;

typedef std::string SymbolKeyType;
typedef std::string SymbolValueType;
typedef std::vector< SymbolValueType > SymbolArrayType;
typedef std::map< SymbolKeyType, SymbolValueType> SymbolHashType;

/**
 * The Symbols class holds the symbols table for passing variables to
 * the parser.
 *
 * @author	Isaac W. Foraker
 * @exception	tptexception
 *
 */
class Symbols {
public:
	Symbols(bool setdefaults=true);
	Symbols(const Symbols& s);
	~Symbols();

	void copy(const Symbols& s);

	bool set(const SymbolKeyType& id, const SymbolValueType& value);
	bool set(const SymbolKeyType& id, int value);
	bool set(const SymbolKeyType& id, const SymbolArrayType& value);
	bool set(const SymbolKeyType& id, const SymbolHashType& value);
	bool set(const SymbolKeyType& id, Object& value);

	bool push(const SymbolKeyType& id, const SymbolValueType& value);
	bool push(const SymbolKeyType& id, const SymbolArrayType& value);
	bool push(const SymbolKeyType& id, const SymbolHashType& value);
	bool push(const SymbolKeyType& id, Object& value);

	bool exists(const SymbolKeyType& id) const;
	bool empty(const SymbolKeyType& id) const;

	bool isscalar(const SymbolKeyType& id) const;
	bool isarray(const SymbolKeyType& id) const;
	bool ishash(const SymbolKeyType& id) const;
	unsigned size(const SymbolKeyType& id) const;
	bool get(const SymbolKeyType& id, SymbolValueType& val) const;
	bool get(const SymbolKeyType& id, SymbolArrayType& sym) const;
	//TODO bool get(const SymbolKeyType& id, SymbolHashType& sym) const;
	//TODO Object& getbase();
	bool unset(const SymbolKeyType& id);
	Symbols& operator=(const Symbols&);
private:
	Symbols_Impl* imp;
	friend class Parser_Impl;
};


} // end namespace TPT

#endif // include_libtpt_symbols_h
