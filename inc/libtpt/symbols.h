/*
 * symbols.h
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

#ifndef __tptlib_symbols_h
#define __tptlib_symbols_h

#include <string>
#include <map>
#include <vector>
#include <stack>

namespace TPTLib {

const unsigned maxarraysize = 65536;

typedef std::string SymbolKeyType;
typedef std::string SymbolValueType;
typedef std::vector< SymbolValueType > SymbolArrayType;

class Symbols {
public:
	Symbols();
	Symbols(const Symbols& s);
	~Symbols();

	void set(const SymbolKeyType& id, const SymbolValueType& value);
	void set(const SymbolKeyType& id, const SymbolArrayType& value);
	bool exists(const SymbolKeyType& id) const;
	bool empty(const SymbolKeyType& id) const;
	bool isarray(const SymbolKeyType& id) const;
	unsigned size(const SymbolKeyType& id) const;
	bool get(const SymbolKeyType& id, SymbolValueType& val) const;
	bool get(const SymbolKeyType& id, SymbolArrayType& sym) const;
	bool unset(const SymbolKeyType& id);
	void dump();
	Symbols& operator=(const Symbols&);
private:
	struct Impl;
	Impl* imp;
};


} // end namespace TPTLib

#endif // __tptlib_symbols_h
