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

	Impl(Buffer& buf) : lex(buf) {}
	std::vector< std::string > errlist;
	bool pass1(std::ostream* os);
};

Parser::Parser(Buffer& buf)
{
	imp = new Impl(buf);
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
