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
const ChrSet<> set_text("a-zA-Z0-9()[]");
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

	bool makereturn(char c, std::string& cr, Buffer& buf);
	bool isreturn(const Token<>& t);

	void buildcomment(Buffer& b, Token<>& t);
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
	t.type = token_error;
	char c;
	Buffer& buf = imp->buf;	// Lazy typist reference

	if (!(c = imp->safeget(buf)))
	{
		t.type = token_eof;	
		return t;
	}
	t.value = c;

	switch (c) {
	// For simplicity, have the special token reader process these
	// symbols to avoid duplicate code.
	case ' ':
	case '\t':
	case '\n':
	case '\r':
	case '\\':	// escape character
	case '@':	// keyword, macro, or comment
	case '$':	// variable name
	case '{':	// start block
	case '}':	// end block
		buf.unget();
		return getspecialtoken();
	default:
		t.type = token_text;
		break;
	}
	if (t.type == token_text)
		imp->buildtoken(t.value, buf, set_text);

	imp->lasttoken = t;
	return t;
}

Token<> Lex::getstricttoken()
{
	Token<> t;
	t.type = token_whitespace;
	Buffer& buf = imp->buf;	// Lazy typist reference

	// strict tokens skip white-spaces
	while ((t.type == token_whitespace) || (t.type == token_comment))
	{
		t = getspecialtoken();
	}

	imp->lasttoken = t;
	return t;
}

Token<> Lex::getspecialtoken()
{
	Token<> t;
	t.type = token_error;
	char c;
	Buffer& buf = imp->buf;	// Lazy typist reference

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
//std::cout << "'" << t.value << "'" << std::endl;
		break;
	case '}':
		t.type = token_closebrace;
		// Ignore all whitespace after close brace
		imp->eatwhitespace(t.value, buf);
//std::cout << "'" << t.value << "'" << std::endl;
		break;
	case '(':
		t.type = token_openparen;
		break;
	case ')':
		t.type = token_closeparen;
		break;
	case ',':
		t.type = token_comma;
		break;
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
		t.type = token_operator;
		break;
	case '|':
	case '&':
	case '^':
		c = imp->safeget(buf);
		if (t.value[0] == c)
		{
			// as in ||, &&, ^^
			t.type = token_operator;
			t.value+= c;
		}
		else if (c) // error, no support for binary ops
			buf.unget();
		break;
	// Handle whitespace
	case ' ':
	case '\t':
		t.type = token_whitespace;
		while ( (c = imp->safeget(buf)) && (c == ' ') || (c == '\t'))
			t.value+= c;
		if (c == '@')
		{
			// check for comment
			c = imp->safeget(buf);
			if (c == '#')
			{
				buf.unget();
				buf.unget();
				t = getspecialtoken();
			}
			else {
				if (c) buf.unget();
				buf.unget();
			}
		}
		else if (c)
			buf.unget();
		break;
	// Handle new lines
	case '\n':
	case '\r':
		t.type = token_whitespace;
		imp->makereturn(c, t.value, buf);
		++imp->lineno;
		break;
	case '"':	// quoted strings
		imp->getstring(t, buf);
		break;
	case '\'':
		imp->getsqstring(t, buf);
		break;
	case '\\':	// escape sequences
		c = imp->safeget(buf);
		if (imp->makereturn(c, t.value, buf))
		{
//std::cout << "Ignore CR" << std::endl;
			++imp->lineno;
			t.type = token_joinline;
			t.value+= token_escape;
		}
		else
		{
			t.type = token_escape;
			t.value = c;
		}
		break;
	case '@':	// keyword or user macro
		c = imp->safeget(buf);
		t.value+= c;
		if (c == set_startvarname)
		{
			imp->buildtoken(t.value, buf, set_varname);
			t.type = imp->checkreserved(&t.value[1]);
		}
		else if (c == '#') // this is a @# comment
			imp->buildcomment(buf, t);
		else
		{
			if (c) buf.unget();
			t.type = token_text;
			return t;
		}
		break;
	case '$':	// variable name
		imp->getclosedidname(t, buf);
		break;
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
		break;
	case '>':
	case '<':
	case '=':
		t.type = token_relop;
		c = imp->safeget(buf);
		if (c == '=')
			t.value+= c;
		else
			if (c) buf.unget();
		break;
	default: t.type = token_error; break;
	}
	// Next, process tokens that are comprised of sets
	// Group digits together
	if (t.type == token_error)
	{
		if (c == set_num)
		{
			t.type = token_integer;
			imp->buildtoken(t.value, buf, set_num);
		}
		else if (c == set_startvarname)
		{
			imp->getidname(t, buf);
		}
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
	std::string cr;
	while ( (c = safeget(buf)) && ((c == ' ') || (c == '\t')))
		value+= c;
	if (makereturn(c, cr, buf))
	{
		value+= cr;
		++lineno;
	}
	// eat a newline
	else if (c)
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
	unsigned depth = 1;
	std::string block;
	Token<> t;

	// Ignore all whitespace before the opening brace;
	do {
		t = getloosetoken();
		block+= t.value;
	} while (t.type == token_whitespace);

	// If the current token is not an open brace, then something is
	// wrong, so abort this function.
	if (t.type != token_openbrace)
		return "";
	else
		block = t.value;

	// Iterate through the buffer until the close brace for this block is found
	do
	{
		t = getloosetoken();
		if (t.type == token_eof)
			break;
		if (t.type == token_escape)
			block+= '\\';
		if (t.type != token_joinline)
			block+= t.value;
		if (t.type == token_openbrace)
			++depth;
		else if (t.type == token_closebrace)
			--depth;
	} while (depth > 0);

	return block;
}

bool Lex::Impl::makereturn(char c, std::string& cr, Buffer& buf)
{
	if (c == '\n')
	{
		cr = c;
		return true;
	}
	else if (c == '\r')
	{
		cr = c;
		c = safeget(buf);
		if (c == '\n') cr+= c;
		else if (c) buf.unget();
		return true;
	}
	return false;
}

bool Lex::Impl::isreturn(const Token<>& t)
{
	if ((t.type == token_whitespace) &&
		((t.value[0] == '\n') ||
		(t.value[0] == '\r')))
		return true;
	return false;
}

void Lex::Impl::buildcomment(Buffer& b, Token<>& t)
{
	char c;
	std::string cr;
	t.type = token_comment;

	while ( (c = safeget(buf)) )
	{
		if (c == '\\')
		{
			// backslash at the end of the line means ignore cr
			c = safeget(buf);
			if (makereturn(c, cr, buf))
			{
				++lineno;
				break;
			}
			else if (c)
				buf.unget();
		}
		else if (makereturn(c, cr, buf))
		{
			// If the last token was a newline, then this is a full
			// line comment and should include the next carriage return
			if (isreturn(lasttoken))
				++lineno;
			else
			{
				buf.unget();
				if (cr.size() > 1)
					buf.unget();
			}
			break;
		}
		t.value+= c;
	}
}

} // end namespace TPTLib
