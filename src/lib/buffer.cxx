/*
 * buffer.cxx
 *
 * Handle file read buffering.
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
 */

#include "conf.h"
#include "libtpt/buffer.h"
#include "libtpt/tptexcept.h"
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstring>

// Anonymous namespace for local constants
namespace {
	const unsigned BUFFER_SIZE = 4096;
}

namespace TPT {

/**
 *
 * Construct a read Buffer for the specified file.
 *
 * @param	filename		name of file to buffer
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(const char* filename) :
	instr(0),
	buffersize(0),
	bufferalloc(BUFFER_SIZE),
	buffer(new char[BUFFER_SIZE]),
	bufferindex(0),
	freestreamwhendone(true),
	done(false)
{
	// Just in case there is an exception while allocating the stream.
	try {
		openfile(filename);
	} catch(...) {
		delete [] buffer;
		throw;
	}
	readfile();
}


/**
 *
 * Construct a read Buffer for an open fstream.  The input stream will
 * not be closed when Buffer is destructed.
 *
 * @param	filein			input fstream
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(std::istream* is) :
	instr(is),
	buffersize(0),
	bufferalloc(BUFFER_SIZE),
	buffer(new char[BUFFER_SIZE]),
	bufferindex(0),
	freestreamwhendone(false),
	done(false)
{
	readfile();
}


/**
 *
 * Construct a read Buffer for an existing buffer.
 *
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(const char* buf, unsigned long bufsize) :
	instr(0),
	buffersize(bufsize),
	bufferalloc(bufsize),
	buffer(new char[bufsize]),
	bufferindex(0),
	freestreamwhendone(false),
	done(!bufsize)	// if zero buffer, then done
{
	std::memcpy(buffer, buf, bufsize);
}


/**
 *
 * Construct a read Buffer on a subspace of an existing Buffer.
 *
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(const Buffer& buf, unsigned long start, unsigned long end) :
	instr(0),
	buffersize(end-start),
	bufferalloc(end-start),
	buffer(new char[end-start]),
	bufferindex(0),
	freestreamwhendone(false),
	done(!(end-start))
{
	std::memcpy(buffer, &buf.buffer[start], end-start);
}


/**
 *
 * Free buffer and fstream if necessary
 *
 */
Buffer::~Buffer()
{
	delete [] buffer;
	if (freestreamwhendone)
		delete instr;
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
	register char c = buffer[bufferindex];
	++bufferindex;

	// If our buffer is empty, try reading from the stream
	// if there is one.
	if (bufferindex >= buffersize)
		readfile();

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
	if (!bufferindex)
		return true;

	--bufferindex;
	done = false;
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
	bufferindex = 0;
	done = false;
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
	// This loop allows seeking to part of buffer that has not yet been read.
	while (!done && (index >= buffersize))
		readfile();
	if (index >= buffersize)
		return true;
	bufferindex = index;
	done = false;

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
unsigned long Buffer::offset() const
{
	return bufferindex;
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
	while (!done && (index >= buffersize))
		readfile();
	if (index >= buffersize)
		throw tptexception("Index out of bounds");
	return buffer[index];
}


/**
 *
 * Perform a boolean check for availability of data in the buffer.
 * Usage can look like:
 *
 * TPT::Buffer readbuf("input.txt");\n
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
Buffer::operator bool() const
{
	return !done;
}


/**
 *
 * Get the size of the current buffer.  If this is a file or
 * stream buffer, the size may be smaller than the current file
 * or stream if the buffer has not been fully filled.
 *
 * @param	none
 * @return	current size of buffer
 * @author	Isaac W. Foraker
 *
 */
unsigned long Buffer::size() const
{
	return buffersize;
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
void Buffer::openfile(const char* filename)
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
bool Buffer::readfile()
{
	if (!done && instr && !instr->eof())
	{
		char buf[BUFFER_SIZE];
		size_t size = 0;
		if (instr->read(buf, BUFFER_SIZE) || instr->gcount())
			size = instr->gcount();
		if (!size)
			done = true;
		else
		{
			if ((buffersize + size) >= bufferalloc)
				enlarge();
			std::memcpy(&buffer[buffersize], buf, size);
			buffersize+= size;
		}
	}
	else
		done = true;

	return done;
}


/*
 * Increase the size of the buffer by BUFFER_SIZE
 *
 */
void Buffer::enlarge()
{
	bufferalloc+= BUFFER_SIZE;
	char* temp = new char[bufferalloc];
	std::memcpy(temp, buffer, buffersize);
	delete [] buffer;
	buffer = temp;
}

/**
 *
 * \example	bufferfile.cxx
 *
 * This is an example of how to use TPT::Buffer with a file.
 *
 */

/**
 *
 * \example	bufferfstream.cxx
 *
 * This is an example of how to use TPT::Buffer with an input
 * fstream.
 *
 */

/**
 *
 * \example	bufferbuffer.cxx
 *
 * This is an example of how to use TPT::Buffer with a buffer.
 *
 */


} // end namespace TPT
