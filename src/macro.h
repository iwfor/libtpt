/*
 * macro.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_macro_h
#define __tptlib_macro_h

#include <map>
#include <string>
#include <vector>

namespace TPTLib {

// The macro struct defines parameters of a macro
typedef std::vector< std::string > ParamList;

struct Macro {
	ParamList params;
	std::string body;
};

typedef std::map< std::string, Macro > MacroList;

} // end namespace TPTLib

#endif // __tptlib_macro_h
