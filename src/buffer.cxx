/*
 * buffer.cxx
 *
 * Handle file read buffering.
 *
 * $Id$
 *
 */

#include "tptlib/buffer.h"

#include <fstream>

namespace {
	const unsigned DEFAULT_BUFFER_SIZE = 8192;
}

namespace TPTLib {

struct Buffer::impl {
	std::fstream* mFstream;
	bool mFreefstreamwhendone;

	const char* mConstbuffer;
	char* mBuffer;
	unsigned mBuffersize;	// size of allocated buffer
	unsigned mBuffercount;	// number of characters in buffer
	unsigned mBufferindex;

	bool mDone;
	
	impl(unsigned bufsize) :
		mDone(false),
		mBuffer(new char[bufsize]),
		mBuffersize(bufsize),
		mBuffercount(0),
		mConstbuffer(mBuffer),
		mBufferindex(0),
		mFreefstreamwhendone(false)
		{ }
	impl(const char* buffer, unsigned bufsize) :
		mDone(!bufsize),	// if zero buffer, then done
		mBuffer(0),
		mBuffersize(bufsize),
		mBuffercount(bufsize),
		mConstbuffer(buffer),
		mBufferindex(0),
		mFreefstreamwhendone(false)
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
	mImpl = new impl(DEFAULT_BUFFER_SIZE);

	mImpl->openfile(filename);
	mImpl->mFreefstreamwhendone = true;
	mImpl->readfile();
}


/**
 *
 * Construct a read buffer for an open fstream.
 *
 * @param	filein			input fstream
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
Buffer::Buffer(std::fstream* filein)
{
	mImpl = new impl(DEFAULT_BUFFER_SIZE);

	mImpl->mFstream = filein;
	mImpl->readfile();
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
	mImpl = new impl(buffer, size);
}


/**
 *
 * Free buffer and fstream if necessary
 *
 */
Buffer::~Buffer()
{
	delete mImpl;
}


/**
 *
 * Get the next available character from the buffer.  Use
 * Buffer::eof() to determine if more data is available.
 *
 * @param	none
 * @return	next available character;
 * @return	undefined if no more characters available
 * @author	Isaac W. Foraker
 *
 */
char Buffer::getnextchar()
{
	register char c = mImpl->mConstbuffer[mImpl->mBufferindex];
	++mImpl->mBufferindex;

	// If our buffer is empty, try reading from the stream
	// if there is one.
	if (mImpl->mBufferindex >= mImpl->mBuffercount)
		mImpl->readfile();

	return c;
}


/**
 *
 * Check if any more data in buffer.
 *
 * @param	none
 * @return	true if past end of buffer;
 * @return	false if more data available
 * @author	Isaac W. Foraker
 *
 */
bool Buffer::eof() const
{
	return mImpl->mDone;
}


/**
 *
 * Reset the buffer index to the beginning of the input buffer.
 * Buffer::eof() will return true if index could not be reset.
 *
 * @param	none
 * @return	nothing
 * @author	Isaac W. Foraker
 *
 */
void Buffer::reset()
{
	mImpl->mBufferindex = 0;
	if (mImpl->mFstream)
	{
		mImpl->mFstream->seekg(0);
		mImpl->readfile();
	}
	else
		mImpl->mDone = !mImpl->mBuffersize;
}


/**
 *
 * Perform a boolean check for availability of data in the
 * buffer.  Usage can look like:
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
	return !mImpl->mDone;
}


Buffer::impl::~impl()
{
	if (mFreefstreamwhendone)
		delete mFstream;
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
	mFstream = new std::fstream(filename, std::ios::in);
}


/*
 * Read the next block of bytes from the file.  Set
 * the mDone flag if there's no more data to read.
 *
 * @param	none
 * @return	false on success;
 * @return	true if no more data
 *
 */
bool Buffer::impl::readfile()
{
	if (mFstream && mFstream->is_open() && !mFstream->eof())
	{
		mBuffercount = mFstream->readsome(mBuffer, mBuffersize);
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
 * \class Buffer
 *
 * The TPTLib::Buffer class provides a generic way to buffer
 * input from a file, file stream, or existing buffer one
 * character at a time.
 *
 */

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
 * This is an example of how to use TPTLib::Buffer with a readable fstream.
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

