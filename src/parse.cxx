/*
 * parse.cxx
 *
 * Parser
 *
 * $Id$
 *
 */


#include "lexical.h"
#include <tptlib/buffer.h>

#include <iosfwd>

namespace TPTLib {

/**
 *
 * The Parser class is responsible
 *
 */
class Parser {
public:
	Parser(Buffer& buf);					///< ctor
	Parser(Buffer& buf, std::ostream& os);	///< ctor
	~Parser();								///< dtor

	std::string run();

private:
	struct Impl;
	Impl* imp;

	Parser();
};

struct Parser::Impl {
	std::ostream* os;
	bool usingstream;
	Lex lex;

	Impl(Buffer& buf) : lex(buf), usingstream(false) {}
	Impl(Buffer& buf, std::ostream* s) : lex(buf), os(s), usingstream(true) {}
};

Parser::Parser(Buffer& buf)
{
	imp = new Impl(buf);
}

Parser::Parser(Buffer& buf, std::ostream& os)
{
	imp = new Impl(buf, &os);
}

Parser::~Parser()
{
	delete imp;
}


} // end namespace TPTLib
