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

#include <cstring>

namespace TPTLib {

using ExtendedTypes::ChrSet;

const ChrSet<> set_digits("0-9");
const ChrSet<> set_alpha("a-zA-Z");
const ChrSet<> set_varname("a-zA-Z0-9_");
const ChrSet<> set_startvarname("a-zA-Z_");

struct Lex::Impl {
	Buffer& buf;
	Impl(Buffer& b) : buf(b) {}
	char safeget(Buffer& buf)
		{ if (!buf) return 0; return buf.getnextchar(); }
	Token<>::en getfunctiontype(const char* str);
	void buildtoken(std::string& value, Buffer& buf,
		const ChrSet<>& testset);
	void getidname(Token<>& t, Buffer& buf);
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
	Buffer& buf = imp->buf;	// I'm lazy, and don't like typing imp->buf

	if (!buf)
	{
		t.type = token_eof;	
	}

	c = buf.getnextchar();
	t.value = c;

	// First try to process the simple tokens
	switch (c) {
	case '{': t.type = token_openbrace; return t;
	case '}': t.type = token_closebrace; return t;
	case '(': t.type = token_openparen; return t;
	case ')': t.type = token_closeparen; return t;
	case '+':
	case '-':
	case '*':
	case '/':
	case '%': t.type = token_operator; return t;
	case '|':
	case '&':
	case '^':
		c = buf.getnextchar();
		if (t.value[0] == c)
		{
			t.type = token_operator;
			t.value+= c;
		}
		else
		{
			t.type = token_symbol;
			buf.unget();
		}
		return t;
	case ' ':
	case 255:
	case '\t': t.type = token_whitespace; return t;
	case '\n': t.type = token_newline; return t;
	case '\r':
		t.type = token_newline;
		if (c == '\r')
		{
			c = buf.getnextchar();
			if (c == '\n')
				t.value+= c;
			else
				buf.unget();
		}
		return t;
	case '"':	// quoted strings
		t.type = token_string;
		while (buf)
		{
			c = buf.getnextchar();
			t.value+= c;
			if (c == '"')
				break;
			if (c == '\n' || c == '\r')
			{
				buf.unget();
				t.type = token_escape;
				break;
			}
		}
		return t;
	case '\\':	// escape sequences
		c = buf.getnextchar();
		t.value+= c;
		if (c == '\n')
			t.type = token_joinline;
		else if (c == '\r')
		{
			// handle cases where newlines are represented by \r\n
			t.type = token_joinline;
			c = buf.getnextchar();
			if (c == '\n')
				t.value+= c;
			else
				buf.unget();
		}
		else
			t.type = token_escape;
		return t;
	case '@':	// keyword or user macro
		c = buf.getnextchar();
		if (c ==set_varname)
		{
			t.value+= c;
			imp->buildtoken(t.value, buf, set_varname);
			t.type = imp->getfunctiontype(&t.value[1]);
		}
		else if (c == '#') // this is a comment
		{
			while (buf)
			{
				c = buf.getnextchar();
				t.value+= c;
				if (c == '\n' || c == '\r')
					buf.unget();
			}
		}
		else
		{
			buf.unget();
			t.type = token_symbol;
			return t;
		}
		return t;
	case '$':	// variable name
		imp->getidname(t, buf);
		return t;
	case '>':
	case '<':
	case '=':
	case '!':
		t.type = token_relop;
		c = buf.getnextchar();
		if (c == '=')
			t.value+= c;
		else
			buf.unget();
		return t;
	default: t.type = token_symbol; break;
	}
	// Next, process tokens that are comprised of sets
	// Group digits together
	if (c == set_digits)
	{
		t.type = token_integer;
		imp->buildtoken(t.value, buf, set_digits);
	}
	// Extract words
	else if (c == set_alpha)
	{
		t.type = token_text;
		imp->buildtoken(t.value, buf, set_alpha);
	}

	return t;
}


Token<>::en Lex::Impl::getfunctiontype(const char* str)
{
	// Should not be in this function unless str[0] == '@'
	switch (*str)
	{
	case 'e':
		if (!std::strcmp(str, "else")) return token_else;
		break;
	case 'f':
		if (!std::strcmp(str, "foreach")) return token_foreach;
		break;
	case 'i':
		if (!std::strcmp(str, "if")) return token_if;
		else if (!std::strcmp(str,  "include")) return token_include;
		break;
	case 'l':
		if (!std::strcmp(str, "last")) return token_last;
		break;
	case 'm':
		if (!std::strcmp(str, "macro")) return token_macro;
		break;
	case 'n':
		if (!std::strcmp(str, "next")) return token_next;
		break;
	case 'r':
		if (!std::strcmp(str, "rand")) return token_rand;
		break;
	case 's':
		if (!std::strcmp(str, "set")) return token_set;
		break;
	case 'w':
		if (!std::strcmp(str, "while")) return token_while;
		break;
	default:
		break;
	}
	return token_usermacro;
}

void Lex::Impl::getidname(Token<>& t, Buffer& buf)
{
	char c;

	// If next character is not a {, then this is not an id
	c = safeget(buf);
	if (c != '{')
	{
		if (c) buf.unget();
		t.type = token_symbol;
		return;
	}
	t.value+= c;

	// Start keeping track of open braces
	c = safeget(buf);
	if (c == set_startvarname)
		t.value = c;
	else if (c == '$')
	{
		buf.unget();
		getidname(t, buf);
	}
	else
	{
		if (c) buf.unget();
		t.type = token_error;
		return;
	}

	while ((c == '}') && (c = safeget(buf)))
	{
		if (c == '$')
		{
			buf.unget();
			getidname(t, buf);
		}
		else if (c != set_varname)
		{
			t.type = token_error;
			buf.unget();
			break;
		}
		t.value+= c;
	}
	if (c == '}')
		t.type = token_id;
	else
		t.type = token_error;
}

void Lex::Impl::buildtoken(std::string& value, Buffer& buf,
						   const ChrSet<>& testset)
{
	char c;
	while (buf)
	{
		c = buf.getnextchar();
		if (c == testset)
			value+= c;
		else
		{
			buf.unget();
			break;
		}
	}
}

} // end namespace TPTLib
