/*
 * strtest.cxx
 *
 * Tests for string builtins: substr index clamping and bounded output from
 * the padding/repeat builtins.
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


bool test_substr_normal();
bool test_substr_negative_start();
bool test_substr_past_end();
bool test_pad_normal();
bool test_pad_huge_refused();
bool test_repeat_normal();
bool test_repeat_huge_refused();

int main()
{
	bool result = false, r;

	struct { const char* name; bool (*fn)(); } tests[] = {
		{ "substr normal",            test_substr_normal },
		{ "substr negative start",    test_substr_negative_start },
		{ "substr past end",          test_substr_past_end },
		{ "pad normal",               test_pad_normal },
		{ "pad huge width refused",   test_pad_huge_refused },
		{ "repeat normal",            test_repeat_normal },
		{ "repeat huge count refused", test_repeat_huge_refused },
		{ 0, 0 }
	};
	for (int i = 0; tests[i].fn; ++i)
	{
		r = tests[i].fn();
		if (r)
			std::cout << "str test " << (i + 1) << " ("
				<< tests[i].name << "): failed" << std::endl;
		result |= r;
	}

	std::cout << (result ? "FAILED" : "PASSED") << std::endl;
	return result;
}

bool test_substr_normal()
{
	unsigned errs = 0;
	std::string out = render("@substr(\"Hello World\", 6)", errs);
	if (out != "World" || errs != 0)
		return true;
	errs = 0;
	out = render("@substr(\"Hello World\", 0, 5)", errs);
	return (out != "Hello") || (errs != 0);
}

// A negative start index must clamp to 0, not wrap to a huge size_t.
bool test_substr_negative_start()
{
	unsigned errs = 0;
	std::string out = render("@substr(\"Hello\", -3)", errs);
	return (out != "Hello") || (errs != 0);
}

// A start past the end must yield an empty string, not throw.
bool test_substr_past_end()
{
	unsigned errs = 0;
	std::string out = render("@substr(\"Hello\", 100)", errs);
	return (out != "") || (errs != 0);
}

bool test_pad_normal()
{
	unsigned errs = 0;
	std::string l = render("@lpad(\"x\", 5)", errs);
	if (l != "    x" || errs != 0)
		return true;
	errs = 0;
	std::string rr = render("@rpad(\"x\", 3)", errs);
	return (rr != "x  ") || (errs != 0);
}

// An absurd pad width must be refused (error, no output), not allocated.
bool test_pad_huge_refused()
{
	unsigned errs = 0;
	std::string out = render("@lpad(\"x\", 999999999)", errs);
	return (out.size() != 0) || (errs == 0);
}

bool test_repeat_normal()
{
	unsigned errs = 0;
	std::string out = render("@repeat(\"ab\", 3)", errs);
	return (out != "ababab") || (errs != 0);
}

// An absurd repeat count must be refused, not emit gigabytes of output.
bool test_repeat_huge_refused()
{
	unsigned errs = 0;
	std::string out = render("@repeat(\"ab\", 2000000000)", errs);
	return (out.size() != 0) || (errs == 0);
}
