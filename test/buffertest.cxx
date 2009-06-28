/*
 * buffertest.cxx
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

#include <libtpt/buffer.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <cstring>

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
		r = test1(argv[1]);
        if (r)
            std::cout << "Buffer test 1: failed" << std::endl;
		result|= r;

		r = test2(argv[1]);
        if (r)
            std::cout << "Buffer test 2: failed" << std::endl;
		result|= r;

		r = test2(argv[1]);
        if (r)
            std::cout << "Buffer test 3: failed" << std::endl;
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
	TPT::Buffer buf(filename);
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
		if (!f.read(&c, 1))
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
	TPT::Buffer buf(&bf);
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
		if (!f.read(&c, 1))
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

	while (bf.read(&buffer[fsize], 65536) || bf.gcount())
	{
		size=bf.gcount();
		fsize+=size;
		falloc+= 65536;
		char* t = new char[falloc];
		std::memcpy(t, buffer, fsize);
		delete buffer;
		buffer = t;
	}

	TPT::Buffer buf(buffer, fsize);
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
		if (!f.read(&c, 1))
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
