default: lib

all: lib tests

tests: lib
	cd ${PWD}/test ; ${MAKE} all

lib:
	cd ${PWD}/src ; ${MAKE} libtpt.a

clean:
	cd ${PWD}/src ; \
	${MAKE} clean
	cd ${PWD}/test ; \
	${MAKE} clean

