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
#include <iostream>

namespace TPTLib {

using ExtendedTypes::ChrSet;

const ChrSet<> set_num("0-9");
const ChrSet<> set_alpha("a-zA-Z");
const ChrSet<> set_alphanum("a-zA-Z0-9");
const ChrSet<> set_varname("a-zA-Z0-9_");
const ChrSet<> set_startvarname("a-zA-Z_");

struct Lex::Impl {
	Buffer& buf;
	unsigned lineno;
	Impl(Buffer& b) : buf(b), lineno(1) {}
	char safeget(Buffer& buf)
		{ if (!buf) return 0; return buf.getnextchar(); }
	Token<>::en checkreserved(const char* str);
	void buildtoken(std::string& value, Buffer& buf,
		const ChrSet<>& testset);
	void getidname(Token<>& t, Buffer& buf);
	void getclosedidname(Token<>& t, Buffer& buf);
	void getstring(Token<>& t, Buffer& buf);
};

Lex::Lex(Buffer& buf)
{
	imp = new Impl(buf);
}

Lex::~Lex()
{
	delete imp;
}

unsigned Lex::getlineno()
{
	return imp->lineno;
}

Token<> Lex::getloosetoken()
{
	Token<> t;
	char c;
	Buffer& buf = imp->buf;	// I'm lazy, and don't like typing imp->buf

	if (!(c = imp->safeget(buf)))
	{
		t.type = token_eof;	
		return t;
	}

	t.value = c;

	switch (c) {
	case ' ':
	case 255:
	case '\t': t.type = token_whitespace; return t;
	// For simplicity, have the strict token reader process these
	// symbols to avoid duplicate code.
	case '\n':
		t.type = token_whitespace;
		++imp->lineno;
		return t;
	case '\r':
		t.type = token_whitespace;
		c = imp->safeget(buf);
		if (c == '\n') t.value+= c;
		else if (c) buf.unget();
		++imp->lineno;
		return t;
	case '\\':	// escape character
	case '@':	// keyword, macro, or comment
	case '$':	// variable name
		buf.unget();
		return getstricttoken();
	case '}': t.type = token_closebrace; return t;
	default: t.type = token_text; break;
	}
	// Next, process tokens that are comprised of sets
	// Group digits together
	if (c == set_alphanum)
	{
		t.type = token_text;
		imp->buildtoken(t.value, buf, set_alphanum);
	}
	return t;
}

Token<> Lex::getstricttoken()
{
	Token<> t;
	t.type = token_whitespace;
	char c;
	Buffer& buf = imp->buf;	// I'm lazy, and don't like typing imp->buf

	if (!(c = imp->safeget(buf)))
	{
		t.type = token_eof;	
		return t;
	}

	// strict tokens skip white-spaces
	while (t.type == token_whitespace)
	{
		t.value = c;

		switch (c) {
		case '{': t.type = token_openbrace; return t;
		case '}': t.type = token_closebrace; return t;
		case '(': t.type = token_openparen; return t;
		case ')': t.type = token_closeparen; return t;
		case ',': t.type = token_comma; return t;
		case '+':
		case '-':
		case '*':
		case '/':
		case '%': t.type = token_operator; return t;
		case '|':
		case '&':
		case '^':
			c = imp->safeget(buf);
			if (t.value[0] == c)
			{
				t.type = token_operator;
				t.value+= c;
			}
			else
			{
				t.type = token_error;
				if (c) buf.unget();
			}
			return t;
		// Handle whitespace
		case ' ':
		case 255:
		case '\t': t.type = token_whitespace; return t;
		// Handle new lines
		case '\n':
			t.type = token_whitespace;
			++imp->lineno;
			break;
//			return t;
		case '\r':
			t.type = token_whitespace;
			c = imp->safeget(buf);
			if (c == '\n') t.value+= c;
			else if (c) buf.unget();
			++imp->lineno;
			break;
//			return t;
		case '"':	// quoted strings
			imp->getstring(t, buf);
			return t;
		case '\\':	// escape sequences
			c = imp->safeget(buf);
			t.value+= c;
			if (c == '\n')
				t.type = token_joinline;
			else if (c == '\r')
			{
				// handle cases where newlines are represented by \r\n
				t.type = token_joinline;
				c = imp->safeget(buf);
				if (c == '\n')
					t.value+= c;
				else
					if (c) buf.unget();
			}
			else
				t.type = token_escape;
			return t;
		case '@':	// keyword or user macro
			c = imp->safeget(buf);
			if (c == set_startvarname)
			{
				t.value+= c;
				imp->buildtoken(t.value, buf, set_varname);
				t.type = imp->checkreserved(&t.value[1]);
			}
			else if (c == '#') // this is a comment
			{
				// Read everything until the end of line
				while (c = imp->safeget(buf))
				{
					t.value+= c;
					if (c == '\n' || c == '\r')
						buf.unget();
				}
			}
			else
			{
				if (c) buf.unget();
				t.type = token_text;
				return t;
			}
			return t;
		case '$':	// variable name
	//		std::cout << "\nPARSING SYMBOL ID" << std::endl;
			imp->getclosedidname(t, buf);
			return t;
		case '>':
		case '<':
		case '=':
		case '!':
			t.type = token_relop;
			c = imp->safeget(buf);
			if (c == '=')
				t.value+= c;
			else
				if (c) buf.unget();
			return t;
		default: t.type = token_error; break;
		}
	}
	// Next, process tokens that are comprised of sets
	// Group digits together
	if (c == set_num)
	{
		t.type = token_integer;
		imp->buildtoken(t.value, buf, set_num);
	}
	else if (c == set_startvarname)
	{
		imp->getidname(t, buf);
	}

	return t;
}


/*
 * This is a bit of a hack, but seems like the best way
 * to handle current design problems.
 *
 */
void Lex::unget(const Token<>& tok)
{
	unsigned i = tok.value.size();
	while (i--)
		imp->buf.unget();
}


/*
 * Check a function token to see if it is a reserved word, assuming the
 * prepending @ is excluded.
 *
 */
Token<>::en Lex::Impl::checkreserved(const char* str)
{
	// Should not be in this function unless str[0] == '@'
	switch (*str)
	{
	case 'c':
			 if (!std::strcmp(str, "concat"))	return token_concat;
		break;
	case 'e':
			 if (!std::strcmp(str, "else"))		return token_else;
		else if (!std::strcmp(str, "empty"))	return token_empty;
		break;
	case 'f':
			 if (!std::strcmp(str, "foreach"))	return token_foreach;
		break;
	case 'i':
			 if (!std::strcmp(str, "if"))		return token_if;
		else if (!std::strcmp(str,  "include"))	return token_include;
		break;
	case 'l':
			 if (!std::strcmp(str, "last"))		return token_last;
		break;
	case 'm':
			 if (!std::strcmp(str, "macro"))	return token_macro;
		break;
	case 'n':
			 if (!std::strcmp(str, "next"))		return token_next;
		break;
	case 'r':
			 if (!std::strcmp(str, "rand"))		return token_rand;
		break;
	case 's':
			 if (!std::strcmp(str, "set"))		return token_set;
		break;
	case 'w':
			 if (!std::strcmp(str, "while"))	return token_while;
		break;
	default:
		break;
	}
	return token_usermacro;
}

/*
 * Get an enclosed id token, assuming the prepending $ has
 * already been scanned.
 *
 */
void Lex::Impl::getclosedidname(Token<>& t, Buffer& buf)
{
	char c;

	// If next character is not a {, then this is not an id
	c = safeget(buf);
	if (c != '{')
	{
//		std::cout << "\nNOT A SYMBOL AFTER ALL" << std::endl;
		if (c) buf.unget();
		t.type = token_text;
		return;
	}
	t.value+= c;

	// Start keeping track of open braces
	c = safeget(buf);
	if (c == set_startvarname)
		t.value+= c;
	else if (c == '$')
	{
		t.value+= c;
		getclosedidname(t, buf);
	}
	else
	{
		if (c) buf.unget();
		t.type = token_error;
		return;
	}

	while ((c != '}') && (c = safeget(buf)))
	{
		if (c == '$')
		{
			t.value+= c;
			getclosedidname(t, buf);
		}
		else if (c == '}')
			continue;
		else if (c != set_varname)
		{
			t.type = token_error;
			buf.unget();
			break;
		}
		t.value+= c;
	}
	if (c == '}')
	{
		t.value+= c;
		t.type = token_id;
	}
	else
		t.type = token_error;
//	std::cout << "\nCREATED ID: " << t.value << std::endl;
}

/*
 * Get an id token, assuming the first character has been scanned.
 *
 */
void Lex::Impl::getidname(Token<>& t, Buffer& buf)
{
	char c;

	while (c = safeget(buf))
	{
		if (c == '$')
		{
			t.value+= c;
			getclosedidname(t, buf);
			if (t.type != token_id)
			{
				t.type = token_error;
				return;
			}
		}
		else if (c != set_varname)
		{
			buf.unget();
			break;
		}
		t.value+= c;
	}
	t.type = token_id;
}

/*
 * Get a string token, assuming the open quote has already been
 * scanned.
 *
 */
void Lex::Impl::getstring(Token<>& t, Buffer& buf)
{
	char c;
	t.type = token_string;
	t.value.erase();
	while (c = safeget(buf))
	{
		t.value+= c;
		if (c == '"')
		{
			c = safeget(buf);
			if (c != '"')	// hardcoded quote in string
			{
				if (c) buf.unget();
				return;
			}
			// else embed a quote (")
		}
		t.value+= c;
		if (c == '\n' || c == '\r')
		{
			buf.unget();
			t.type = token_error;
			break;
		}
	}
	if (!c)
		t.type = token_error;
}

void Lex::Impl::buildtoken(std::string& value, Buffer& buf,
						   const ChrSet<>& testset)
{
	while (char c = safeget(buf))
	{
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
