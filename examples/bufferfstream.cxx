// Example usage of TPTLib::Buffer class.

#include "mylex.h"

#include "tptlib/buffer.h"

int main()
{
	fstream mystream("myfile.txt");
	TPTLib::Buffer inbuf(mystream);
	char c;

	while (inbuf)
	{
	    c = inbuf.getnextchar();
	    lex(c);
	}

	return 0;
}
