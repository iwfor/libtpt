/*
 * macrotest.cxx
 *
 * Security test for the @macro recursion-depth guard.
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


bool test_infinite_recursion_is_bounded();
bool test_normal_macro_still_works();

int main()
{
	bool result = false, r;

	r = test_infinite_recursion_is_bounded();
	if (r)
		std::cout << "macro test 1 (infinite recursion bounded): failed"
			<< std::endl;
	result |= r;

	r = test_normal_macro_still_works();
	if (r)
		std::cout << "macro test 2 (normal macro works): failed"
			<< std::endl;
	result |= r;

	std::cout << (result ? "FAILED" : "PASSED") << std::endl;
	return result;
}

/*
 * A self-referential macro must be stopped by the depth guard rather than
 * recursing until the native stack overflows.  Reaching this assertion at all
 * means we returned instead of crashing; the guard must also record an error
 * (which is now propagated out of the macro expansion).
 */
bool test_infinite_recursion_is_bounded()
{
	unsigned errs = 0;
	render("@macro(boom) { @boom() }@boom()", errs);
	return (errs == 0);
}

/*
 * An ordinary (non-recursive) macro must still expand normally and report no
 * error -- i.e. the depth counter does not interfere with legitimate use.
 */
bool test_normal_macro_still_works()
{
	unsigned errs = 0;
	std::string out = render("@macro(greet) { hello }@greet()", errs);
	return (out.find("hello") == std::string::npos) || (errs != 0);
}
