/*
 * dump.cxx
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac@tazthecat.net)
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
 *
 */

#include <libtpt/tpt.h>
#include <libtpt/compat.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <cstdio>

void dumptemplate(const char* filename);

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: dump <filename>" << std::endl;
		return 0;
	}

	try {
		dumptemplate(argv[1]);
	} catch(const std::exception& e) {
		std::cout << "Exception " << e.what() << std::endl;
	} catch(...) {
		std::cout << "Unknown exception" << std::endl;
	}

	return 0;
}

#include "shared.inl"

void dumptemplate(const char* filename)
{
	TPT::Symbols sym;
	sym.set("var", "this is the value of var");
	sym.set("var1", "Supercalifragilisticexpialidocious");
	sym.set("var2", "The red fox runs through the plain and jumps over the fence.");
	sym.set("title", "TEST TITLE");
	sym.push("myarray", "value1");
	sym.push("myarray", "value2");
	sym.push("myarray", "value3");
	sym.push("myarray", "value4");

	TPT::Buffer tptbuf(filename);
	TPT::Parser p(tptbuf, sym);

	p.addfunction("mycallback", &mycallback);
	p.addfunction("fsum", &fsum);
	p.addincludepath("tests");
	p.run(std::cout);

	TPT::ErrorList errlist;
	if (p.geterrorlist(errlist))
	{
		TPT::ErrorList::const_iterator it(errlist.begin()), end(errlist.end());
		for (; it != end; ++it)
			std::cerr << (*it) << std::endl;

	}
}
