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
#include "macro.h"

#include <algorithm>
#include <sstream>

namespace TPTLib {

struct Parser::Impl {
	Lex lex;
	unsigned level;	// if/loop level
	SymbolTable symbols;
	ErrorList errlist;
	Token<> tok;
	MacroList macros;

	Impl(Buffer& buf, const SymbolTable* st) : lex(buf), level(0)
	{ if (st) symbols = *st; }
	void recorderror(const std::string& desc);
	bool getnextparam(std::string& value);
	bool getnextnonwhitespace();
	bool getparamlist(ParamList& pl);

	bool pass1(std::ostream* os);
	void parse_include(std::ostream* os);
	void parse_if(std::ostream* os);
	void parse_macro();
	void parse_set();
	void parse_empty(std::ostream* os);
	void parse_rand(std::ostream* os);

	void addmacro(const std::string& name, const ParamList& params,
		const std::string& body);
	void execmacro(const std::string& name, const ParamList& params);
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
		case token_whitespace:
		case token_text:
			*os << tok.value;
			break;
		case token_id:
			*os << get_symbol(tok.value, symbols);
		case token_closebrace:
			if (level)	// if in a block, this is the end of it
				return false;
			else		// else, on top level, so print close brace
				*os << tok.value;
			break;
		case token_if:
			parse_if(os);
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
	std::stringstream ss;
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
