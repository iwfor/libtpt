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

#ifdef _MSC_VER
#include <native.h>
#else
#include <sys/types.h>
#endif

#include <cstdlib>

namespace TPTLib {

// TODO: Move this number stuff to a better home
int64_t str2num(const char* str);
// This inline is to help readability
inline int64_t str2num(const std::string& str)
{
	return str2num(str.c_str());
}
void num2str(int64_t value, std::string& str);

// TODO:
// this token format is not compatible with getparamlist.  Fix it.
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
		lwork = 0x8000;
	}
	else
	{
		if (pl.size() == 1)
			recorderror("@rand takes zero or one arguments");
		lwork = str2num(pl[0]);
		lwork = lwork > 0x8000 ? 0x8000 : lwork;
	}
	lwork = std::rand() % lwork;
	num2str(lwork, result.value);

	return result;
}


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
		if (pl.size() == 1)
			recorderror("@empty takes one argument");
		result.value = (pl[0] == "") ? "1" : "0";
	}
	// TODO
	return result;
}


} // end namespace TPTLib
