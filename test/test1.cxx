/*
 * test1.cxx
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

#include <libtpt/tpt.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <ostream>
#include <cstring>
#include <cstdlib>

bool test1(unsigned testcount);

int main(int argc, char* argv[])
{
	bool result=false, r;

	if (argc != 2) {
		std::cout << "Usage: test1 <testcount>" << std::endl;
		return 0;
	}

	try {
		r = test1(std::atoi(argv[1]));
		result|= r;
	} catch(const std::exception& e) {
		result = true;
		std::cout << "Exception " << e.what() << std::endl;
	} catch(...) {
		result = true;
		std::cout << "Unknown exception" << std::endl;
	}
	if (result)
		std::cout << "FAILED" << std::endl;
	else
		std::cout << "PASSED" << std::endl;

	return result;
}

#include "shared.inl"

bool test1(unsigned testcount)
{
	TPT::ErrorList errlist;
	bool result = false;
	TPT::Symbols sym;

	sym.set("var", "this is the value of var");
	sym.set("var1", "Supercalifragilisticexpialidocious");
	sym.set("var2", "The red fox runs through the plain and jumps over the fence.");
	sym.set("title", "TEST TITLE");
	sym.push("myarray", "value1");
	sym.push("myarray", "value2");
	sym.push("myarray", "value3");
	sym.push("myarray", "value4");

	char tptfile[256], outfile[256];
	unsigned i;

	for (i = 0; i < testcount; ++i) {
		// generate test file names by rule
		sprintf(tptfile, "tests/test%u.tpt", i+1);
		sprintf(outfile, "tests/test%u.out", i+1);

		// Process the tpt file and store the result in a string
		TPT::Parser p(tptfile, sym);
		std::string tptstr;
		std::stringstream strs(tptstr);
		p.addfunction("mycallback", &mycallback);
		p.addfunction("fsum", &fsum);
		p.addincludepath("tests");
		p.run(strs);

		if (p.geterrorlist(errlist)) {
			std::cout << "Errors!" << std::endl;
			TPT::ErrorList::const_iterator it(errlist.begin()), end(errlist.end());
			for (; it != end; ++it)
				std::cout << (*it) << std::endl;
		}

		// Load the out file
		TPT::Buffer outbuf(outfile);
		std::string outstr;
		while (outbuf)
			outstr+= outbuf.getnextchar();

		// Compare tptstr to outstr
		if (strs.str() != outstr) {
			result|= true;
			std::cout << "test" << (i+1) << ".tpt: ";
			std::cout << "failed" << std::endl;
dumpstr("tptstr", strs.str());
dumpstr("outstr", outstr);
		}
	}

	return result;
}
