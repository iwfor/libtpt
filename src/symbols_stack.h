/*
 * symbols_stack.h
 *
 * $Id$
 *
 */

#ifndef __tptlib_symbol_stack_h
#define __tptlib_symbol_stack_h

#include "symbols_impl.h"

#include <stack>

namespace TPTLib {

typedef std::stack< Symbol_t > SymbolStack_t;

class SymbolStack {
public:
	void push(const std::string& id, const SymbolValueType& value);
	void popall(Symbols& symmap);
private:
	SymbolStack_t symstack;
};

} // end namespace TPTLib

#endif // __tptlib_symbol_stack_h
