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

typedef std::vector< std::string > ParamList;

struct Macro {
	std::string name;
	ParamList params;
};

struct Parser::Impl {
	Lex lex;
	unsigned lineno;
	unsigned level;	// if/loop level
	SymbolTable symbols;
	ErrorList errlist;
	Token<> tok;

	Impl(Buffer& buf, const SymbolTable* st) : lex(buf), lineno(1), level(0)
	{ if (st) symbols = *st; }
	std::string get_symbol(const std::string& id);
	void recorderror(const std::string& desc);
	bool getnextparam(std::string& value);
	bool getnextnonwhitespace();

	bool pass1(std::ostream* os);
	void pass_include(std::ostream* os);
	void pass_if(std::ostream* os);
	void pass_macro();
	void pass_set();
	void pass_while(std::ostream* os);
	void pass_foreach(std::ostream* os);
	void pass_concat(std::ostream* os);
	void pass_empty(std::ostream* os);
	void pass_rand(std::ostream* os);
};

bool Parser::Impl::pass1(std::ostream* os)
{
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
			recorderror("Syntax error");
			break;
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
	// TEST
	// TEST
	// TEST
	// TEST
	// When an id contains an embedded ${id}, recurse and build new id
	// name.
	if (id.find('$'))	// need to recursively parse this id
	{
		Buffer buf(id.c_str(), id.size());
		Parser p(buf);
		std::string newid(p.run());
		return get_symbol(newid);
	}
	else
		return symbols[id];
}

void Parser::Impl::pass_macro()
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
	sprintf(buf, "%u", lineno);
	std::string errstr(desc);
	errstr+= " at line ";
	errstr+= buf;
	errlist.push_back(errstr);
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

unsigned Parser::geterrorcount()
{
	return imp->errlist.size();
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
