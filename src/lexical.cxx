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

#include "ttypes.h"
#include "lexical.h"
#include "tptlib/buffer.h"

namespace TPTLib {

using ExtendedTypes::ChrSet;

const ChrSet<> set_digits("0-9");
const ChrSet<> set_hex("0-9a-fA-F");
const ChrSet<> set_alphalower("a-z");
const ChrSet<> set_alphaupper("A-Z");
const ChrSet<> set_alpha("a-zA-Z");
const ChrSet<> set_alnum("a-zA-Z0-9");
const ChrSet<> set_varname("a-zA-Z0-9_");
const ChrSet<> set_funcname("a-zA-Z");
const ChrSet<> set_whitespace(" \t\r");


static void buildtoken(std::string& value, Buffer& buf, const ChrSet<>& testset)
{
	char c;
	while (buf)
	{
		c = buf.getnextchar();
		if (c == testset)
			value+= c;
		else
		{
			buf.unget(c);
			break;
		}
	}
}


struct Lex::Impl {
	Buffer& buf;
	Impl(Buffer& b) : buf(b) {}
};

Lex::Lex(Buffer& buf)
{
	imp = new Impl(buf);
}

Lex::~Lex()
{
	delete imp;
}

Token<> Lex::gettoken()
{
	Token<> t;
	char c;

	c = imp->buf.getnextchar();
	t.value = c;

	// Group whitespace together
	if (c == set_whitespace)
	{
		t.type = token_whitespace;
		buildtoken(t.value, imp->buf, set_whitespace);
	}
	// Group digits together
	else if (c == set_digits)
	{
		t.type = token_integer;
		buildtoken(t.value, imp->buf, set_digits);
	}
	// Extract words
	else if (c == set_alpha)
	{
		t.type = token_text;
		buildtoken(t.value, imp->buf, set_alpha);
	}
	// Handle variable names
	else if (c == '$')
	{
		c = imp->buf.getnextchar();
		if (c != '{')
		{
			imp->buf.unget(c);
			t.type = token_symbol;
			return t;
		}
		t.value+= c;
		c = imp->buf.getnextchar();
		if (c != set_varname)
		{
			imp->buf.unget(c);
			t.type = token_error;
			return t;
		}
		t.value+= c;
		buildtoken(t.value, imp->buf, set_varname);
		c = imp->buf.getnextchar();
		if (c != '}')
		{
			imp->buf.unget(c);
			t.type = token_error;
			return t;
		}
		t.type = token_variable;
	}
	// Handle function names
	else if (c == '@')
	{
		c = imp->buf.getnextchar();
		if (c != set_funcname)
		{
			imp->buf.unget(c);
			t.type = token_symbol;
			return t;
		}
		t.value+= c;
		buildtoken(t.value, imp->buf, set_funcname);
		t.type = token_function;
	}
	else
	{
		switch (c) {
		case '{': t.type = token_openbrace; break;
		case '}': t.type = token_closebrace; break;
		case '(': t.type = token_openparen; break;
		case ')': t.type = token_closeparen; break;
		case '"': t.type = token_quote; break;
		case '\\': t.type = token_escape; break;
		case '#': t.type = token_comment; break;
		default: t.type = token_symbol; break;
		}
	}

	return t;
}

} // end namespace TPTLib
