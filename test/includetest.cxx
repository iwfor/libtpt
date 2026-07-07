/*
 * includetest.cxx
 *
 * Security tests for @include / @includetext path confinement and the
 * include cycle/depth guard.
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
#include <fstream>
#include <string>
#include <cstring>
#include <cerrno>
#include <sys/stat.h>
#include <unistd.h>

namespace {

const char* SANDBOX = "inctest_tmp";		// created under the test cwd
const char* ROOT = "inctest_tmp/root";		// the sole allowed include root
const char* SECRET_TEXT = "TOPSECRET";		// lives OUTSIDE the root

void writefile(const std::string& path, const std::string& content)
{
	std::ofstream f(path.c_str(), std::ios::out | std::ios::binary);
	f << content;
}

// Parse an in-memory template confined to ROOT and report output + errors.
std::string render(const char* tpl, unsigned& errcount,
	TPT::IncludeMode mode = TPT::include_confined)
{
	TPT::Symbols sym;
	TPT::Parser p(tpl, static_cast<unsigned long>(std::strlen(tpl)), sym);
	p.setincludemode(mode);
	p.addincludepath(ROOT);
	std::stringstream out;
	p.run(out);
	errcount = p.geterrorcount();
	return out.str();
}

bool contains(const std::string& hay, const std::string& needle)
{
	return hay.find(needle) != std::string::npos;
}

} // end anonymous namespace


bool setup();
bool test_confined_allows_relative();
bool test_blocks_parent_traversal();
bool test_blocks_absolute();
bool test_blocks_cycle();
bool test_unrestricted_optin();
bool test_disabled();

int main()
{
	bool result = false, r;

	if (setup())
	{
		std::cout << "Could not create test sandbox" << std::endl;
		std::cout << "FAILED" << std::endl;
		return 1;
	}

	struct { const char* name; bool (*fn)(); } tests[] = {
		{ "confined allows relative include", test_confined_allows_relative },
		{ "blocks ../ traversal",             test_blocks_parent_traversal },
		{ "blocks absolute path",             test_blocks_absolute },
		{ "blocks include cycle",             test_blocks_cycle },
		{ "unrestricted opt-in works",        test_unrestricted_optin },
		{ "disabled blocks include",          test_disabled },
		{ 0, 0 }
	};
	for (int i = 0; tests[i].fn; ++i)
	{
		r = tests[i].fn();
		if (r)
			std::cout << "include test " << (i + 1) << " ("
				<< tests[i].name << "): failed" << std::endl;
		result |= r;
	}

	std::cout << (result ? "FAILED" : "PASSED") << std::endl;
	return result;
}

// Build:
//   inctest_tmp/
//     secret.txt            (outside the root; must never be readable)
//     root/
//       inc.tph             (legitimate include target)
//       cycle.tph           (includes itself)
bool setup()
{
	if (mkdir(SANDBOX, 0755) && errno != EEXIST)
		return true;
	if (mkdir(ROOT, 0755) && errno != EEXIST)
		return true;
	writefile(std::string(SANDBOX) + "/secret.txt", SECRET_TEXT);
	writefile(std::string(ROOT) + "/inc.tph", "INCLUDED_OK");
	writefile(std::string(ROOT) + "/cycle.tph", "@include(\"cycle.tph\")");
	return false;
}

// A normal relative include inside the root must work and report no error.
bool test_confined_allows_relative()
{
	unsigned errs = 0;
	std::string out = render("@include(\"inc.tph\")", errs);
	return !contains(out, "INCLUDED_OK") || (errs != 0);
}

// "../secret.txt" must not escape the root.
bool test_blocks_parent_traversal()
{
	unsigned errs = 0;
	std::string out = render("@includetext(\"../secret.txt\")", errs);
	return contains(out, SECRET_TEXT) || (errs == 0);
}

// An absolute path to a real, readable file outside the root must still be
// rejected in confined mode.  Pairs with test_unrestricted_optin, which
// confirms the same file IS readable once the policy is relaxed.
bool test_blocks_absolute()
{
	unsigned errs = 0;
	char cwd[4096];
	if (!getcwd(cwd, sizeof(cwd)))
		return true;	// cannot run the test safely
	std::string tpl = std::string("@includetext(\"") + cwd + "/"
		+ SANDBOX + "/secret.txt\")";
	std::string out = render(tpl.c_str(), errs);
	return contains(out, SECRET_TEXT) || (errs == 0);
}

// A file that includes itself must be stopped by the cycle guard, not loop
// or crash.
bool test_blocks_cycle()
{
	unsigned errs = 0;
	std::string out = render("@include(\"cycle.tph\")", errs);
	// The guard must record an error; the important thing is we returned
	// at all (no infinite recursion / stack overflow).
	return (errs == 0);
}

// The legacy escape hatch must still allow absolute reads when explicitly
// requested by the embedder.
bool test_unrestricted_optin()
{
	unsigned errs = 0;
	std::string abspath =
		std::string("@includetext(\"") + SANDBOX + "/secret.txt\")";
	// Build an absolute path to the secret for a fair test.
	char cwd[4096];
	std::string full = abspath;
	if (getcwd(cwd, sizeof(cwd)))
	{
		full = std::string("@includetext(\"") + cwd + "/" + SANDBOX
			+ "/secret.txt\")";
	}
	std::string out = render(full.c_str(), errs, TPT::include_unrestricted);
	return !contains(out, SECRET_TEXT);
}

// Disabled mode must reject even a legitimate relative include.
bool test_disabled()
{
	unsigned errs = 0;
	std::string out = render("@include(\"inc.tph\")", errs,
		TPT::include_disabled);
	return contains(out, "INCLUDED_OK") || (errs == 0);
}
