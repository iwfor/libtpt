/*
 * lexical.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_lexical_h
#define __tptlib_lexical_h

#include "token.h"

namespace TPTLib {

class Buffer;

class Lex {
public:
	Lex(Buffer& buf);
	~Lex();

	Token<> getloosetoken();
	Token<> getstricttoken();

private:
	struct Impl;
	Impl* imp;

	Lex();
};


} // end namespace TPTLib

#endif // __tptlib_lexical_h
