/*
 * buffer.cxx
 *
 * Handle file read buffering
 *
 * $Id$
 *
 */

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstdlib>


namespace TPTLib {

class Buffer {
public:
	Buffer();
	Buffer(char* filename);
	Buffer(std::fstream* filein);
	Buffer(char* buffer, int size);
	~Buffer();

	char getnextchar();

private:
	struct pimpl;
	pimpl* m_pimpl;
	
}; // end class Buffer


struct Buffer::pimpl {
	bool m_fileopen;
	bool m_bufferalloc;
	bool m_closefilewhendone;
	
	fstream* m_filein;

	char* m_buffer;
	int m_bufsize;
	int m_bufindex;
	
	bool openfile(char* filename);
	bool readfile();

	pimpl() : m_fileopen(false), m_bufferalloc(false), m_closefilewhendone(false) {}
	~pimpl();
	
}; // end struct Buffer::pimpl

Buffer::Buffer()
{
	m_pimpl = new pimpl;

}

Buffer::Buffer(char* filename)
{
	m_pimpl = new pimpl;

	m_pimpl->openfile(filename);
}

Buffer::Buffer(std::fstream* filein)
{
	m_pimpl = new pimpl;

	m_pimpl->m_fileopen = true;
	m_pimpl->m_filein = filein;
}

Buffer::~Buffer()
{
	delete m_pimpl;
}

Buffer::pimpl::~pimpl()
{
	if (m_closefilewhendone)
	{
		delete m_filein;
	}
}

bool Buffer::pimpl::openfile(char* filename)
{
	m_filein = new fstream(filename);
	return false;
}


} // end namespace TPTLib

