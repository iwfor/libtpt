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
		// Break loop and return if this is the end of a section.
		case token_closebrace:
			if (level)	// if in a block, this is the end of it
				return false;
			else		// else, on top level, so print close brace
				*os << tok.value;
			break;
		// Process an if statement.
		case token_if:
			parse_if(os);
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
	Buffer buf(pl[0].c_str());
	Parser p(buf, symbols);
	p.run(*os);
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


} // end namespace TPTLib
