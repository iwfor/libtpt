OBJECTS=	buffer.o \
			lexical.o \
			parse.o \
			parse_impl.o \
			parse_impl_rd.o \
			symbols.o
CXXFLAGS=	-O2 -pipe -Wall -I../inc

default: libtpt.a

libtpt.a: ${OBJECTS}
	${AR} ${ARFLAGS} libtpt.a ${OBJECTS}

clean:
	rm -f *.core ${OBJECTS} libtpt.a

