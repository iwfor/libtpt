/*
 * shared.inl
 *
 * Some common code shared by the test apps.
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

#include <cstdio>
#include <cstdlib>

bool mycallback(std::ostream& os, TPT::Object& params)
{
	TPT::Object::ArrayType& pl = params.array();

	TPT::Object::ArrayType::const_iterator it(pl.begin()), end(pl.end());
	for (; it != end; ++it)
	{
		TPT::Object& obj = *(*it).get();
		if (obj.gettype() == TPT::Object::type_scalar)
			os << obj.scalar();
		else
			os << "[non-scalar]";
	}
	return false;
}

// Get sum of set of floats
bool fsum(std::ostream& os, TPT::Object& params)
{
	TPT::Object::ArrayType& pl = params.array();
	double work=0;

	TPT::Object::ArrayType::const_iterator it(pl.begin()), end(pl.end());
	for (; it != end; ++it)
	{
		TPT::Object& obj = *(*it).get();
		if (obj.gettype() == TPT::Object::type_scalar)
			work+= std::atof(obj.scalar().c_str());
		else
			return true;	// expected scalar!
	}
	os << work;
	return false;
}

// Dump a string buffer (for debug use)
void dumpstr(const char* title, const std::string& s)
{
    bool newline = true;
    unsigned lineno = 1;
    std::string::const_iterator it(s.begin()), end(s.end());
    std::cout << "\n" << title << " = (" << s.size() << ") <quote>\n";
    for (; it != end; ++it) {
        if (newline) {
            char numbuf[16];
            std::sprintf(numbuf, "%03u ", lineno++);
            std::cout << numbuf;
            newline = false;
        }
        if (*it == '\n') {
            std::cout << '|';
            newline = true;
        }
        std::cout << *it;
    }
    std::cout << "</quote> (" << lineno-1 << " lines)" <<std::endl;
}
