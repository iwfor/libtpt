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

/*
 * TODO:
 *	Add \escape support to strings
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
const ChrSet<> set_startvarname("a-zA-Z_.");
const ChrSet<> set_whitespace(" \t\r\n");

struct Lex::Impl {
	Buffer& buf;
	unsigned lineno;
	Token<> lasttoken;

	Impl(Buffer& b) : buf(b), lineno(1) {
		lasttoken.type = token_whitespace;
		lasttoken.value = "\n";
	}

	char safeget(Buffer& buf)
	{ if (!buf) return 0; return buf.getnextchar(); }
	Token<>::en checkreserved(const char* str);
	void buildtoken(std::string& value, Buffer& buf,
		const ChrSet<>& testset);
	void eatwhitespace(std::string& value, Buffer& buf);

	void getidname(Token<>& t, Buffer& buf);
	void getclosedidname(Token<>& t, Buffer& buf);
	void getbracketexpr(Token<>& t, Buffer& buf);

	void getstring(Token<>& t, Buffer& buf);
	void getsqstring(Token<>& t, Buffer& buf);
};

Lex::Lex(Buffer& buf)
{
	imp = new Impl(buf);
}

Lex::~Lex()
{
	delete imp;
}

unsigned Lex::getlineno() const
{
	return imp->lineno;
}

Token<> Lex::getloosetoken()
{
	Token<> t;
	char c;
	Buffer& buf = imp->buf;	// Lazy typist reference

	if (!(c = imp->safeget(buf)))
	{
		t.type = token_eof;	
		return t;
	}
	t.value = c;

	switch (c) {
	case ' ':
	case '\t':
		t.type = token_whitespace;
		while ( (c = imp->safeget(buf)) )
		{
			if ((c == ' ') || (c == '\t'))
				t.value+= c;
			else if (c == '@')
			{
				// check for comment
				c = imp->safeget(buf);
				if (c == '#')
				{
					while ( (c = imp->safeget(buf)) )
					{
						if ((c == '\r') || (c == '\n'))
						{
							buf.unget();
							break;
						}
					}
					t.type = token_comment;
				}
				else
				{
					if (c) buf.unget();
					buf.unget();
				}
				break;
			}
			else
			{
				buf.unget();
				break;
			}
		}
		return t;
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
	case '{':	// start block
	case '}':	// end block
		buf.unget();
		return getstricttoken();
	default: t.type = token_text; break;
	}
	// Next, process tokens that are comprised of sets
	// Group digits together
	if (c == set_alphanum)
	{
		t.type = token_text;
		imp->buildtoken(t.value, buf, set_alphanum);
	}

	imp->lasttoken = t;
	return t;
}

Token<> Lex::getstricttoken()
{
	Token<> t;
	t.type = token_whitespace;
	char c;
	Buffer& buf = imp->buf;	// Lazy typist reference

	// strict tokens skip white-spaces
	while ((t.type == token_whitespace) || (t.type == token_comment))
	{
		if (!(c = imp->safeget(buf)))
		{
			t.type = token_eof;	
			return t;
		}

		t.value = c;

		switch (c) {
		case '{':
			t.type = token_openbrace;
			// Ignore all whitespace after open brace
			imp->eatwhitespace(t.value, buf);
			return t;
		case '}':
			t.type = token_closebrace;
			// Ignore all whitespace after close brace
			imp->eatwhitespace(t.value, buf);
			return t;
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
		case '\t':
			t.type = token_whitespace;
			break;
		// Handle new lines
		case '\n':
			t.type = token_whitespace;
			++imp->lineno;
			break;
		case '\r':
			t.type = token_whitespace;
			c = imp->safeget(buf);
			if (c == '\n') t.value+= c;
			else if (c) buf.unget();
			++imp->lineno;
			break;
		case '"':	// quoted strings
			imp->getstring(t, buf);
			return t;
		case '\'':
			imp->getsqstring(t, buf);
			return t;
		case '\\':	// escape sequences
			c = imp->safeget(buf);
			t.value+= c;
			if (c == '\n')
			{
				++imp->lineno;
				t.type = token_joinline;
			}
			else if (c == '\r')
			{
				++imp->lineno;
				// handle cases where newlines are represented by \r\n
				t.type = token_joinline;
				c = imp->safeget(buf);
				if (c == '\n')
					t.value+= c;
				else
					if (c) buf.unget();
			}
			else
			{
				t.type = token_escape;
				t.value = c;
			}
			return t;
		case '@':	// keyword or user macro
			c = imp->safeget(buf);
			t.value+= c;
			if (c == set_startvarname)
			{
				imp->buildtoken(t.value, buf, set_varname);
				t.type = imp->checkreserved(&t.value[1]);
			}
			else if (c == '#') // this is a @# comment
			{
				// Read everything until the end of line
				while ( (c = imp->safeget(buf)) )
				{
					if (c == '\n')
					{
						if (imp->lasttoken.type == token_whitespace)
						{
							// Don't forget the line number!
							++imp->lineno;
						}
						else
						{
							buf.unget();
						}
						break;
					}
					else if (c == '\r')
					{
						if (imp->lasttoken.type == token_whitespace)
						{
							c = imp->safeget(buf);
							if (c && (c != '\n')) buf.unget();
							// Don't forget the line number!
							++imp->lineno;
						}
						else
						{
							buf.unget();
						}
					}
					t.value+= c;
				}
				t.type = token_comment;
			}
			else
			{
				if (c) buf.unget();
				t.type = token_text;
				return t;
			}
			if (t.type == token_comment)
				continue;
			return t;
		case '$':	// variable name
			imp->getclosedidname(t, buf);
			return t;
		case '!':
			c = imp->safeget(buf);
			if (c == '=')
			{
				t.type = token_relop;
				t.value+= c;
			}
			else
			{
				t.type = token_operator;
				if (c) buf.unget();
			}
			return t;
		case '>':
		case '<':
		case '=':
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

	imp->lasttoken = t;
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
		if (!std::strcmp(str, "elsif"))		return token_elsif;
		if (!std::strcmp(str, "empty"))		return token_empty;
		if (!std::strcmp(str, "eval"))		return token_eval;
		break;
	case 'f':
		if (!std::strcmp(str, "foreach"))	return token_foreach;
		break;
	case 'i':
		if (!std::strcmp(str, "if"))		return token_if;
		if (!std::strcmp(str,  "include"))	return token_include;
		break;
	case 'l':
		if (!std::strcmp(str, "last"))		return token_last;
		if (!std::strcmp(str, "lc"))		return token_lc;
		if (!std::strcmp(str, "length"))	return token_length;
		break;
	case 'm':
		if (!std::strcmp(str, "macro"))		return token_macro;
		break;
	case 'n':
		if (!std::strcmp(str, "next"))		return token_next;
		break;
	case 'r':
		if (!std::strcmp(str, "rand"))		return token_rand;
		break;
	case 's':
		if (!std::strcmp(str, "set"))		return token_set;
		if (!std::strcmp(str, "setif"))		return token_setif;
		if (!std::strcmp(str, "size"))		return token_size;
		if (!std::strcmp(str, "substr"))	return token_substr;
		break;
	case 'u':
		if (!std::strcmp(str, "uc"))		return token_uc;
		break;
	case 'w':
		if (!std::strcmp(str, "while"))		return token_while;
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
		if (c) buf.unget();
		t.type = token_text;
		return;
	}
	t.value+= c;

	// Use getidname instead of duplicating code
	getidname(t, buf);
	if (t.type != token_id)
	{
		t.type = token_error;
		return;
	}
	// Get closing brace
	c = safeget(buf);
	if (c == '}')
	{
		t.value+= c;
		t.type = token_id;
	}
	else
		t.type = token_error;
}

/*
 * Get an id token, assuming the first character has been scanned.
 *
 */
void Lex::Impl::getidname(Token<>& t, Buffer& buf)
{
	char c;

	t.type = token_id;
	while ( (t.type != token_error) && (c = safeget(buf)) )
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
		else if (c == '[')
		{
			t.value+= c;
			getbracketexpr(t, buf);
			c = safeget(buf);
			if (c != ']')
				t.type = token_error;
			else
				t.value+= c;
			break;
		}
		else if (c != set_varname)
		{
			buf.unget();
			break;
		}
		else
			t.value+= c;
	}
}

void Lex::Impl::getbracketexpr(Token<>& t, Buffer& buf)
{
	// assume last char was '['
	char c = 0;
	while ( (t.type != token_error) && (c = safeget(buf)) )
	{
		if (c == ']')
		{
			buf.unget();
			return;
		}
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
		else if (c == set_startvarname)
		{
			buf.unget();
			getidname(t, buf);
		}
		else
			t.value+= c;
	}
	if (!c)
		t.type = token_error;
	return;
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
	while ( (c = safeget(buf)) )
	{
		if (c == '"')
			return;
		else if (c == '\\')
		{
			if ( !(c = safeget(buf)) )
			{
				t.type = token_error;
				break;
			}
			switch (c)
			{
			case 'n':
				c = '\n';
				break;
			case 'r':
				c = '\r';
				break;
			case 't':
				c = '\t';
				break;
			case 'a':
				c = '\a';
				break;
			}
		}
		else if (c == '\n' || c == '\r')
		{
			buf.unget();
			t.type = token_error;
			break;
		}
		t.value+= c;
	}
	if (!c)
		t.type = token_error;
}


/*
 * Get a string token, assuming open single quote has already been
 * scanned.
 *
 */
void Lex::Impl::getsqstring(Token<>& t, Buffer& buf)
{
	char c;
	t.type = token_string;
	t.value.erase();
	while ( (c = safeget(buf)) )
	{
		if (c == '\'')
			return;
		else if (c == '\\')
		{
			if ( !(c = safeget(buf)) )
			{
				t.type = token_error;
				break;
			}
			switch (c)
			{
			case 'n':
				c = '\n';
				break;
			case 'r':
				c = '\r';
				break;
			case 't':
				c = '\t';
				break;
			case 'a':
				c = '\a';
				break;
			}
		}
		else if (c == '\n' || c == '\r')
		{
			buf.unget();
			t.type = token_error;
			break;
		}
		t.value+= c;
	}
	if (!c)
		t.type = token_error;
}

/*
 * Build a token based on the given testset.
 *
 */
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

void Lex::Impl::eatwhitespace(std::string& value, Buffer& buf)
{
	char c;
	while ( (c = safeget(buf)) )
	{
		if ((c == ' ') || (c == '\t'))
			value+= c;
		else
			break;
	}
	// eat a newline
	if (c == '\r')
	{
		++lineno;
		value+= c;
		c = safeget(buf);
		if (!c)
			return;
		if (c == '\n')
			value+= c;
		else
			buf.unget();
	}
	else if (c == '\n')
	{
		++lineno;
		value+= c;
	}
	else
		buf.unget();
}

/*
 *
 * Create a Buffer of the next brace enclosed {} block of template.
 *
 * The Buffer returned by getblock() is created with new, so should
 * be deleted when no longer in use.
 *
 */
std::string Lex::getblock()
{
	Buffer &buf = imp->buf;	// lazy typist reference
	char c = imp->safeget(buf);
	unsigned depth = 0;
	std::string block;

	// Ignore all whitespace before the opening brace;
	while (c == set_whitespace)
		c = imp->safeget(buf);

	// If the current character is not an open brace, then something is
	// wrong, so abort this function.
	if (c != '{')
		return "";

	block = c;

	// Iterate through the buffer until the close brace for this block is found
	while (c)
	{
		c = imp->safeget(buf);
		block+= c;
		if (c == '\\')	// escaped sequence
		{
			c = imp->safeget(buf);
			if (!c)
				return "";
			// Now just ignore this character, whatever it is.
			block+= c;
		}
		else if (c == '{')
			++depth;
		else if (c == '}')
		{
			imp->eatwhitespace(block, buf);	
			if (depth > 0)
				--depth;
			else
				break;
		}
	}

	if (c != '}')
		return "";

	return block;
}


} // end namespace TPTLib
