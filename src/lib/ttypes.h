/*
 * types.h
 *
 * Define special extended types
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac@tazthecat.net)
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

#ifndef _extendedtypes_types_h
#define _extendedtypes_types_h

#include <algorithm>
#include <vector>

namespace ExtendedTypes {

template<typename T = char> struct ChrSet {
	std::vector< T > names;
	ChrSet(const T*);
	ChrSet(const ChrSet<T>& cs) : names(cs.names) {}

	// Assignment operator
	ChrSet& operator=(const ChrSet<T>& cs)
		{ names = cs.names; return *this; }

	// Boolean operator
	friend bool operator==(T a, const ChrSet<T>& b)
	{ return std::binary_search(b.names.begin(), b.names.end(), a); }
	friend bool operator==(const ChrSet<T>& a, T b)
	{ return std::binary_search(a.names.begin(), a.names.end(), b); }
	friend bool operator!=(T a, const ChrSet<T>& b)
	{ return !std::binary_search(b.names.begin(), b.names.end(), a); }
	friend bool operator!=(const ChrSet<T>& a, T b)
	{ return !std::binary_search(a.names.begin(), a.names.end(), b); }
};


} // end namespace ExtendedTypes

#include "ttypes.inl"

#endif // _extendedtypes_types_h
