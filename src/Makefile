OBJECTS=	buffer.o \
			lex_impl.o \
			lexical.o \
			parse.o \
			parse_funcs.o \
			parse_impl.o \
			parse_impl_if.o \
			parse_impl_include.o \
			parse_impl_macro.o \
			parse_impl_rd.o \
			parse_impl_set.o \
			symbols.o \
			symbols_stack.o
CXXFLAGS=	-O2 -pipe -Wall -I../inc


default: libtpt.a

libtpt.a: ${OBJECTS}
	${AR} ${ARFLAGS} libtpt.a ${OBJECTS}
	cp -p libtpt.a ../lib/

clean:
	rm -f *.core ${OBJECTS} libtpt.a

