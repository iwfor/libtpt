/*
 * bench.cxx
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002,2003 Isaac W. Foraker (isaac@tazthecat.net)
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
#include <sstream>
#include <ctime>

const unsigned RUNCOUNT = 1000;

void dumptemplate();
void start();

int main()
{
	try {
		start();
	} catch(const std::exception& e) {
		std::cout << "Exception " << e.what() << std::endl;
	} catch(...) {
		std::cout << "Unknown exception" << std::endl;
	}

	return 0;
}

void start()
{
	std::cout << "Running through " << RUNCOUNT << " loops..." << std::endl;
	std::clock_t starttime = std::clock();
	for (unsigned i=0; i < RUNCOUNT; ++i)
	{
		dumptemplate();
		if (!((i+1) % (RUNCOUNT/10)))
		{
			std::cout << i / (RUNCOUNT/10);
			std::cout.flush();
		}
	}
	std::clock_t endtime = std::clock();
	float totaltime = (endtime - starttime);
	totaltime/= CLOCKS_PER_SEC;
	std::cout << "\nRan through in " << totaltime << " sec" << std::endl;
}

void dumptemplate()
{
	std::stringstream str;
	TPT::Parser p("tests/bench.tpt");
	p.addincludepath("./tests");
	p.run(str);
	// Ignore output
}
