// Example usage of TPT::Buffer class.

#include "mylex.h"

#include "tpt/buffer.h"

int main()
{
	TPT::Buffer inbuf("myfile.txt");
	char c;

	while (inbuf)
	{
	    c = inbuf.getnextchar();
	    lex(c);
	}

	return 0;
}
