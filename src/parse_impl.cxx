/*
 * parse_impl.cxx
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

#include "parse_impl.h"

#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPTLib {

const char* toktypestr(const Token<>& tok);

bool Parser::Impl::pass1(std::ostream* os)
{
	parse_main(os);

	return errlist.size() ? true : false;
}


void Parser::Impl::parse_main(std::ostream* os)
{
	Token<> tok;
	do {
		// Read a loosely defined token for outer pass
		tok = lex.getloosetoken();
		switch (tok.type) {
		case token_eof:
			break;
		default:
			parse_dotoken(os, tok);
			break;
		}
	} while (tok.type != token_eof);
}


void Parser::Impl::ignore_block()
{
	if (lex.getblock().empty())
		recorderror("Error, expected { block }");
}

/*
 * Parse a brace enclosed {} block.  This call is used for macros and
 * if/else statements.
 *
 */
void Parser::Impl::parse_block(std::ostream* os)
{
	Token<> tok(lex.getstricttoken());
//	int x = rand() % 100; // id
	if (tok.type != token_openbrace)
		recorderror("Expected open brace '{'", &tok);

	do {
		// Read a loosely defined token for outer pass
		tok = lex.getloosetoken();
//std::cout << x << " <" << toktypestr(tok) <<
//	"> '" << tok.value << "'" << std::endl;
		switch (tok.type) {
		// Close brace (}) is end of block
		case token_closebrace:
			break;
		case token_eof:
			recorderror("Unexpected end of file");
			break;
		default:
			parse_dotoken(os, tok);
			break;
		}
	} while ( (tok.type != token_eof) &&
		(tok.type != token_closebrace) );
}


/*
 * Add special handling to a loop block to support @next and @last
 * calls.
 *
 * @return	true on end of block or @next;
 * @return	false on @last
 *
 */
bool Parser::Impl::parse_loopblock(std::ostream* os)
{
	Token<> tok(lex.getstricttoken());
	if (tok.type != token_openbrace)
		recorderror("Expected open brace '{'", &tok);

	do {
		// Read a loosely defined token for outer pass
		tok = lex.getloosetoken();
		switch (tok.type) {
		case token_next:
			return true;
		case token_last:
			return false;
		// Close brace (}) is end of block
		case token_closebrace:
			break;
		case token_eof:
			recorderror("Unexpected end of file");
			break;
		default:
			parse_dotoken(os, tok);
			break;
		}
	} while ( (tok.type != token_eof) &&
		(tok.type != token_closebrace) );
	return true;
}


/*
 * Handle any tokens not handled by the calling function
 *
 */
void Parser::Impl::parse_dotoken(std::ostream* os, Token<> tok)
{
//	std::cout << "TOK: <" << toktypestr(tok) << "> " << tok.value << std::endl;
	switch (tok.type)
	{
	// Quit on end of file.
	case token_eof:
		recorderror("Unexpected end of file");
		break;
	// Ignore join lines (i.e. backslash (\) last on line)
	// and comments.
	case token_comment:
	case token_joinline:
		break;
	// Just output whitespace and text.
	case token_whitespace:
	case token_text:
	case token_escape:
		*os << tok.value;
		break;
	// Expand symbols.
	case token_id:
		{
			std::string val;
			if (!symbols.get(tok.value, val))
				*os << val;
		}
		break;
	// Process an if statement.
	case token_if:
		parse_if(os);
		break;
	// loop through an array.
	case token_foreach:
		parse_foreach(os);
		break;
	// loop while expression is true.
	case token_while:
		parse_while(os);
		break;
	// Include another file.
	case token_include:
		parse_include(os);
		break;
	// Set a variable.
	case token_set:
		parse_set();
		break;
	case token_setif:
		parse_setif();
		break;
	case token_unset:
		parse_unset();
		break;
	// Define a macro
	case token_macro:
		parse_macro();
		break;
	// Display a random number.
	case token_rand:
		tok = parse_rand();
		*os << tok.value;
		break;
	// Check if a variable is empty, but why would this be raw.
	case token_empty:
		tok = parse_empty();
		*os << tok.value;
		break;
	// Concatenate some variables, but is this needed raw?
	case token_eval:	// alias for concat
	case token_concat:
		tok = parse_concat();
		*os << tok.value;
		break;
	// get length of string
	case token_length:
		tok = parse_length();
		*os << tok.value;
		break;
	// UpperCase a string
	case token_uc:
		tok = parse_uc();
		*os << tok.value;
		break;
	// LowerCase a string
	case token_lc:
		tok = parse_lc();
		*os << tok.value;
		break;
	// Get size of array variable
	case token_size:
		tok = parse_size();
		*os << tok.value;
		break;
	// Get substring of a string
	case token_substr:
		tok = parse_substr();
		*os << tok.value;
		break;
	// Call a user defined macro
	case token_usermacro:
		user_macro(tok.value, os);
		break;
	// Syntax errors should be hard to create at this point.
	default:
		recorderror("Syntax error", &tok);
		break;
	}
}


/*
 * Get the next strict token.
 * @return	false on success;
 * @return	true on end of file
 *
 */
bool Parser::Impl::getnextstrict(Token<>& target)
{
	target = lex.getstricttoken();
	return (target.type == token_eof);
}

void Parser::Impl::recorderror(const std::string& desc, const Token<>* neartoken)
{
	char buf[32];
	sprintf(buf, "%u", lex.getlineno());
	std::string errstr(desc);
	errstr+= " at line ";
	errstr+= buf;
	if (neartoken)
	{
		errstr+= " near <";
		errstr+= toktypestr(*neartoken);
		errstr+= "> '";
		errstr+= neartoken->value;
		errstr+= "'";
	}
	errlist.push_back(errstr);
}


/*
 * Get a parenthesis enclosed, comma delimeted parameter list.
 *
 * @param	pl			Reference to List to receive parameters.
 * @return	false on success;
 * @return	true on failure
 *
 */
bool Parser::Impl::getparamlist(ParamList& pl)
{
	Token<> tok;
	// Expect opening parenthesis
	tok = lex.getstricttoken();
	if (tok.type != token_openparen)
	{
		recorderror("Syntax error, parameters must be enclosed in "
			"parenthesis", &tok);
		return true;
	}

	// tok holds the current token and nexttok holds the next token
	// returned by the rd parser.
	Token<> nexttok;
	tok = lex.getstricttoken();
	while ((tok.type != token_eof) && (tok.type != token_closeparen))
	{
		nexttok = parse_level0(tok);
		if (tok.type != token_string)
		{
			recorderror("Syntax error, expected expression parameter", &tok);
			return true;
		}
		pl.push_back(tok.value);

		tok = nexttok;
		// The next token should be a comma or a close paren
		if (tok.type == token_closeparen)
			break;
		else if (tok.type != token_comma)
		{
			recorderror("Syntax error, expected comma or close parenthesis", &tok);
			return true;
		}
		tok = lex.getstricttoken();
	}

	return false;
}


/*
 * Get a parenthesis enclosed, comma delimeted id and parameter list.
 *
 * @param	id			Reference to String to receive id.
 * @param	pl			Reference to List to receive parameters.
 * @return	false on success;
 * @return	true on failure
 *
 */
bool Parser::Impl::getidparamlist(std::string& id,  ParamList& pl)
{
	Token<> tok;
	// Expect opening parenthesis
	tok = lex.getstricttoken();
	if (tok.type != token_openparen)
	{
		recorderror("Syntax error, parameters must be enclosed in "
			"parenthesis", &tok);
		return true;
	}

	// tok holds the current token and nexttok holds the next token
	// returned by the rd parser.
	Token<> nexttok;
	tok = lex.getstricttoken();
	if (tok.type != token_id)
	{
		recorderror("Syntax error, expected id", &tok);
		return true;
	}
	id = tok.value;
	tok = lex.getstricttoken();
	if (tok.type == token_comma)
		tok = lex.getstricttoken();

	while ((tok.type != token_eof) && (tok.type != token_closeparen))
	{
		nexttok = parse_level0(tok);
		if (tok.type != token_string)
		{
			recorderror("Syntax error, expected expression parameter", &tok);
			return true;
		}
		pl.push_back(tok.value);

		tok = nexttok;
		// The next token should be a comma or a close paren
		if (tok.type == token_closeparen)
			break;
		else if (tok.type != token_comma)
		{
			recorderror("Syntax error, expected comma or close parenthesis", &tok);
			return true;
		}
		tok = lex.getstricttoken();
	}

	return false;
}


const char* toktypestr(const Token<>& tok)
{
	switch(tok.type) {
	case token_error:
		return "error";
	case token_eof:
		return "eof";
	case token_id:
		return "id";
	case token_usermacro:
		return "usermacro";
	case token_integer:
		return "integer";
	case token_string:
		return "string";
	case token_text:
		return "text";
	case token_comment:
		return "comment";
	case token_whitespace:
		return "whitespace";
	case token_joinline:
		return "joinline";
	case token_escape:
		return "escape";
	case token_openbrace:
		return "openbrace";
	case token_closebrace:
		return "closebrace";
	case token_openparen:
		return "openparen";
	case token_closeparen:
		return "closeparen";
	case token_comma:
		return "comma";
	case token_quote:
		return "quote";
	case token_operator:
		return "operator";
	case token_relop:
		return "relop";
	case token_include:
		return "include";
	case token_set:
		return "set";
	case token_setif:
		return "setif";
	case token_macro:
		return "macro";
	case token_foreach:
		return "foreach";
	case token_while:
		return "while";
	case token_next:
		return "next";
	case token_last:
		return "last";
	case token_if:
		return "if";
	case token_elsif:
		return "elsif";
	case token_else:
		return "else";
	case token_empty:
		return "empty";
	case token_rand:
		return "rand";
	case token_concat:
		return "concat";
	case token_size:
		return "size";
	case token_length:
		return "length";
	case token_lc:
		return "lc";
	case token_uc:
		return "uc";
	default:
		return "undefined";
	}
}


} // end namespace TPTLib
