/*
 * parse_impl_if.cxx
 *
 * Parse If Implementation
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

#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPTLib {


/*
 *
 */
bool Parser::Impl::parse_ifexpr(std::ostream* os)
{
	ParamList pl;

	if (getparamlist(pl))
		return false;

	if (pl.empty())
	{
		recorderror("Syntax error, @if requires an expression");
		return false;
	}
	else if (pl.size() > 1)
		recorderror("Warning: extra parameters in @if ignored");

	int64_t lwork;
	Token<> tok;
	lwork = str2num(pl[0]);	// lwork = result of if expression

	if (lwork)	// if true, then TPT if was true
		parse_block(os);
	else
	{
		std::string ignore;
		std::stringstream ignorestr(ignore);
		parse_block(&ignorestr);
	}

	return !!lwork;
}

void Parser::Impl::parse_if(std::ostream* os)
{
	Token<> tok;

	std::string ignore;
	std::stringstream ignorestr(ignore);

	bool done;
	done = parse_ifexpr(os);
	tok = lex.getloosetoken();

	while (tok.type == token_elsif)
	{
		if (done)
			parse_ifexpr(&ignorestr);
		else
			done = parse_ifexpr(os);
		tok = lex.getloosetoken();
	}

	if (tok.type == token_else)
	{
		if (done)
			parse_block(&ignorestr);
		else
			parse_block(os);
	}
	else
	{
		// unget token
		lex.unget(tok);
	}
}

} // end namespace TPTLib
