/*
 * parse_impl.cxx
 *
 * Parser Implementation
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

#include "parse_impl.h"
#include "symbols_stack.h"

#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPTLib {


void Parser::Impl::parse_macro()
{
	Token<> tok;
	if (level > 0)
	{
		recorderror("Macro may not be defined in sub-block");
		return;
	}
	if (getnextstrict(tok))
		return;
	if (tok.type != token_openparen)
	{
		recorderror("Expected macro declaration");
		return;
	}
	if (getnextstrict(tok))
		return;
	if (tok.type != token_id)
	{
		recorderror("Macro requires name parameter");
		return;
	}
	Macro newmacro;
	std::string name(tok.value);

	tok = lex.getstricttoken();
	while (tok.type == token_comma)
	{
		tok = lex.getstricttoken();
		if (tok.type != token_id)
		{
			recorderror("Syntax error, expected identifier", &tok);
			return;
		}
		newmacro.params.push_back(tok.value);
		tok = lex.getstricttoken();

		if (tok.type == token_closeparen)
			break;
		// The next token should be a comma or a close paren
		if (tok.type != token_comma)
		{
			recorderror("Syntax error, expected comma or close parenthesis", &tok);
			return;
		}
	}
	if (tok.type == token_eof)
	{
		recorderror("Unexpected end of file");
		return;
	}
	else if (tok.type != token_closeparen)
	{
		recorderror("Expected close parenthesis", &tok);
		return;
	}

	newmacro.body = lex.getblock();
	if (newmacro.body.empty())
	{
		recorderror("Expected macro body {}");
		return;
	}
	macros[name] = newmacro;
}


void Parser::Impl::user_macro(const std::string& name, std::ostream* os)
{
	std::string id(name.substr(1));
	ParamList pl;
	if (getparamlist(pl))
		return;

	// The id does not include the prefix @.
	MacroList::const_iterator it(macros.find(id));
	if (it == macros.end())
	{
		recorderror("Undefined macro: " + name);
		return;
	}

	const Macro& mac = (*it).second;
	// While processing parameters, preserve existing symbols
	// if there is a name collision.
	SymbolStack_t symstack;	// preserve scope on global symbols.
	Symbol_t tempsym;
	ParamList::const_iterator pit(mac.params.begin()),
		pend(mac.params.end());
	unsigned i = 0;
	for (; pit != pend; ++pit)
	{
		if (symbols.exists((*pit)))
		{
			tempsym.id = (*pit);
			if (symbols.get((*pit), tempsym.value))
				symstack.push(tempsym);
		}
		if (i >= pl.size())
			symbols.set((*pit), "");
		else
			symbols.set((*pit), pl[i++]);
	}

	// Call the macro
	Buffer newbuf(mac.body.c_str(), mac.body.size()+1);
	Impl imp(newbuf, symbols, macros);
	imp.parse_block(os);

	// Pop saved symbols off the stack
	while (!symstack.empty())
	{
		Symbol_t& ref = symstack.top();
		symbols.set(ref.id, ref.value[0]);
		symstack.pop();
	}
}


} // end namespace TPTLib
