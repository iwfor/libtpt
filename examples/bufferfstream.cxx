// Example usage of TPT::Buffer class.

#include "mylex.h"

#include "libtpt/buffer.h"

int main()
{
	fstream mystream("myfile.txt");
	TPT::Buffer inbuf(mystream);
	char c;

	while (inbuf)
	{
	    c = inbuf.getnextchar();
	    lex(c);
	}

	return 0;
}
