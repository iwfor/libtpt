/*
 * symbols.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_symbols_h
#define __tptlib_symbols_h

#include <string>
#include <map>

namespace TPTLib {

/// Symbol lookup table
typedef std::map< std::string, std::string > SymbolTable;

/// Get the specified symbol from the specified symbol table.
std::string get_symbol(const std::string& id, const SymbolTable& symtab);

} // end namespace TPTLib

#endif __tptlib_symbols_h
