/*
 * mathtest.cxx
 *
 * Security tests for arithmetic: division/modulo by zero and @rand must not
 * crash the process (SIGFPE), and normal arithmetic must be unaffected.
 *
 * Copyright (C) 2002-2010 Isaac W. Foraker (isaac at noscience dot net)
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

#include <libtpt/parse.h>
#include <libtpt/symbols.h>

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

namespace {

std::string render(const char* tpl, unsigned& errcount)
{
	TPT::Symbols sym;
	TPT::Parser p(tpl, static_cast<unsigned long>(std::strlen(tpl)), sym);
	std::stringstream out;
	p.run(out);
	errcount = p.geterrorcount();
	return out.str();
}

} // end anonymous namespace


bool test_divide_by_zero();
bool test_modulo_by_zero();
bool test_int64min_over_neg1();
bool test_rand_zero();
bool test_normal_arithmetic();

int main()
{
	bool result = false, r;

	struct { const char* name; bool (*fn)(); } tests[] = {
		{ "divide by zero is safe",      test_divide_by_zero },
		{ "modulo by zero is safe",      test_modulo_by_zero },
		{ "int64min / -1 is safe",       test_int64min_over_neg1 },
		{ "@rand(0) is safe",            test_rand_zero },
		{ "normal arithmetic unchanged", test_normal_arithmetic },
		{ 0, 0 }
	};
	for (int i = 0; tests[i].fn; ++i)
	{
		r = tests[i].fn();
		if (r)
			std::cout << "math test " << (i + 1) << " ("
				<< tests[i].name << "): failed" << std::endl;
		result |= r;
	}

	std::cout << (result ? "FAILED" : "PASSED") << std::endl;
	return result;
}

// Reaching the assertion (rather than dying from SIGFPE) already proves the
// crash is gone; the guard must also record an error.
bool test_divide_by_zero()
{
	unsigned errs = 0;
	render("@set(x, 1 / 0)${x}", errs);
	return (errs == 0);
}

bool test_modulo_by_zero()
{
	unsigned errs = 0;
	render("@set(x, 5 % 0)${x}", errs);
	return (errs == 0);
}

// int64min / -1 overflows and traps on x86 just like divide-by-zero.  The
// only requirement here is that the process survives.
bool test_int64min_over_neg1()
{
	unsigned errs = 0;
	render("@set(x, -9223372036854775808 / -1)${x}", errs);
	return false;	// survival is the assertion; a SIGFPE would abort main
}

bool test_rand_zero()
{
	unsigned errs = 0;
	render("@rand(0)", errs);
	return (errs == 0);
}

// Ordinary arithmetic must still evaluate correctly.
bool test_normal_arithmetic()
{
	unsigned errs = 0;
	std::string div = render("@set(x, 64 * (2 + 3) / -16)${x}", errs);
	if ((div != "-20") || (errs != 0))
		return true;
	errs = 0;
	std::string mod = render("@set(x, 17 % 5)${x}", errs);
	return (mod != "2") || (errs != 0);
}
