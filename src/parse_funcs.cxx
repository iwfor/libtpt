/*
 * parse_funcs.cxx
 *
 * $Id$
 *
 */

#include "parse_impl.h"

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
		lwork = 0x8000000;
	}
	else
	{
		if (pl.size() > 1)
			recorderror("@rand takes zero or one arguments");
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
			recorderror("@empty takes one argument");
		result.value = (pl[0] == "") ? "1" : "0";
	}
	return result;
}


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

} // end namespace TPTLib
