// Example usage of TPTLib::Buffer class.

#include "mylex.h"
#include "mybuffer.h"

#include "tptlib/buffer.h"

int main()
{
	char buffer[BUFSIZE];
	int bufsize = fillbuffer(buffer);

	TPTLib::Buffer inbuf(buffer, bufsize);
	char c;

	while (inbuf)
	{
	    c = inbuf.getnextchar();
	    lex(c);
	}

	return 0;
}
