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
	char* m_buffer;
	int m_bufsize;
	int m_bufindex;
	fstream* m_filein;

	bool openfile(char* filename);
	bool readfile();
}; // end struct Buffer::pimpl


} // end namespace TPTLib

