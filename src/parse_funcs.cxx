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
Token<> Parser::Impl::parse_rand(Token<>& left)
{
	ParamList pl;
	if (getparamlist(pl))
		return left;
	int64_t lwork;

	if (pl.empty())
	{
		// use default parameter
		lwork = 0x7fff;
	}
	else
	{
		if (pl.size() == 1)
			recorderror("@rand takes zero or one arguments");
		lwork = str2num(pl[0]);
	}
	lwork = std::rand() % lwork;
	num2str(lwork, left.value);

	return lex.getstricttoken();
}


Token<> Parser::Impl::parse_empty(Token<>& left)
{
	// TODO
	return left;
}


} // end namespace TPTLib
