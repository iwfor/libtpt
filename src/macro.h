/*
 * macro.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_macro_h
#define __tptlib_macro_h

#include <set>
#include <string>
#include <vector>

namespace TPTLib {

// The macro struct defines parameters of a macro
typedef std::vector< std::string > ParamList;

struct Macro {
	std::string name;
	ParamList params;
	std::string body;

	friend bool operator<(const Macro& a, const Macro& b) { return a.name < b.name; }
	friend bool operator==(const Macro& a, const Macro& b) { return a.name == b.name; }
};

typedef std::set< Macro > MacroList;

} // end namespace TPTLib

#endif // __tptlib_macro_h
