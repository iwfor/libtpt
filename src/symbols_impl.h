/*
 * symbols_impl.h
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

#ifndef __tptlib_symbol_impl_h
#define __tptlib_symbol_impl_h

#include <tptlib/symbols.h>

namespace TPTLib {

typedef SymbolArrayType SymbolValue;

struct Symbol_t {
	std::string	id;
	SymbolValue value;

	Symbol_t() {}
	Symbol_t(const Symbol_t& x) : id(x.id), value(x.value) {}
	Symbol_t& operator=(const Symbol_t& x)
	{
		id = x.id;
		value = x.value;
	}
};

typedef std::map< SymbolKeyType, SymbolValue > SymbolTable;


/*
 * The private implementation of Symbols.
 *
 */
struct Symbols::Impl {
public:
	SymbolTable symmap;

	Impl() {};
	Impl(const SymbolTable& sm) : symmap(sm) {}
	~Impl() {};

	bool getrealid(const SymbolKeyType& id, SymbolKeyType& realkey,
		unsigned& index, const Symbols& parent);
	bool istextnumber(const char* str);
};



} // end namespace TPTLib

#endif // __tptlib_symbol_impl_h
