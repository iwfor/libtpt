/*
 * buffer.cxx
 *
 * Handle file read buffering.
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2001 Isaac W. Foraker (isaac*nospam*@tazthecat.net)
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

#include "tptlib/buffer.h"

#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

namespace {
	const unsigned BUFFER_SIZE = 8192;
}

namespace TPTLib {

struct Buffer::Impl {
	std::istream* instr;
	bool freestreamwhendone;
	char* buffer;
	unsigned long buffersize;
	unsigned long bufferalloc;
	unsigned bufferindex;
	bool done;
	
	Impl(unsigned bufsize) :
		done(false),
		buffersize(0),
		bufferalloc(bufsize),
		buffer(new char[bufsize]),
		bufferindex(0),
		freestreamwhendone(false)
		{ }
	Impl(const char* buf, unsigned bufsize) :
		done(!bufsize),	// if zero buffer, then done
		buffersize(0),
		bufferalloc(bufsize),
		buffer(new char[bufsize]),
		bufferindex(0),
		freestreamwhendone(false)
		{ memcpy(buffer, buf, bufsize); }
	~Impl();

	
	void openfile(const char* filename);
	bool readfile();
	void enlarge();	// increase buffer size

}; // end struct Buffer::Impl


/**
 *
 * Construct a read buffer for the specified file.
 *
 * @param	filename		name of file to buffer
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(const char* filename)
{
	imp = new Impl(BUFFER_SIZE);

	imp->openfile(filename);
	imp->freestreamwhendone = true;
	imp->readfile();
}


/**
 *
 * Construct a read buffer for an open fstream.  The input stream will
 * not be closed when Buffer is destructed.
 *
 * @param	filein			input fstream
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(std::istream* is)
{
	imp = new Impl(BUFFER_SIZE);

	imp->instr = is;
	imp->readfile();
}


/**
 *
 * Construct a read buffer for an existing buffer.
 *
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(const char* buffer, int size)
{
	imp = new Impl(buffer, size);
}


/**
 *
 * Free buffer and fstream if necessary
 *
 */
Buffer::~Buffer()
{
	delete imp;
}


/**
 *
 * Get the next available character from the buffer.  Use operator
 * bool() to determine if more data is available.  Reading past the end
 * of the buffer results in undefined behavior.
 *
 * @param	none
 * @return	next available character;
 * @return	undefined if no more characters available
 * @author	Isaac W. Foraker
 *
 */
char Buffer::getnextchar()
{
	register char c = imp->buffer[imp->bufferindex];
	++imp->bufferindex;

	// If our buffer is empty, try reading from the stream
	// if there is one.
	if (imp->bufferindex >= imp->buffersize)
		imp->readfile();

	return c;
}


/**
 *
 * Unget a single character back into the buffer.
 *
 * @param	c			character to put back.
 * @return	false on success;
 * @return	true if unget buffer full
 * @author	Isaac W. Foraker
 *
 */
bool Buffer::unget()
{
	if (!imp->bufferindex)
		return true;

	--imp->bufferindex;
	return false;
}


/**
 *
 * Reset the buffer index to the beginning of the input buffer.
 *
 * @param	none
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void Buffer::reset()
{
	imp->bufferindex = 0;
}


/**
 *
 * Seek to a index point in the buffer.  The seek will force reading of
 * the file or stream if the index has not yet been buffered.  If the
 * index is past the end of the buffer, the attempt will fail and the
 * internal index will not be reset.
 *
 * @param	index			Offset into buffer from beginning.
 * @return	false on success;
 * @return	true if past end of buffer
 * @author	Isaac W. Foraker
 *
 */
bool Buffer::seek(unsigned long index)
{
	while (!imp->done && (index >= imp->buffersize))
		imp->readfile();
	if (imp->done)
		return true;
	imp->bufferindex = index;

	return false;
}


/**
 *
 * Get the offset of the next character in the buffer to be read.  Use
 * operator bool() to determine if there is data in the buffer.
 * 
 * @param	none
 * @return	current offset into buffer of next character to be read.
 * @author	Isaac W. Foraker
 *
 */
unsigned long Buffer::offset()
{
	return imp->bufferindex;
}


/**
 *
 * Read from a specific index in the buffer, without losing the current
 * position information, reading the input file or stream if necessary.
 *
 * @param	index			Offset into buffer from beginning.
 * @return	character at index;
 * @return	undefined if invalid index
 * @author	Isaac W. Foraker
 *
 */
const char Buffer::operator[](unsigned long index)
{
	while (!imp->done && (index >= imp->buffersize))
		imp->readfile();
	return imp->buffer[index];
}


/**
 *
 * Perform a boolean check for availability of data in the buffer.
 * Usage can look like:
 *
 * TPTLib::Buffer readbuf("input.txt");\n
 * if (readbuf) {\n
 *		.\n
 *		.\n
 *		.\n
 * }
 *
 * @param	none
 * @return	false if no data is available;
 * @return	true if data is available
 * @author	Isaac W. Foraker
 *
 */
Buffer::operator bool()
{
	return !imp->done;
}


Buffer::Impl::~Impl()
{
	delete buffer;
	if (freestreamwhendone)
		delete instr;
}


/*
 * Open a file stream for the specified file.
 *
 * @param	filename
 * @return	false on success
 * @return	true on failure
 * @author	Isaac W. Foraker
 *
 */
void Buffer::Impl::openfile(const char* filename)
{
	std::fstream* is = new std::fstream(filename, std::ios::in | std::ios::binary);

	if (!is->is_open())
		done = true;
	instr = is;
}


/*
 * Read the next block of bytes from the file.  Set the done flag if
 * there's no more data to read.
 *
 * @param	none
 * @return	false on success;
 * @return	true if no more data
 *
 */
bool Buffer::Impl::readfile()
{
	if (!done && instr && !instr->eof())
	{
		char buf[BUFFER_SIZE];
		size_t size;
		size = instr->readsome(buf, BUFFER_SIZE-1);
		if (size < 1)
			done = true;
		else
		{
			if ((buffersize + size) >= bufferalloc)
				enlarge();
			std::memcpy(&buffer[buffersize], buf, size);
			buffersize+= size;
//			buffer[buffersize] = 0;
		}
	}
	else
		done = true;
	return done;
}

void Buffer::Impl::enlarge()
{
	bufferalloc+= BUFFER_SIZE;
	char* temp = new char[bufferalloc];
	std::memcpy(temp, buffer, buffersize);
	delete buffer;
	buffer = temp;
}

/**
 *
 * \example	bufferfile.cxx
 *
 * This is an example of how to use TPTLib::Buffer with a file.
 *
 */

/**
 *
 * \example	bufferfstream.cxx
 *
 * This is an example of how to use TPTLib::Buffer with an input
 * fstream.
 *
 */

/**
 *
 * \example	bufferbuffer.cxx
 *
 * This is an example of how to use TPTLib::Buffer with a buffer.
 *
 */


} // end namespace TPTLib
