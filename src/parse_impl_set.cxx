/*
 * parse_impl_set.cxx
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

#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPTLib {

void Parser::Impl::parse_unset()
{
	std::string id;
	ParamList pl;

	if (getidparamlist(id, pl))
		return;

	if (!pl.empty())
		recorderror("Warning: @unset takes only an id parameter");

	symbols.unset(id);
}

void Parser::Impl::parse_set()
{
	std::string id;
	ParamList pl;

	if (getidparamlist(id, pl))
		return;

	if (pl.empty())
		symbols.set(id, "");
	else
	{
		if (pl.size() > 1)
		{
			// this is an array
			symbols.set(id, pl);
		}
		else
			symbols.set(id, pl[0]);
	}
}


/*
 * Only set the variable if it is empty.
 *
 */
void Parser::Impl::parse_setif()
{
	std::string id;
	ParamList pl;

	if (getidparamlist(id, pl))
		return;

	if (pl.empty())
	{
		if (!symbols.empty(id))
			symbols.set(id, "");
	}
	else
	{
		if (pl.size() > 1)
		{
			// this is an array
			if (!symbols.empty(id))
				symbols.set(id, pl);
		}
		else
		{
			if (!symbols.empty(id))
				symbols.set(id, pl[0]);
		}
	}
}

} // end namespace TPTLib
