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
#include <vector>
#include <stack>

namespace TPTLib {

const unsigned maxarraysize = 65536;

typedef std::string SymbolKeyType;
typedef std::string SymbolValueType;
typedef std::vector< SymbolValueType > SymbolArrayType;

class Symbols {
public:
	Symbols();
	Symbols(const Symbols& s);
	~Symbols();

	void set(const SymbolKeyType& id, const SymbolValueType& value);
	void set(const SymbolKeyType& id, const SymbolArrayType& value);
	bool exists(const SymbolKeyType& id) const;
	bool empty(const SymbolKeyType& id) const;
	bool isarray(const SymbolKeyType& id) const;
	unsigned size(const SymbolKeyType& id) const;
	bool get(const SymbolKeyType& id, SymbolValueType& val) const;
	bool get(const SymbolKeyType& id, SymbolArrayType& sym) const;
	void dump();
	Symbols& operator=(const Symbols&);
private:
	struct Impl;
	Impl* imp;
};


} // end namespace TPTLib

#endif // __tptlib_symbols_h
