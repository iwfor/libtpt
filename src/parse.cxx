/*
 * parse.cxx
 *
 * Parser
 *
 * $Id$
 *
 */


#include <tptlib/parse.h>
#include "lexical.h"

#include <algorithm>
#include <strstream>

namespace TPTLib {

struct Parser::Impl {
	Lex lex;
	unsigned lineno;
	unsigned level;	// if/loop level
	SymbolTable symbols;
	ErrorList errlist;

	Impl(Buffer& buf, const SymbolTable* st) : lex(buf), lineno(1), level(0)
	{ if (st) symbols = *st; }
	std::string get_symbol(const std::string& id);

	bool pass1(std::ostream* os);
	void pass_if(std::ostream* os);

};

bool Parser::Impl::pass1(std::ostream* os)
{
	Token<> tok;

	do {
		tok = lex.getloosetoken();
		switch (tok.type)
		{
		case token_eof:
			break;
		case token_newline:
			++lineno;
			// no break; let output
		case token_whitespace:
		case token_text:
			*os << tok.value;
			break;
		case token_id:
			*os << get_symbol(tok.value);
		case token_closebrace:
			if (level)	// if in a block, this is the end of it
				return false;
			else		// else, on top level, so print close brace
				*os << tok.value;
			break;
		case token_if:
			pass_if(os);
		default:
			// error
			{
				char buf[32];
				sprintf(buf, "%u", lineno);
				std::string errstr("Syntax error on line ");
				errstr+= buf;
				errlist.push_back(errstr);
			}
		}
	} while (tok.type != token_eof);

	return errlist.size() ? true : false;
}

std::string Parser::Impl::get_symbol(const std::string& id)
{
	// If this is an enclosed id, strip ${};
	if (id[0] == '$')
		return get_symbol(id.substr(2, id.size()-3));
	// For now, don't handle embedded IDs.
	return symbols[id];
	// TODO: When an id contains an embedded ${id}, recurse and build
	// new id name.
	if (id.find('$'))	// need to recursively parse this id
	{
	}
}

Parser::Parser(Buffer& buf, const SymbolTable* st)
{
	imp = new Impl(buf, st);
}

Parser::~Parser()
{
	delete imp;
}

std::string Parser::run()
{
	std::strstream ss;
	run(ss);
	return ss.str();
}

bool Parser::run(std::ostream& os)
{
	imp->errlist.clear();
	return imp->pass1(&os);
}

bool Parser::syntax()
{
	return imp->pass1(0);
}

bool Parser::geterrorlist(std::vector< std::string >& errlist)
{
	if (imp->errlist.size())
	{
		errlist.clear();
		std::copy(imp->errlist.begin(), imp->errlist.end(), errlist.begin());
		return true;
	}
	else
		return false;
}


} // end namespace TPTLib
