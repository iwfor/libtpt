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

#include <iostream>
#include <fstream>

namespace {
	const unsigned BUFFER_SIZE = 1024;
	const unsigned UNGET_BUFFER_SIZE = 3;
}

namespace TPTLib {

struct Buffer::impl {
	std::istream* mIs;
	bool mFreefstreamwhendone;

	const char* mConstbuffer;
	char* mBuffer;
	unsigned mBuffersize;	// size of allocated buffer
	unsigned mBuffercount;	// number of characters in buffer
	unsigned mBufferindex;

	char mUngetbuffer[UNGET_BUFFER_SIZE];
	unsigned mUngetindex;

	bool mDone;
	
	impl(unsigned bufsize) :
		mDone(false),
		mBuffer(new char[bufsize]),
		mBuffersize(bufsize),
		mBuffercount(0),
		mConstbuffer(mBuffer),
		mBufferindex(0),
		mFreefstreamwhendone(false),
		mUngetindex(0)
		{ }
	impl(const char* buffer, unsigned bufsize) :
		mDone(!bufsize),	// if zero buffer, then done
		mBuffer(0),
		mBuffersize(bufsize),
		mBuffercount(bufsize),
		mConstbuffer(buffer),
		mBufferindex(0),
		mFreefstreamwhendone(false),
		mUngetindex(0)
		{ }
	~impl();
	
	void openfile(const char* filename);
	bool readfile();

}; // end struct Buffer::impl


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
	imp = new impl(BUFFER_SIZE);

	imp->openfile(filename);
	imp->mFreefstreamwhendone = true;
	imp->readfile();
}


/**
 *
 * Construct a read buffer for an open fstream.  The input fstream will
 * not be closed when Buffer is destructed.
 *
 * @param	filein			input fstream
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(std::istream* is)
{
	imp = new impl(BUFFER_SIZE);

	imp->mIs = is;
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
	imp = new impl(buffer, size);
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
 * bool() to determine if more data is available.
 *
 * @param	none
 * @return	next available character;
 * @return	undefined if no more characters available
 * @author	Isaac W. Foraker
 *
 */
char Buffer::getnextchar()
{
	if (imp->mUngetindex)
		return imp->mUngetbuffer[--imp->mUngetindex];

	register char c = imp->mConstbuffer[imp->mBufferindex];
	++imp->mBufferindex;

	// If our buffer is empty, try reading from the stream
	// if there is one.
	if (imp->mBufferindex >= imp->mBuffercount)
		imp->readfile();

	return c;
}


/**
 *
 * Put a single character back into the buffer.  A maximum of three
 * characters may be put back on the buffer at a time.
 *
 * @param	c			character to put back.
 * @return	false on success;
 * @return	true if unget buffer full
 * @author	Isaac W. Foraker
 *
 */
bool Buffer::unget(char c)
{
	if (imp->mUngetindex >= UNGET_BUFFER_SIZE)
		return true;

	imp->mUngetbuffer[imp->mUngetindex];
	++imp->mUngetindex;
	return false;
}


/**
 *
 * Reset the buffer index to the beginning of the input buffer.
 * operator bool() will return false if index could not be reset.
 *
 * @param	none
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void Buffer::reset()
{
	imp->mBufferindex = 0;
	imp->mUngetindex = 0;
	if (imp->mIs)
	{
		imp->mIs->seekg(0);
		imp->readfile();
	}
	else
		imp->mDone = !imp->mBuffersize;
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
	return !imp->mDone || imp->mUngetindex;
}



Buffer::impl::~impl()
{
	if (mFreefstreamwhendone)
		delete mIs;
	if (mBuffer)
		delete mBuffer;
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
void Buffer::impl::openfile(const char* filename)
{
	std::fstream* is = new std::fstream(filename, std::ios::in);
	if (!is->is_open())
		mDone = true;
	mIs = is;
}


/*
 * Read the next block of bytes from the file.  Set the mDone flag if
 * there's no more data to read.
 *
 * @param	none
 * @return	false on success;
 * @return	true if no more data
 *
 */
bool Buffer::impl::readfile()
{
	if (mIs && !mIs->eof())
	{
		mBuffercount = mIs->readsome(mBuffer, mBuffersize);
		mBufferindex = 0;
		if (mBuffercount < 1)
			mDone = true;
	}
	else
		mDone = true;
	return mDone;
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

