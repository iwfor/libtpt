/*
 * dump.cxx
 *
 * $Id$
 *
 */

#ifdef _MSC_VER
// Disable long symbol name warning on MSVC++
#pragma warning(disable:4786)
#endif

#include <tptlib/buffer.h>
#include <tptlib/parse.h>

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


void dumptemplate(const char* filename)
{
	TPTLib::Symbols sym;
	sym.set("var", "this is the value of var");
	sym.set("var1", "Supercalifragilisticexpialidocious");
	sym.set("var2", "The red fox runs through the plain and jumps over the fence.");
	sym.set("title", "TEST TITLE");

	TPTLib::Buffer tptbuf(filename);
	TPTLib::Parser p(tptbuf, &sym);

	std::string tptstr;
	std::stringstream strs(tptstr);
	p.run(strs);

	std::cout << strs.str();
	std::cout.flush();

	TPTLib::ErrorList errlist;
	if (p.geterrorlist(errlist))
	{
		TPTLib::ErrorList::const_iterator it(errlist.begin()), end(errlist.end());
		for (; it != end; ++it)
			std::cerr << (*it) << std::endl;

	}
}
