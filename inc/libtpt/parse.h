/*
 * parse.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_parse_h
#define __tptlib_parse_h

#include <tptlib/buffer.h>
#include <tptlib/symbols.h>
#include <iosfwd>
#include <string>
#include <vector>

namespace TPTLib {

/// Error list
typedef std::vector< std::string > ErrorList;

/// The Parser class
class Parser {
public:
	Parser(Buffer& buf, const Symbols* st = 0);	///< ctor
	~Parser();										///< dtor

	std::string run();
	bool run(std::ostream& os);
	bool syntax();
	unsigned geterrorcount() const;
	bool geterrorlist(ErrorList& errlist);

private:
	struct Impl;
	Impl* imp;

	Parser();
};

} // end namespace TPTLib

#endif // __tptlib_parse_h
