/*
 * buffertest.cxx
 *
 * $Id$
 *
 */

#include <tptlib/buffer.h>

#include <iostream>
#include <fstream>
#include <stdexcept>

bool test1(const char* filename);	// file
bool test2(const char* filename);	// stream
bool test3(const char* filename);	// memory
// TODO: Test unget, seek, reset, and []

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
		return 0;
	}

	bool result=false, r;

	try {
		std::cout << "Test1: ";
		r = test1(argv[1]);
		if (r) std::cout << "failed" << std::endl;
		else std::cout << "passed" << std::endl;
		result|= r;

		std::cout << "Test2: ";
		r = test2(argv[1]);
		if (r) std::cout << "failed" << std::endl;
		else std::cout << "passed" << std::endl;
		result|= r;

		std::cout << "Test3: ";
		r = test2(argv[1]);
		if (r) std::cout << "failed" << std::endl;
		else std::cout << "passed" << std::endl;
		result|= r;

	} catch(const std::exception& e) {
		result = true;
		std::cout << "Exception - " << e.what() << std::endl;
	} catch(const char* e) {
		result = true;
		std::cout << "Exception - " << e << std::endl;
	} catch(...) {
		result = true;
		std::cout << "Exception occured" << std::endl;
	}
	if (result)
		std::cout << "FAILED" << std::endl;
	else
		std::cout << "PASSED" << std::endl;

	return result;
}

bool test1(const char* filename)
{
	TPTLib::Buffer buf(filename);
	std::fstream f(filename, std::ios::in | std::ios::binary);
	char c, d;
	bool filehasdata(true), bufhasdata;

	if (!f.is_open())
	{
		std::cout << "Could not open file" << std::endl;
		return true;
	}

	// Can not use f.eof to match with buf, since eof() requires read
	// past end of file, so this code may look funny
	for (;;)
	{
		if (f.readsome(&c, 1) != 1)
		{
			filehasdata = false;
			break;
		}
		if (!buf)
			break;
		d = buf.getnextchar();
		if (c != d)
		{
			std::cout << "Buffer does not match file" << std::endl;
			return true;
		}
	}
	bufhasdata = buf;
	if (filehasdata != bufhasdata)
	{
		std::cout << "Buffer is wrong size" << std::endl;
		return true;
	}

	return false;
}


bool test2(const char* filename)
{
	std::fstream bf(filename, std::ios::in | std::ios::binary);
	TPTLib::Buffer buf(&bf);
	std::fstream f(filename, std::ios::in | std::ios::binary);
	char c, d;
	bool filehasdata(true), bufhasdata;

	if (!f.is_open())
	{
		std::cout << "Could not open file" << std::endl;
		return true;
	}

	// Can not use f.eof to match with buf, since eof() requires read
	// past end of file, so this code may look funny
	for (;;)
	{
		if (f.readsome(&c, 1) != 1)
		{
			filehasdata = false;
			break;
		}
		if (!buf)
			break;
		d = buf.getnextchar();
		if (c != d)
		{
			std::cout << "Buffer does not match file" << std::endl;
			return true;
		}
	}
	bufhasdata = buf;
	if (filehasdata != bufhasdata)
	{
		std::cout << "Buffer is wrong size" << std::endl;
		return true;
	}

	return false;
}

bool test3(const char* filename)
{
	std::fstream bf(filename, std::ios::in | std::ios::binary);
	unsigned long fsize=0, falloc=65536;
	char* buffer = new char[falloc];
	size_t size;

	while ((size = bf.readsome(&buffer[fsize], 65536)) < 1)
	{
		fsize+=size;
		falloc+= 65536;
		char* t = new char[falloc];
		memcpy(t, buffer, fsize);
		delete buffer;
		buffer = t;
	}

	TPTLib::Buffer buf(buffer, fsize);
	std::fstream f(filename, std::ios::in | std::ios::binary);
	char c, d;
	bool filehasdata(true), bufhasdata;

	if (!f.is_open())
	{
		std::cout << "Could not open file" << std::endl;
		return true;
	}

	// Can not use f.eof to match with buf, since eof() requires read
	// past end of file, so this code may look funny
	for (;;)
	{
		if (f.readsome(&c, 1) != 1)
		{
			filehasdata = false;
			break;
		}
		if (!buf)
			break;
		d = buf.getnextchar();
		if (c != d)
		{
			std::cout << "Buffer does not match file" << std::endl;
			return true;
		}
	}
	bufhasdata = buf;
	if (filehasdata != bufhasdata)
	{
		std::cout << "Buffer is wrong size" << std::endl;
		return true;
	}

	return false;
}
