/*
 * lexical.cxx
 *
 * Lexical Analyzer
 *
 * $Id$
 *
 */


/*
 * The lexical analyzer is responsible for converting input
 * text to tokens.
 *
 */

#ifdef _MSC_VER
#	pragma	warning(disable:4786)
#endif

#include "tptlib/buffer.h"
#include "literal.h"

#include <cctype>

namespace TPTLib {

template<typename E> struct Token {
	E type;
	std::string value;
};

class Lex {
public:
	lex(Buffer& buf);

	gettoken();

private:
	struct impl;
	impl* imp;

	lex();
};

void test()
{
	Range<wchar_t> wdr('0','9');
	Set<wchar_t> wdigits(wdr);
	Set<> digits("0-9");
	Set<> hex("0-9a-fA-F");
	Set<> alnum("a-zA-Z");

	ChrLit<> lit_add('+');
	ChrLit<> lit_sub('-');
	ChrLit<> lit_mul('*');
	ChrLit<> lit_div('/');
	ChrLit<> lit_mod('%');
	ChrLit<> lit_opblock('{');
	ChrLit<> lit_clblock('}');
	StrLit<> lit_not("not");
	StrLit<> lit_if("@if");
	StrLit<> lit_else("@else");
	StrLit<> lit_opvar("${");
	ChrLit<> lit_clvar('}');
	StrLit<> lit_oparr("@{");
	ChrLit<> lit_clarr('}');

	bool a;
	a = "+" == lit_add;
	a = lit_add == "+";
	a = lit_not == "not";
	a = hex == '5';

}

} // end namespace TPTLib
