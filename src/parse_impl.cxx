/*
 * parse_impl.cxx
 *
 * Parser Implementation
 *
 * $Id$
 *
 */

#include "parse_impl.h"

#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPTLib {

const char* toktypestr(const Token<>& tok);

Parser::Impl::~Impl()
{
}

bool Parser::Impl::pass1(std::ostream* os)
{
	parse_block(os, true);

	return errlist.size() ? true : false;
}


void Parser::Impl::parse_block(std::ostream* os, bool istop)
{
//	int x = rand() % 100;
	if (!istop)
	{
		tok = lex.getstricttoken();
		if (tok.type != token_openbrace)
			recorderror("Expected open brace '{'", &tok);
	}

	do {
		// Read a loosely defined token for outer pass
		tok = lex.getloosetoken();
//std::cout << x << " <" << toktypestr(tok) << "> '" << tok.value << "'" << std::endl;
		switch (tok.type)
		{
		// Close brace (}) is end of block unless istop
		case token_closebrace:
			if (istop)
				*os << tok.value;
			else
				tok.type = token_eof;	// force break out of block
			break;
		// Quit on end of file.
		case token_eof:
			if (!istop)
				recorderror("Unexpected end of file");
			break;
		// Ignore join lines (i.e. backslash (\) last on line)
		case token_joinline:
			break;
		// Just output whitespace and text.
		case token_whitespace:
		case token_text:
			*os << tok.value;
			break;
		// Expand symbols.
		case token_id:
			*os << symbols.get(tok.value);
			break;
		// Process an if statement.
		case token_if:
			parse_if(os);
			break;
		// Include another file.
		case token_include:
			parse_include(os);
			break;
		// Set a variable.
		case token_set:
			parse_set();
			break;
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
		case token_concat:
			tok = parse_concat();
			*os << tok.value;
			break;
		case token_usermacro:
			user_macro(tok.value, os);
			break;
		// Syntax errors should be hard to create at this point.
		default:
			recorderror("Syntax error", &tok);
			break;
		}
	} while (tok.type != token_eof);
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
	// Expect opening parenthesis
	tok = lex.getstricttoken();
	if (tok.type != token_openparen)
	{
		recorderror("Syntax error, expected opening parenthesis");
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
	case token_else:
		return "else";
	case token_empty:
		return "empty";
	case token_rand:
		return "rand";
	case token_concat:
		return "concat";
	default:
		return "undefined";
	}
}


} // end namespace TPTLib
