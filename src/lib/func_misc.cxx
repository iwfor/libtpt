/*
 * func_misc.cxx
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
#include <iostream>

namespace TPT {

/*
 * Reinventing the wheel?  Not all libraries support these functions
 *
 */
int64_t str2num(const char* str)
{
	// Largest representable magnitudes, written without <stdint.h> limit
	// macros: INT64_MAX for a positive result, INT64_MAX + 1 for the
	// most-negative result.
	const int64_t i64max = 9223372036854775807LL;
	const int64_t i64min = -i64max - 1;

	// Accept a single leading sign.
	bool isneg = false;
	if (*str == '-') { isneg = true; ++str; }
	else if (*str == '+') ++str;

	const uint64_t limit = isneg
		? static_cast<uint64_t>(i64max) + 1
		: static_cast<uint64_t>(i64max);

	// Accumulate digits only, stopping at the first non-digit (atoi-like),
	// and saturate rather than overflowing on very long inputs.
	uint64_t value = 0;
	while (*str >= '0' && *str <= '9')
	{
		unsigned digit = static_cast<unsigned>(*str - '0');
		if (value > (limit - digit) / 10)
			return isneg ? i64min : i64max;	// saturate at the limit
		value = value * 10 + digit;
		++str;
	}

	if (isneg)
	{
		if (value == static_cast<uint64_t>(i64max) + 1)
			return i64min;			// exactly INT64_MIN
		return -static_cast<int64_t>(value);
	}
	return static_cast<int64_t>(value);
}


void num2str(int64_t value, std::string& str)
{
	str.erase();
	if (value == 0)
	{
		str = "0";
		return;
	}

	// Convert the magnitude in unsigned arithmetic so that INT64_MIN --
	// whose negation overflows a signed int64_t -- is rendered correctly.
	bool isneg = value < 0;
	uint64_t mag = isneg
		? 0 - static_cast<uint64_t>(value)
		: static_cast<uint64_t>(value);

	char buf[24];	// a 64-bit value is at most 20 decimal digits
	int i = 0;
	while (mag)
	{
		buf[i++] = static_cast<char>(mag % 10) + '0';
		mag /= 10;
	}
	if (isneg)
		str = '-';
	while (i > 0)
		str += buf[--i];
}


} // end namespace TPT
