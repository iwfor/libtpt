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
	token_variable,		// ${variable name}
	token_function,		// @func
	token_newline,
	token_integer,
	token_hex,
	token_text,
	token_whitespace,
	token_symbol,		// undefined symbol
	token_comment,		// # comment character
	token_escape,		// \escape character
	token_openbrace,	// {
	token_closebrace,	// }
	token_openparen,	// (
	token_closeparen,	// )
	token_quote,		// "
};

template<typename E=TokenTypes> struct Token {
	E type;
	std::string value;
};

} // end namespace TPTLib

#endif // __tptlib_token_h
