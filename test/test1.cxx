/*
 * test1.cxx
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
#include <cstring>

bool test1(const char* filename);

int main(int argc, char* argv[])
{
	bool result=false, r;

	if (argc != 2)
	{
		std::cout << "Usage: test1 <filename>" << std::endl;
		return 0;
	}

	try {
		std::cout << "Test1: ";
		r = test1(argv[1]);
		if (r) std::cout << "failed" << std::endl;
		else std::cout << "passed" << std::endl;
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

bool test1(const char* filename)
{
	char tptfile[256], outfile[256];
	strcpy(tptfile, filename);
	strcpy(outfile, filename);
	strcat(tptfile, ".tpt");
	strcat(outfile, ".out");

	TPTLib::SymbolTable sym;
	TPTLib::Buffer buf(tptfile);

	sym["var"] = "this is the value of var";

	TPTLib::Parser p(buf, &sym);
	p.run(std::cout);

	return false;
}
