// Example usage of TPTLib::Buffer class.

#include "mylex.h"

#include "tptlib/buffer.h"

int main()
{
	TPTLib::Buffer inbuf("myfile.txt");
	char c;

	while (inbuf)
	{
	    c = inbuf.getnextchar();
	    lex(c);
	}

	return 0;
}
