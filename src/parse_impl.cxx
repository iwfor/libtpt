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

bool Parser::Impl::pass1(std::ostream* os)
{
//	symbols.dump();
	do {
		// Read a loosely defined token for outer pass
		tok = lex.getloosetoken();
		switch (tok.type)
		{
		// Quit on end of file.
		case token_eof:
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
		// Break loop and return if this is the end of a section.
		case token_closebrace:
			if (level)	// if in a block, this is the end of it
				return false;
			else		// else, on top level, so print close brace
				*os << tok.value;
			break;
		// Process an if statement.
		case token_if:
//			parse_if(os);
			break;
		// Include another file.
		case token_include:
			parse_include(os);
			break;
		// Set a variable
		case token_set:
			parse_set(os);
			break;
		// Syntax errors should be hard to create at this point.
		default:
			recorderror("Syntax error", &tok);
			break;
		}
	} while (tok.type != token_eof);

	return errlist.size() ? true : false;
}


void Parser::Impl::parse_include(std::ostream* os)
{
	ParamList pl;
	if (getparamlist(pl))
		return;

	if (pl.size() != 1)
	{
		recorderror("Include takes exactly 1 parameter");
		return;
	}
	// Create another Impl which inherits symbols table
	// to process include
	Buffer buf(pl[0].c_str());
	Impl incl(buf, symbols);
	incl.pass1(os);
	// copy incl's error list, if any
	ErrorList::iterator it(incl.errlist.begin()), end(incl.errlist.end());
	for (; it != end; ++it)
		errlist.push_back(*it);
}

void Parser::Impl::parse_set(std::ostream* os)
{
	// Set has to be manually parsed since the first parameter is the
	// identifier to be set.
	tok = lex.getstricttoken();
	if (tok.type != token_openparen)
	{
		recorderror("Syntax error, expected open parenthesis", &tok);
		return;
	}

	// tok holds the current token and nexttok holds the token returned
	// by the parser.
	tok = lex.getstricttoken();
	if (tok.type != token_id)
	{
		recorderror("Syntax error, first parameter must be ID", &tok);
		return;
	}
	std::string id(tok.value);

	tok = lex.getstricttoken();
	// If this is a close parenthesis, then just clear the symbol
	if (tok.type == token_closeparen)
	{
		symbols.set(id, "");
		return;
	}
	if (tok.type != token_comma)
	{
		recorderror("Syntax error, expected comma (,)", &tok);
		return;
	}
	tok = lex.getstricttoken();
	// If this is a close parenthesis, then just clear the symbol
	if (tok.type == token_closeparen)
	{
		symbols.set(id, "");
		return;
	}
	// Parse the expression
	Token<> nexttok = parse_level0(tok);
	symbols.set(id, tok.value);
	tok = nexttok;
	if (tok.type != token_closeparen)
		recorderror("Syntax error, expected close parenthesis", &tok);
}

void Parser::Impl::parse_macro()
{
	if (level > 0)
	{
		recorderror("Macro may not be defined in sub-block");
		return;
	}
	if (getnextstrict(tok))
		return;
	if (tok.type != token_openparen)
	{
		recorderror("Expected macro declaration");
		return;
	}
	if (getnextstrict(tok))
		return;
	if (tok.type != token_id)
	{
		recorderror("Macro requires name parameter");
		return;
	}
	// TODO: Accept parameter list
}

/*
 * Skip all whitespaces and get the next strict token.
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
		errstr+= ">'";
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
		recorderror("Expected opening parenthesis");
		return true;
	}

	// tok holds the current token and nexttok holds the token returned
	// by the parser.
	Token<> nexttok;
	tok = lex.getstricttoken();
	while (tok.type != token_eof)
	{
		while (tok.type == token_comma)
		{
			// this is an empty parameter
			pl.push_back("");
		}
		nexttok = parse_level0(tok);
		pl.push_back(tok.value);

		// The next token should be a comma or a close paren
		tok = nexttok;
		if (tok.type == token_comma)
		{
			tok = lex.getstricttoken();
			nexttok = parse_level0(tok);
		}
		else if (tok.type == token_closeparen)
			break;
		else
			recorderror("Syntax error", &tok);
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
