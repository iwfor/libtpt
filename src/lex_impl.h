/*
 * lex_impl.h
 *
 * Lexical Analyzer Implementation
 *
 * $Id$
 *
 */

#ifndef __TPTLib_lex_impl_h
#define __TPTLib_lex_impl_h

#include "lexical.h"
#include "ttypes.h"
#include "tptlib/buffer.h"

namespace {
	using ExtendedTypes::ChrSet;
	const ChrSet<> set_num("0-9");
	const ChrSet<> set_alpha("a-zA-Z");
	const ChrSet<> set_alphanum("a-zA-Z0-9");
	// rawtext contains pretty much everything but reserved (i.e. $ @ { } )
	const ChrSet<> set_rawtext("a-zA-Z0-9()[],.!?:;&*~`-_+=");
	const ChrSet<> set_varname("a-zA-Z0-9_");
	const ChrSet<> set_startvarname("a-zA-Z_.");
	const ChrSet<> set_whitespace(" \t\r\n");
}

namespace TPTLib {

using ExtendedTypes::ChrSet;

struct Lex::Impl {
	Buffer& buf;
	unsigned lineno;
	unsigned column;

	Impl(Buffer& b) : buf(b), lineno(1), column(1) {}

	char safeget(Buffer& buf)
	{ if (!buf) return 0; ++column; return buf.getnextchar(); }
	void safeunget(Buffer& buf)
	{ --column; buf.unget(); }
	Token<>::en checkreserved(const char* str);
	void buildtoken(std::string& value, Buffer& buf,
		const ChrSet<>& testset);
	void eatwhitespace(std::string& value, Buffer& buf);

	void getidname(Token<>& t, Buffer& buf);
	void getclosedidname(Token<>& t, Buffer& buf);
	void getbracketexpr(Token<>& t, Buffer& buf);

	void getstring(Token<>& t, Buffer& buf);
	void getsqstring(Token<>& t, Buffer& buf);

	bool makereturn(char c, std::string& cr, Buffer& buf);
	bool isreturn(const Token<>& t);

	void buildcomment(Buffer& b, Token<>& t);
	void newline();
};

} // end namespace TPTLib

#endif // __TPTLib_lex_impl_h
