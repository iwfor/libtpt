OBJECTS=	buffer.o \
			lexical.o \
			parse.o
CXXFLAGS=	-I../include

all: libtpt.a

libtpt.a: ${OBJECTS}
	${AR} ${ARFLAGS} libtpt.a ${OBJECTS}

clean:
	rm -f *.core ${OBJECTS} libtpt.a

