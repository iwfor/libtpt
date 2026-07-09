
SET( TPT_TESTS
    bench
    buffertest
    smartptrtest
    includetest
    macrotest
    mathtest
    strtest
    dump
    test1
    test2
    test3
)
FOREACH( TESTFILE ${TPT_TESTS} )
    add_executable( ${TESTFILE} ${TESTFILE}.cxx )
    target_link_libraries( ${TESTFILE} ${TPT_LIB} )
ENDFOREACH()

# smartptrtest exercises the reference count from multiple threads.
FIND_PACKAGE( Threads )
IF( CMAKE_THREAD_LIBS_INIT )
    target_link_libraries( smartptrtest ${CMAKE_THREAD_LIBS_INIT} )
ENDIF()