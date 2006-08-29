/*
 * iparse.h
 *
 * Copyright (C) 2002-2006 Isaac W. Foraker (isaac at noscience dot net)
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

#ifndef include_tpt_iparse_h
#define include_tpt_iparse_h

#include <libtpt/tpttypes.h>
#include <libtpt/buffer.h>
#include <libtpt/symbols.h>
#include <iosfwd>
#include <string>
#include <vector>

namespace TPT {

// Forward Declarations
class Parser_Impl;
class Object;

/**
 * The IParser class parses a template like the Parser class, except the
 * IParser class allows the Symbols table to be modified by the template.
 *
 * @author	Isaac W. Foraker
 * @exception	tptexception
 *
 */
class IParser {
public:
	IParser(const char* filename, Symbols& st);
	IParser(const char* buf, unsigned long size, Symbols& st);
	IParser(Buffer& buf, Symbols& st);
	~IParser();

	/// Parse template into a string.
	std::string run();
	/// Parse template directly to stream.
	bool run(std::ostream& os);
	/// Just do a syntax check on template.
	bool syntax();
	/// Get the error count from a parse.
	unsigned geterrorcount() const;
	/// Get the error list from a parse.
	bool geterrorlist(ErrorList& errlist);

	/// Add an include search path.
	void addincludepath(const char* path);
	
	/// Add a callback function.
	bool addfunction(const char* name,
			bool (*func)(std::ostream&, Object&));

private:
	Parser_Impl* imp;
	IParser();
};

} // end namespace TPT

#endif // include_tpt_iparse_h
