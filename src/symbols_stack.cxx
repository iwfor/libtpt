/*
 * symbols_stack.cxx
 *
 * $Id$
 *
 */

#include "symbols_stack.h"

namespace TPTLib {


/*
 *
 * Push a symbol (id and value) onto the symbol stack.
 *
 * @param	id			ID of the symbol to be pushed.
 * @param	value		Value of the symbol to be pushed.
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void SymbolStack::push(const std::string& id, const std::string& value)
{
	Symbol_t sym;
	sym.id = id;
	sym.value[0] = value;
	symstack.push(sym);
}


/*
 *
 * @param	symmap		Symbol Map to receive symbols.
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void SymbolStack::popall(Symbols& symmap)
{
	while (!symstack.empty())
	{
		Symbol_t& sym = symstack.top();
		symstack.pop();
		symmap.set(sym.id, sym.value);
	}
}

} // end namespace TPTLib
