/*
 * parse.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_parse_h
#define __tptlib_parse_h

#include <tptlib/buffer.h>
#include <iosfwd>
#include <string>
#include <vector>

namespace TPTLib {

/**
 *
 * The Parser class is responsible
 *
 */
class Parser {
public:
	Parser(Buffer& buf);					///< ctor
	~Parser();								///< dtor

	std::string run();
	bool run(std::ostream& os);
	bool syntax();
	bool geterrorlist(std::vector< std::string >& errlist);

private:
	struct Impl;
	Impl* imp;

	Parser();
};

} // end namespace TPTLib

#endif __tptlib_parse_h
