/*
 * tpt.cxx
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac*nospampleaz*@tazthecat.net)
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

/*
 * This is the source for the tpt command line utility.  Link the object
 * generated from this code with libtpt.a to generate the command line
 * executable.  Install the executable in /usr/local/bin and add the
 * line:
 *
 * #!/usr/local/bin/tpt
 *
 * to the top of your TPT template files to allow them to be run as
 * scripts.
 *
 */

#include "clo.h"

#include <libtpt/tpt.h>
#include <libtpt/compat.h>

#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <cstdio>

const char* VERSION = "${template_fullname}";

void dumptemplate(clo::parser& parser);

int main(int argc, char* argv[])
{
	clo::options options;
	if (argc < 2)
	{
		std::cout << "Usage: tpt <filename>" << std::endl;
		return 0;
	}

	try {
		// Use clo++ to process the command line
		clo::parser parser;
		parser.parse(argc, argv);

		dumptemplate(parser);
	} catch(const clo::exception& e) {
		std::cout << e.what() << std::endl;
	} catch(const std::exception& e) {
		std::cout << "Exception " << e.what() << std::endl;
	} catch(...) {
		std::cout << "Unknown exception" << std::endl;
	}

	return 0;
}


void dumptemplate(clo::parser& parser)
{
	const clo::options& options = parser.get_options();
	const std::vector<std::string>& other = parser.get_non_options();

	// Print version and exit if requested.
	if (options.version)
	{
		TPT::Parser p(VERSION, std::strlen(VERSION));
		std::stringstream str;
		
		p.run(str);
		std::cout << str.str() << std::endl;

		return;
	}

	// Make sure a filename was specified.
	if (other.empty())
	{
		std::cout << "Must specify a template file" << std::endl;
		return;
	}

	// Print a basic CGI header, if requested.
	if (options.cgiheader)
		std::cout << "Content-type: text/html" << std::endl << std::endl;
	
	TPT::Parser p(other[0].c_str());

	// Add Include directories.
	if (!options.include.empty())
	{
		std::vector< std::string >::const_iterator it(options.include.begin()),
			end(options.include.end());
		for (; it != end; ++it)
			p.addincludepath(it->c_str());
	}

	std::stringstream str;

	p.run(str);
	if (!options.check)
	{
		std::cout << str.str();
		std::cout.flush();
	}
	
	if (options.warnings || options.check)
	{
		TPT::ErrorList errlist;
		if (p.geterrorlist(errlist))
		{
			TPT::ErrorList::const_iterator it(errlist.begin()), end(errlist.end());
			for (; it != end; ++it)
				std::cout << (*it) << std::endl;

		}
		else if (options.check)
			std::cout << "No errors" << std::endl;
	}
}

