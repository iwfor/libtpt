/*
 * symbols_impl.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_symbol_impl_h
#define __tptlib_symbol_impl_h

#include <tptlib/symbols.h>

namespace TPTLib {

typedef SymbolArrayType SymbolValue;

struct Symbol_t {
	std::string	id;
	SymbolValue value;
};

typedef std::map< SymbolKeyType, SymbolValue > SymbolTable;


/*
 * The private implementation of Symbols.
 *
 */
struct Symbols::Impl {
public:
	SymbolTable symmap;

	Impl() {};
	Impl(const SymbolTable& sm) : symmap(sm) {}
	~Impl() {};

	bool getrealid(const SymbolKeyType& id, SymbolKeyType& realkey,
		unsigned& index);
};



} // end namespace TPTLib

#endif // __tptlib_symbol_impl_h
