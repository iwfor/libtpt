/*
 * parse.cxx
 *
 * Parser
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002-2003 Isaac W. Foraker (isaac@tazthecat.net)
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


#include "conf.h"
#include "parse_impl.h"
#include <libtpt/parse.h>
#include <algorithm>
#include <sstream>
#include <iostream>

namespace TPT {


/**
 * Construct a Parser for the specified file.
 *
 * @param	filename	Path to TPT source file.
 *
 */
Parser::Parser(const char* filename)
{
	imp = new Parser_Impl(filename);
}

/**
 * Construct a Parser for the specified file and Symbols table.
 *
 * @param	filename	Path to TPT source file.
 * @param	st			Reference to Symbols table.
 *
 */
Parser::Parser(const char* filename, const Symbols& st)
{
	imp = new Parser_Impl(filename);
	imp->symbols.copy(st);
}

/**
 * Construct a Parser for the specified fixed length buffer.
 *
 * @param	buffer		Pointer to buffer of TPT source.
 * @param	size		Size of TPT source buffer.
 *
 */
Parser::Parser(const char* buffer, unsigned long size)
{
	imp = new Parser_Impl(buffer, size);
}

/**
 * Construct a Parser for the specified fixed length buffer and Symbols
 * table.
 *
 * @param	buffer		Pointer to buffer of TPT source.
 * @param	size		Size of TPT source buffer.
 *
 */
Parser::Parser(const char* buffer, unsigned long size, const Symbols& st)
{
	imp = new Parser_Impl(buffer, size);
	imp->symbols.copy(st);
}

/**
 * Construct a Parser for a Buffer.
 *
 * @param	buf			Reference to buffer containing source template.
 *
 */
Parser::Parser(Buffer& buf)
{
	imp = new Parser_Impl(buf);
}

/**
 * Construct a Parser for a Buffer and Symbols table.
 *
 * @param	buf			Reference to Buffer containing source template.
 * @param	st			Reference to Symbols table.
 *
 */
Parser::Parser(Buffer& buf, const Symbols& st)
{
	imp = new Parser_Impl(buf);
	imp->symbols.copy(st);
}

/**
 * Destruct this Parser.
 *
 * @param	none
 *
 */
Parser::~Parser()
{
	delete imp;
}

/**
 * Parse the template in Buffer and return the result as a string.
 *
 * Note: This method is provided for completeness, but the preferred
 * method of parsing is to use streams instead.
 *
 * @param	none
 *
 */
std::string Parser::run()
{
	std::string temp;
	std::stringstream ss(temp);
	run(ss);
	return ss.str();
}

/**
 * Parse the template in Buffer, outputing the result to the given
 * stream while parsing.
 *
 * @param	os		Reference to an output stream to write.
 * @return	false on success;
 * @return	true if there were errors or warnings.
 *
 */
bool Parser::run(std::ostream& os)
{
	imp->errlist.clear();
	return imp->pass1(&os);
}

/**
 * Perform only syntax checking on Buffer.
 *
 * @param	none
 * @return	false on success;
 * @return	true if there were errors or warnings.
 *
 */
bool Parser::syntax()
{
	return imp->pass1(0);
}

/**
 * Get the number of errors and warnings.
 *
 * @param	none
 * @return	0 is good.
 *
 */
unsigned Parser::geterrorcount() const
{
	return imp->errlist.size();
}

/**
 * Copy the errors and warnings generated by the last parse into
 * the specified array.
 *
 * @param	errlist		Reference to array to receive list.
 * @return	false if no errors;
 * @return	true if errors copied.
 *
 */
bool Parser::geterrorlist(ErrorList& errlist)
{
	if (!imp->errlist.empty())
	{
		errlist = imp->errlist;
		return true;
	}
	else
	{
		errlist.clear();
		return false;
	}
}


/**
 * Add a path to the Include search list.  By default, only ./ is
 * searched.
 *
 * @param	path	Path to be searched for include files.
 * @return	nothing
 * 
 */
void Parser::addincludepath(const char* path)
{
	imp->inclist.push_back(path);
}


/**
 * Register a callback function to handle TPT calls to the specified
 * function name.
 *
 * See the LibTPT Documentation for details on how to write a
 * a callback funcion.
 *
 * @param	name	Name of the function (without the @).
 * @param	func	Function to use as callback.
 * @return	false on success;
 * @return	true if name already is registered to another function.
 *
 */
bool Parser::addfunction(const char* name,
		bool (*func)(std::ostream&, Object&))
{
	if (imp->isuserfunc(name))
		return true;
	imp->funcs[name] = func;
	return false;
}

} // end namespace TPT
