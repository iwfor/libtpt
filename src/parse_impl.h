/*
 * parse_impl.h
 *
 * Parser Implementation
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

#ifndef __tptlib_parse_impl_h
#define __tptlib_parse_impl_h

#ifdef _MSC_VER
// Disable long symbol name warning on MSVC++
#pragma warning(disable:4786)
#endif

#include <tptlib/parse.h>
#include "lexical.h"
#include "macro.h"

#ifdef _MSC_VER
#include <native.h>
#else
#include <sys/types.h>
#endif

namespace TPTLib {

struct Parser::Impl {
	Lex lex;
	unsigned level;	// if/loop level
	Symbols localsymmap;
	Symbols& symbols;	// reference to whatever symbol map is in use
	MacroList localmacros;
	MacroList& macros;	// reference to whatever macro list is in use
	ErrorList errlist;
	bool isseeded;

	// kiss_vars are used for pseudo-random number generation
	unsigned long kiss_x;
	unsigned long kiss_y;
	unsigned long kiss_z;
	unsigned long kiss_w;
	unsigned long kiss_carry;
	unsigned long kiss_k;
	unsigned long kiss_m;
	unsigned long kiss_seed;

	Impl(Buffer& buf, const Symbols* st) : lex(buf), level(0),
		symbols(localsymmap), macros(localmacros), isseeded(false)
	{ if (st) localsymmap = *st; }

	Impl(Buffer& buf, Symbols& sm, MacroList& ml) : lex(buf), level(0),
		symbols(sm), macros(ml), isseeded(false) { }

	~Impl() { };

	void recorderror(const std::string& desc, const Token<>* neartoken=0);
	bool getnextparam(std::string& value);
	bool getnextstrict(Token<>& target);
	bool getparamlist(ParamList& pl);
	bool getidparamlist(std::string& id, ParamList& pl);

	bool pass1(std::ostream* os);

	int64_t str2num(const char* str);
	inline int64_t str2num(const std::string& str)
	{ return str2num(str.c_str()); }
	void num2str(int64_t value, std::string& str);

	Token<> parse_level0(Token<>& left);
	Token<> parse_level1(Token<>& left);
	Token<> parse_level2(Token<>& left);
	Token<> parse_level3(Token<>& left);
	Token<> parse_level4(Token<>& left);
	Token<> parse_level5(Token<>& left);
	Token<> parse_level6(Token<>& left);
	Token<> parse_level7(Token<>& left);

	void srandom32();
	unsigned int random32();
	Token<> parse_rand();	// generate pseudorandom number

	Token<> parse_empty();	// check emptiness of symbol
	Token<> parse_concat();	// concatenate strings
	Token<> parse_length();	// get length of a string
	Token<> parse_substr();	// get a sub string
	Token<> parse_uc();		// uppercase a string
	Token<> parse_lc();		// lowercase a string
	Token<> parse_size();	// get size of array

	void parse_main(std::ostream* os);
	void parse_block(std::ostream* os);
	bool parse_loopblock(std::ostream* os);
	void parse_dotoken(std::ostream* os, Token<> tok);

	void parse_include(std::ostream* os);
	void parse_if(std::ostream* os);
	bool parse_ifexpr(std::ostream* os);
	void parse_foreach(std::ostream* os);
	void parse_while(std::ostream* os);
	void parse_set();
	void parse_setif();

	void parse_macro();
	void user_macro(const std::string& name, std::ostream* os);
};


} // end namespace TPTLib

#endif // __tptlib_parse_impl_h
