/*
 * lex_impl.cxx
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

#include "lex_impl.h"

#include <cstring>
#include <iostream>


namespace TPTLib {


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
		if (!std::strcmp(str, "unset"))		return token_unset;
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
		if (c) safeunget(buf);
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
			safeunget(buf);
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
			safeunget(buf);
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
			safeunget(buf);
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
			safeunget(buf);
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
			safeunget(buf);
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
			safeunget(buf);
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
		newline();
	}
	// eat a newline
	else if (c)
		safeunget(buf);
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
		else if (c) safeunget(buf);
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
				newline();
				break;
			}
			else if (c)
				safeunget(buf);
		}
		else if (makereturn(c, cr, buf))
		{
			// If the last token was a newline, then this is a full
			// line comment and should include the next carriage return
			if (t.column == 1)
			{
				newline();
			}
			else
			{
				safeunget(buf);
				if (cr.size() > 1)
					safeunget(buf);
			}
			break;
		}
		t.value+= c;
	}
}


void Lex::Impl::newline()
{
	column = 1;
	++lineno;
}

} // end namespace TPTLib
