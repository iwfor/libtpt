/*
 * lex_impl.h
 *
 * Lexical Analyzer Implementation
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
