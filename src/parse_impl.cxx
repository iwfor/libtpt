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

bool Parser::Impl::pass1(std::ostream* os)
{
	do {
		// Read a loosely defined token for outer pass
		tok = lex.getloosetoken();
		switch (tok.type)
		{
		// Quit on end of file.
		case token_eof:
			break;
		// Just output whitespace and text.
		case token_newline:
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
		// Syntax errors should be hard to create at this point.
		default:
			recorderror("Syntax error");
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

void Parser::Impl::parse_macro()
{
	if (level > 0)
	{
		recorderror("Macro may not be defined in sub-block");
		return;
	}
	if (getnextnonwhitespace())
		return;
	if (tok.type != token_openparen)
	{
		recorderror("Expected macro declaration");
		return;
	}
	if (getnextnonwhitespace())
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
bool Parser::Impl::getnextnonwhitespace()
{
	do {
		tok = lex.getstricttoken();
		if (tok.type == token_eof)
			return true;
	} while (tok.type == token_whitespace);
	return false;
}

void Parser::Impl::recorderror(const std::string& desc)
{
	char buf[32];
	sprintf(buf, "%u", lex.getlineno());
	std::string errstr(desc);
	errstr+= " at line ";
	errstr+= buf;
	errlist.push_back(errstr);
}


/*
 * Get a parenthesis enclosed, comma delimeted parameter list.
 *
 * DEV NOTE: For now, just read what is between the parenthesis.  The
 * next version will incorporate a recursive descent parser.
 *
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

	std::string value;
	tok = lex.getstricttoken();
	while (tok.type != token_closeparen)
	{
		if (tok.type == token_comma)
		{
			pl.push_back(value);
			value.erase();
		}
		else
			value+= tok.value;

		tok = lex.getstricttoken();
	}
	if (!value.empty())
		pl.push_back(value);

	return false;
}


} // end namespace TPTLib
