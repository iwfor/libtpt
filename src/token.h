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
	token_symbol,		// undefined symbol
	token_variable,		// ${variable name}
	token_function,		// @func
	token_integer,		// 12345
	token_text,			// alpha
	token_whitespace,	// space, tab, carriage return symbol
	token_comment,		// # comment character
	token_escape,		// \escape character
	token_newline,		// \n newline
	token_openbrace,	// {
	token_closebrace,	// }
	token_openparen,	// (
	token_closeparen,	// )
	token_quote,		// "
	token_plus,			// +
	token_minus,		// -
	token_mult,			// *
	token_divide,		// /
};

template<typename E=TokenTypes> struct Token {
	E type;
	std::string value;
};

} // end namespace TPTLib

#endif // __tptlib_token_h
