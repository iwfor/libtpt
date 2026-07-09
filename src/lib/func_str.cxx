/*
 * func_str.cxx
 *
 * Copyright (C) 2002-2009 Isaac W. Foraker (isaac at noscience dot net)
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
 */

#include "funcs.h"
#include <libtpt/object.h>
#include <cstring>
#include <cstdlib>

namespace TPT {

namespace {
	// Upper bound on characters a single string builtin may generate, to
	// keep a hostile width/count from exhausting memory.
	const unsigned long MAX_GENERATED = 1000000UL;
}

/*
 * Concatenate a series of string together.  Integer tokens will be
 * treated as strings.
 *
 */
bool func_concat(std::ostream& os, TPT::Object& params)
{
	Object::ArrayType& pl = params.array();
	bool iserr = false;

	if (!pl.empty())
	{
		Object::ArrayType::const_iterator it(pl.begin()), end(pl.end());
		for (; it != end; ++it)
		{	   
			Object& obj = *(*it).get();
			if (obj.gettype() == Object::type_scalar)
				os << obj.scalar();
			else
				iserr = true;
		}	   
	}

	return iserr;
}


/*
 * Get length of string.
 *
 */
bool func_length(std::ostream& os, Object& params)
{
	Object::ArrayType& pl = params.array();
	bool iserr = false;
	int64_t lwork=0;
	std::string result;

	if (pl.size() != 1)
		iserr = true;
	else
	{   
		Object& obj = *pl[0].get();
		if (obj.gettype() == Object::type_scalar)
			lwork = obj.scalar().size();
		else
			iserr = true;
	}
	num2str(lwork, result);
	os << result;
	return iserr;
}

/*
 * Return a substring of a token.
 *
 * Param1 is string
 * Param2 is start
 * Param3 is end (optional)
 *
 */
bool func_substr(std::ostream& os, Object& params)
{
	Object::ArrayType& pl = params.array();
	std::size_t start = 0, len = std::string::npos;
	bool iserr = false;

	if (!pl.empty())
	{
		if (pl.size() == 1)
			return true;
		else
		{
			Object& obj1 = *pl[1].get();
			if (obj1.gettype() != Object::type_scalar)
				return true;
			// Clamp negatives to 0 so a negative index does not wrap to a
			// huge size_t (which would make substr() throw).
			long startval = std::atoi(obj1.scalar().c_str());
			start = (startval < 0) ? 0 : static_cast<std::size_t>(startval);
			if (pl.size() >= 3)
			{
				Object& obj2 = *pl[2].get();
				if (obj2.gettype() != Object::type_scalar)
					return true;
				long lenval = std::atoi(obj2.scalar().c_str());
				len = (lenval < 0) ? 0 : static_cast<std::size_t>(lenval);
			}
			if (pl.size() > 3)
				iserr = true;
		}
		Object& obj = *pl[0].get();
		if (obj.gettype() != Object::type_scalar)
			return true;
		// A start past the end yields an empty string rather than throwing.
		const std::string& s = obj.scalar();
		if (start > s.size())
			start = s.size();
		os << s.substr(start, len);
	}
	else
		return true;

	return iserr;
}


/*
 * Convert a string to uppercase
 *
 */
bool func_uc(std::ostream& os, Object& params)
{
	Object::ArrayType& pl = params.array();
	bool iserr = false;

	if (pl.empty())
		return true;
	if (pl.size() > 1)
		iserr = true;
	Object& obj = *pl[0].get();
	if (obj.gettype() != Object::type_scalar)
		return true;
	std::string result(obj.scalar());
	std::string::iterator it(result.begin()),
		end(result.end());
	for (; it != end; ++it)
		(*it) = std::toupper((*it));
	
	os << result;
	return iserr;
}


/*
 * Convert a string to lowercase
 *
 */
bool func_lc(std::ostream& os, Object& params)
{
	Object::ArrayType& pl = params.array();
	bool iserr = false;

	if (pl.empty())
		return true;
	if (pl.size() > 1)
		iserr = true;
	Object& obj = *pl[0].get();
	if (obj.gettype() != Object::type_scalar)
		return true;
	std::string result(obj.scalar());
	std::string::iterator it(result.begin()),
		end(result.end());
	for (; it != end; ++it)
		(*it) = std::tolower((*it));
	
	os << result;
	return iserr;
}


/*
 * Pad a string on the left with spaces to fit specified width.
 */
bool func_lpad(std::ostream& os, Object& params)
{
	Object::ArrayType& pl = params.array();

	if (pl.empty() || pl.size() != 2)
		return true;
	Object& strobj = *pl[0].get(),
		numobj = *pl[1].get();
	if (strobj.gettype() != Object::type_scalar ||
			numobj.gettype() != Object::type_scalar)
		return true;
	std::string& str = strobj.scalar();
	long widthval = std::atoi(numobj.scalar().c_str());
	unsigned long width = (widthval < 0) ? 0 : static_cast<unsigned long>(widthval);
	if (width > MAX_GENERATED)
		return true;	// refuse an absurd pad width
	if (str.length() >= width)
	{
		os << str;
		return false;
	}
	std::string spaces(width - str.length(), ' ');
	os << spaces << str;
	return false;
}

/*
 * Pad a string on the right with spaces to fit specified width.
 */
bool func_rpad(std::ostream& os, Object& params)
{
	Object::ArrayType& pl = params.array();

	if (pl.empty() || pl.size() != 2)
		return true;
	Object& strobj = *pl[0].get(),
		numobj = *pl[1].get();
	if (strobj.gettype() != Object::type_scalar ||
			numobj.gettype() != Object::type_scalar)
		return true;
	std::string& str = strobj.scalar();
	long widthval = std::atoi(numobj.scalar().c_str());
	unsigned long width = (widthval < 0) ? 0 : static_cast<unsigned long>(widthval);
	if (width > MAX_GENERATED)
		return true;	// refuse an absurd pad width
	if (str.length() >= width)
	{
		os << str;
		return false;
	}
	std::string spaces(width - str.length(), ' ');
	os << str << spaces;
	return false;
}

/*
 * Repeat the specified text n times.
 */
bool func_repeat(std::ostream& os, Object& params)
{
	Object::ArrayType& pl = params.array();

	if (pl.empty() || pl.size() != 2)
		return true;
	Object& strobj = *pl[0].get(),
		numobj = *pl[1].get();
	if (strobj.gettype() != Object::type_scalar ||
			numobj.gettype() != Object::type_scalar)
		return true;
	std::string& str = strobj.scalar();
	int n = std::atoi(numobj.scalar().c_str());
	if (n < 0)
		n = 0;
	// Refuse a count that would generate an unbounded amount of output.
	if (str.length() != 0 &&
			static_cast<unsigned long>(n) > MAX_GENERATED / str.length())
		return true;
	while (n-- > 0)
		os << str;
	return false;
}



} // end namespacr TPT
