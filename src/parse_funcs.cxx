/*
 * parse_funcs.cxx
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
#include "ttypes.h"

#include <algorithm>
#include <sstream>
#include <iostream>

#ifdef WIN32
#	include <process.h>		// For Win32 (getpid)
#else
#	include <unistd.h>		// For BSD & Solaris (getpid)
#endif

#include <cstdlib>
#include <ctime>
#include <cctype>

namespace TPTLib {


/*
 * The random number algorithm is the KISS algorithm which I found on
 * the Internet years ago.  I don't know who to give credit to, so if
 * you know, please tell me.  I use it because it's fast and works
 * well.  (Def: KISS = Keep it simple stupid)
 *
 * The idea is to use several different pseudorandom number algorithms
 * to generate a good random looking composite.
 *
 */
void Parser::Impl::srandom32()
{
	// Seeds always start with the time!
	kiss_seed = time(NULL);
	// Then we mix the time with the process id.
	kiss_seed = (kiss_seed << 16) ^ kiss_seed ^ (getpid()<<1);
	// and end by mixing in the address of *this.
	// is this cast okay?
	kiss_seed^= reinterpret_cast<unsigned int>(this);
	kiss_x = kiss_seed | 1;
	kiss_y = kiss_seed | 2;
	kiss_z = kiss_seed | 4;
	kiss_w = kiss_seed | 8;
	kiss_carry = 0;
}

unsigned int Parser::Impl::random32()
{
	if (!isseeded)
	{
		srandom32();
		isseeded = true;
	}

	kiss_x*=69069;	//	kiss_x = kiss_x * 69069 + 1;
	++kiss_x;
	kiss_y ^= kiss_y << 13;
	kiss_y ^= kiss_y >> 17;
	kiss_y ^= kiss_y << 5;
	kiss_k = (kiss_z >> 2) + (kiss_w >> 3) + (kiss_carry >> 2);
	kiss_m = kiss_w + kiss_w + kiss_z + kiss_carry;
	kiss_z = kiss_w;
	kiss_w = kiss_m;
	kiss_carry = kiss_k >> 30;
	return kiss_x + kiss_y + kiss_w;
}


/*
 * Generate a random 32-bit integer token
 *
 */
Token<> Parser::Impl::parse_rand()
{
	ParamList pl;
	Token<> result;
	result.type = token_integer;
	if (getparamlist(pl))
		return result;
	int64_t lwork;

	if (pl.empty())
	{
		// use default parameter
		lwork = 0xFFFFFFFF;
	}
	else
	{
		if (pl.size() > 1)
			recorderror("Warning: @rand takes zero or one arguments");
		lwork = str2num(pl[0]);
	}
	lwork = random32() % lwork;
	num2str(lwork, result.value);

	return result;
}


/*
 * Return a true token (i.e. "1") if parameter token is empty, or false
 * token (i.e. "0") if parameter token is not empty.
 *
 * Note: "" is empty, but "0" is not empty, therefore, only string
 * tokens can be empty.
 *
 */
Token<> Parser::Impl::parse_empty()
{
	ParamList pl;
	Token<> result;
	result.type = token_integer;
	if (getparamlist(pl))
		return result;

	if (pl.empty())
	{
		// use default parameter
		result.value = "1";
	}
	else
	{
		if (pl.size() >= 1)
			recorderror("Warning: @empty takes one argument");
		result.value = (pl[0] == "") ? "1" : "0";
	}
	return result;
}


/*
 * Concatenate a series of string tokens together.  Integer tokens will
 * be treated as strings.
 *
 */
Token<> Parser::Impl::parse_concat()
{
	ParamList pl;
	Token<> result;
	result.type = token_string;
	if (getparamlist(pl))
		return result;

	if (!pl.empty())
	{
		ParamList::const_iterator it(pl.begin()), end(pl.end());
		for (; it != end; ++it)
			result.value+= (*it);
	}

	return result;
}

/*
 * Get length of string.
 *
 */
Token<> Parser::Impl::parse_length()
{
	ParamList pl;
	Token<> result;
	result.type = token_string;
	if (getparamlist(pl))
		return result;

	int64_t lwork=0;

	if (pl.size() != 1)
		recorderror("Warning: @length takes one parameter");
	else
		lwork = pl[0].size();
	num2str(lwork, result.value);
	return result;
}


/*
 * Return a substring of a token.
 *
 * Param1 is string
 * Param2 is start
 * Param3 is end (optional)
 *
 */
Token<> Parser::Impl::parse_substr()
{
	ParamList pl;
	Token<> result;
	result.type = token_string;
	if (getparamlist(pl))
		return result;

	unsigned int start=0, end=std::string::npos;
	if (!pl.empty())
	{
		if (pl.size() == 1)
		{
			recorderror("Warning: @substr takes 2 or 3 parameters");
		}
		else
		{
			start = std::atoi(pl[1].c_str());
			if (pl.size() >= 3)
				end = std::atoi(pl[2].c_str());
			if (pl.size() > 3)
				recorderror("Warning: @substr takes 2 or 3 parameters");
		}
		result.value = pl[0].substr(start,end);
	}

	return result;
}

/*
 * Convert a string to uppercase
 *
 */
Token<> Parser::Impl::parse_uc()
{
	ParamList pl;
	Token<> result;
	result.type = token_string;
	if (getparamlist(pl))
		return result;

	if (!pl.empty())
	{
		result.value = pl[0];
		std::string::iterator it(result.value.begin()),
			end(result.value.end());
		for (; it != end; ++it)
			(*it) = toupper((*it));
	}

	return result;
}


/*
 * Convert a string to lowercase
 *
 */
Token<> Parser::Impl::parse_lc()
{
	ParamList pl;
	Token<> result;
	result.type = token_string;
	if (getparamlist(pl))
		return result;

	if (!pl.empty())
	{
		result.value = pl[0];
		std::string::iterator it(result.value.begin()),
			end(result.value.end());
		for (; it != end; ++it)
			(*it) = tolower((*it));
	}

	return result;
}


/*
 *
 */
Token<> Parser::Impl::parse_size()
{
	ParamList pl;
	Token<> result;
	result.type = token_integer;
	std::string id;
	if (getidparamlist(id, pl))
		return result;

	if (!pl.empty())
		recorderror("Warning: @size takes only one id parameter");

	num2str(symbols.size(id), result.value);

	return result;
}

} // end namespace TPTLib
