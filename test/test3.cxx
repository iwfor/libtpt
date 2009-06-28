/*
 * test3.cxx
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
#include <fstream>
#include <cstring>
#include <cstdio>

#include "shared.inl"

bool test1(unsigned testcount);

int main(int argc, char* argv[])
{
	bool result=false, r;

	if (argc != 2) {
		std::cout << "Usage: test3 <testcount>" << std::endl;
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

const char* fruits[] = {
    "Apple", "Orange", "Grapes", "Banana", "Lemon", "Lime", "Kiwi", "Peach",
    "Pear", "Nectarine"
};

const unsigned numfruits = sizeof(fruits)/sizeof(char*);

bool test1(unsigned testcount)
{
	TPT::ErrorList errlist;
	bool result = false;
	TPT::Object obj;
	TPT::Symbols sym;
    char numbuf[16];
	unsigned i;

    for (i=0; i < 10; ++i) {
        std::sprintf(numbuf, "%u", i);
        obj["count"][i] = numbuf;
    }
    for (i=0; i < numfruits; ++i) {
        obj["fruit"][i] = fruits[i];
    }
    for (i=0; i < 10; ++i) {
        std::sprintf(numbuf, "%u", i);
        obj["fruitid"][fruits[i]] = numbuf;
    }

    sym.set("obj", obj);

	char tptfile[256], varfile[256];
	std::map< std::string, std::string > vars;

	for (i = 0; i < testcount; ++i) {
		// generate test file names by rule
		snprintf(tptfile, sizeof(tptfile), "tests/objtest%u.tpt", i+1);
		snprintf(varfile, sizeof(varfile), "tests/objtest%u.out", i+1);

		// Process the tpt file and store the result in a string
		TPT::IParser p(tptfile, sym);
		std::string tptstr;
		std::stringstream strs(tptstr);
		p.run(strs);

		if (p.geterrorlist(errlist)) {
			std::cout << "Errors!" << std::endl;
			TPT::ErrorList::const_iterator it(errlist.begin()), end(errlist.end());
			for (; it != end; ++it)
				std::cout << (*it) << std::endl;
		}

		// Load the out file
		TPT::Buffer outbuf(varfile);
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
