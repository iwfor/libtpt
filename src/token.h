/*
 * token.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_token_h
#define __tptlib_token_h

#include <string>

namespace TPTLib {

enum TokenTypes {
	token_error = 0,
	token_eof,
	token_id,			// ${id name}
	token_usermacro,	// @userdefinedmacro
	token_integer,		// 12345
	token_string,		// "string"
	token_array,		// "string", "string", "string"
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
	token_macro,		// @macro
	token_foreach,		// @foreach
	token_while,		// @while
	token_next,			// @next
	token_last,			// @last
	token_if,			// @if
	token_else,			// @else
	token_elsif,		// @elsif
//	token_unless,		// @unless (should I?)

	token_empty,		// @empty
	token_rand,			// @rand
	token_concat,		// @concat
	token_length,		// @length (of string)
	token_substr,		// @substr
	token_uc,			// @uc (uppercase)
	token_lc,			// @lc (lowercase)
	token_size,			// @size (of array)
};

template<typename E=TokenTypes> struct Token {
	E type;
	typedef E en;
	std::string value;
	Token<E>() {}
	Token<E>(const Token<E>& t) : type(t.type),value(t.value) {}
	Token<E>& operator=(const Token<E>& t)
	{type=t.type;value=t.value; return *this;}
};

} // end namespace TPTLib

#endif // __tptlib_token_h
