/*
 * looptest.cxx
 *
 * Tests for the @while iteration budget, which prevents a non-terminating
 * loop from hanging the process.
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

// Render with a low @while iteration cap so the runaway case finishes fast.
std::string render(const char* tpl, unsigned& errcount, unsigned long maxloop)
{
	TPT::Symbols sym;
	TPT::Parser p(tpl, static_cast<unsigned long>(std::strlen(tpl)), sym);
	p.setmaxloop(maxloop);
	std::stringstream out;
	p.run(out);
	errcount = p.geterrorcount();
	return out.str();
}

} // end anonymous namespace


bool test_finite_while_runs();
bool test_infinite_while_is_capped();

int main()
{
	bool result = false, r;

	r = test_finite_while_runs();
	if (r)
		std::cout << "loop test 1 (finite while runs): failed" << std::endl;
	result |= r;

	r = test_infinite_while_is_capped();
	if (r)
		std::cout << "loop test 2 (infinite while capped): failed" << std::endl;
	result |= r;

	std::cout << (result ? "FAILED" : "PASSED") << std::endl;
	return result;
}

// A normal, terminating loop must run to completion with no error, well below
// the cap.
bool test_finite_while_runs()
{
	unsigned errs = 0;
	std::string out = render(
		"@set(x, 3)@while (x > 0) {${x}@set(x, x-1)\\\n}", errs, 1000);
	// Expect 3, 2, 1 emitted in order and no error.
	return (out.find("3") == std::string::npos) ||
		(out.find("2") == std::string::npos) ||
		(out.find("1") == std::string::npos) ||
		(errs != 0);
}

// A non-terminating loop must stop at the cap with a recorded error rather
// than hang.  Reaching this assertion at all proves it terminated.
bool test_infinite_while_is_capped()
{
	unsigned errs = 0;
	render("@while (1) {}", errs, 1000);
	return (errs == 0);
}
