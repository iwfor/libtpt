/*
 * symbols_stack.cxx
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac*nospam*@tazthecat.net)
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
