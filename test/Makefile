LIBS=		-L../lib -ltpt
CXXFLAGS=	-O2 -pipe -Wall -I../inc ${LIBS}

default: all

all: buffertest test1

buffertest: buffertest.cxx
	${CXX} ${CXXFLAGS} -o $@ buffertest.cxx

test1: test1.cxx
	${CXX} ${CXXFLAGS} -o $@ test1.cxx

clean:
	rm -f core test1 buffertest
