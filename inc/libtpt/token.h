/*
 * token.h
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

#ifndef _libtpt_token_h
#define _libtpt_token_h

#include <string>

namespace TPT {

enum TokenTypes {
	token_error = 0,
	token_eof,
	token_id,			// ${id name}
	token_usermacro,	// @userdefinedmacro
	token_integer,		// 12345
	token_float,		// 123.45
	token_string,		// "string"
	token_text,			// alphanum
	token_comment,		// # comment character
	token_whitespace,	// space, tab, \r, \n, \r\n
	token_joinline,		// \\n, \\r, \\r\n
	token_escape,		// \escape character
	token_openbrace,	// {
	token_closebrace,	// }
	token_openparen,	// (
	token_closeparen,	// )
	token_comma,		// ,
	token_quote,		// "
	token_singlequote,	// '
	token_operator,		// + - */ % || && ^^
	token_relop,		// < <= == != > >=
	token_include,		// @include
	token_set,			// @set
	token_setif,		// @setif
	token_push,			// @push
	token_pop,			// @pop
	token_unset,		// @unset
	token_keys,			// @keys
	token_macro,		// @macro
	token_foreach,		// @foreach
	token_while,		// @while
	token_next,			// @next
	token_last,			// @last
	token_if,			// @if
	token_else,			// @else
	token_elsif,		// @elsif
	token_compare,		// @compare(str, str)

	token_empty,		// @empty
	token_size,			// @size (of array)

	// Math
	token_rand,			// @rand

	token_isscalar,		// @isscalar
	token_isarray,		// @isarray
	token_ishash		// @ishash
};

template<typename E=TokenTypes> struct Token {
	typedef E en;
	E type;
	std::string value;
	unsigned column;
	unsigned lineno;

	Token<E>() {}
	Token<E>(const Token<E>& t) : type(t.type),value(t.value),
		column(t.column), lineno(t.lineno) {}
	Token<E>(const std::string& newval, unsigned newcol, unsigned newlineno) :
		value(newval), column(newcol), lineno(newlineno) {}
	Token<E>(unsigned newcol, unsigned newlineno) :
		column(newcol), lineno(newlineno) {}
	Token<E>& operator=(const Token<E>& t)
	{
		type=t.type;
		value=t.value;
		column=t.column;
		lineno=t.lineno;
		return *this;
	}
	~Token<E>() {}
};

} // end namespace TPT

#endif // _libtpt_token_h
