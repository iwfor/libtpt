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
#include <map>

namespace TPTLib {

/**
 *
 * Symbol lookup table
 *
 */
typedef std::map< std::string, std::string > SymbolTable;

/**
 *
 * Error list
 *
 */
typedef std::vector< std::string > ErrorList;

/**
 *
 * The Parser class is responsible
 *
 */
class Parser {
public:
	Parser(Buffer& buf, const SymbolTable* st = 0);	///< ctor
	~Parser();										///< dtor

	std::string run();
	bool run(std::ostream& os);
	bool syntax();
	unsigned geterrorcount();
	bool geterrorlist(ErrorList& errlist);

private:
	struct Impl;
	Impl* imp;

	Parser();
};

} // end namespace TPTLib

#endif __tptlib_parse_h
