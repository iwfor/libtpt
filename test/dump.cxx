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
	TPTLib::ErrorList errlist;
	TPTLib::Buffer tptbuf(filename);
	TPTLib::Parser p(tptbuf);

	std::string tptstr;
	std::stringstream strs(tptstr);
	p.run(strs);

	std::cout << strs.str();
	std::cout.flush();

	if (p.geterrorlist(errlist))
	{
		TPTLib::ErrorList::const_iterator it(errlist.begin()), end(errlist.end());
		for (; it != end; ++it)
			std::cerr << (*it) << std::endl;

	}

}