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
#include <stack>

namespace TPTLib {

/// Symbol lookup table
typedef std::map< std::string, std::string > SymbolTable;

struct Symbol_t {
	std::string	id;
	std::string	value;
};

typedef std::stack< Symbol_t > SymbolStack_t;

class SymbolMap {
public:
	std::string get(const std::string& id);
	void set(const std::string& id, const std::string& value);
	bool exists(const std::string& id);
	const SymbolTable& operator=(const SymbolTable& symtab);
private:
	SymbolTable symmap;
};

class SymbolStack {
public:
	void push(const std::string& id, const std::string& value);
	void popall(SymbolMap& symmap);
private:
	SymbolStack_t symstack;
};


} // end namespace TPTLib

#endif __tptlib_symbols_h
