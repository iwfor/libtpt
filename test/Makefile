LIBS=		-L../lib -ltpt
CXXFLAGS=	-O2 -pipe -Wall -I../inc ${LIBS}

default: all

all: buffertest test1 dump

buffertest: buffertest.cxx
	${CXX} -o $@ buffertest.cxx ${CXXFLAGS}

test1: test1.cxx
	${CXX} -o $@ test1.cxx ${CXXFLAGS}

dump: dump.cxx
	${CXX} -o $@ dump.cxx ${CXXFLAGS}

clean:
	rm -f core test1 buffertest
