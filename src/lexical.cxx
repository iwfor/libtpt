/*
 * lexical.cxx
 *
 * Lexical Analyzer
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

/*
 * The lexical analyzer is responsible for converting input
 * text to tokens.
 *
 * Using the ChrSet<> and Token<> templates, it should be easy to
 * upgrade the lexical analyzer to handle wchar_t (wide character) types
 * as well as the basic char type.
 *
 */

#ifdef _MSC_VER
#	pragma	warning(disable:4786)
#endif

#include "lexical.h"
#include "lex_impl.h"
#include "tptlib/buffer.h"

#include <cstring>
#include <iostream>

namespace TPTLib {


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
	char c;
	Buffer& buf = imp->buf;	// Lazy typist reference

	if (!(c = imp->safeget(buf)))
	{
		Token<> t;
		t.column = imp->column;
		t.type = token_eof;	
		return t;
	}

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
		imp->safeunget(buf);
		return getspecialtoken();
	default:
		break;
	}
	Token<> t;
	t.type = token_text;
	t.value = c;
	t.column = imp->column;
	imp->buildtoken(t.value, buf, set_rawtext);

	return t;
}


Token<> Lex::getstricttoken()
{
	Token<> t;
	t.type = token_whitespace;

	// strict tokens skip white-spaces
	while ((t.type == token_whitespace) || (t.type == token_comment))
	{
		t = getspecialtoken();
	}

	return t;
}


Token<> Lex::getspecialtoken()
{
	Token<> t;
	t.type = token_error;
	t.column = imp->column;
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
			imp->safeunget(buf);
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
				imp->safeunget(buf);
				imp->safeunget(buf);
				t = getspecialtoken();
			}
			else {
				if (c) imp->safeunget(buf);
				imp->safeunget(buf);
			}
		}
		else if (c)
			imp->safeunget(buf);
		break;
	// Handle new lines
	case '\n':
	case '\r':
		t.type = token_whitespace;
		imp->makereturn(c, t.value, buf);
		imp->newline();
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
			imp->newline();
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
			if (c) imp->safeunget(buf);
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
			if (c) imp->safeunget(buf);
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
			if (c) imp->safeunget(buf);
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
		imp->safeunget(imp->buf);
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
		// Preserve escaped sequences
		if (t.type == token_escape)
			block+= '\\';
		// Go ahead and nuke joined lines
		if (t.type == token_joinline)
			continue;
		block+= t.value;
		if (t.type == token_openbrace)
			++depth;
		else if (t.type == token_closebrace)
			--depth;
	} while (depth > 0);

	return block;
}


} // end namespace TPTLib
