/*
 * buffer.h
 *
 * Handle file read buffering.
 *
 * $Id$
 *
 */

/*
 * Copyright (C) 2002 Isaac W. Foraker (isaac@tazthecat.net)
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

#ifndef __libtpt_buffer_h
#define __libtpt_buffer_h

#include <iosfwd>	// Forward declare std::fstream

namespace TPT {


/**
 *
 * The TPT::Buffer class provides a generic way to buffer input from
 * a file, file stream, or existing buffer one character at a time.
 *
 */
class Buffer {
public:
	/// Instantiate on filename.
	explicit Buffer(const char* filename);
	/// Instantiate on open input fstream.
	explicit Buffer(std::istream* is);
	/// Instantiate on existing buffer.
	explicit Buffer(const char* buffer, unsigned long size);
	/// Instantiate on subsection of existing buffer
	explicit Buffer(const Buffer& buf, unsigned long start, unsigned long end);
	/// Cleanup.
	~Buffer();

	/// Get next character from buffer.
	char getnextchar();
	/// Back up in the buffer.
	bool unget();
	/// Reset pointers to beginning of buffer.
	void reset();
	/// Seek to index in buffer.
	bool seek(unsigned long index);
	/// Get current offset into buffer.
	unsigned long offset() const;
	/// True when not at end of buffer.
	operator bool() const;
	/// Index access
	const char operator[](unsigned long index);
	/// Current size
	unsigned long size() const;

private:
	struct Impl;
	Impl* imp;

	Buffer();
	
}; // end class Buffer

} // end namespace TPT


#endif // __libtpt_buffer_h
