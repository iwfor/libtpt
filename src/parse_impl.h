/*
 * parse_impl.h
 *
 * Parser Implementation
 *
 * $Id$
 *
 */

#ifndef __tptlib_parse_impl_h
#define __tptlib_parse_impl_h

#include <tptlib/parse.h>
#include "lexical.h"
#include "macro.h"

namespace TPTLib {

struct Parser::Impl {
	Lex lex;
	unsigned level;	// if/loop level
	SymbolMap symbols;
	ErrorList errlist;
	Token<> tok;
	MacroList macros;

	Impl(Buffer& buf, const SymbolTable* st) : lex(buf), level(0)
	{ if (st) symbols = *st; }
	void recorderror(const std::string& desc);
	bool getnextparam(std::string& value);
	bool getnextnonwhitespace();
	bool getparamlist(ParamList& pl);

	bool pass1(std::ostream* os);
	void parse_include(std::ostream* os);
	void parse_if(std::ostream* os);
	void parse_macro();
	void parse_set();
	void parse_empty(std::ostream* os);
	void parse_rand(std::ostream* os);

	void addmacro(const std::string& name, const ParamList& params,
		const std::string& body);
	void execmacro(const std::string& name, const ParamList& params);
};


} // end namespace TPTLib

#endif // __tptlib_parse_impl_h
