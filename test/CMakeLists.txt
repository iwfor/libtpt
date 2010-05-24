
SET( TPT_TESTS
    bench
    buffertest
    dump
    test1
    test2
    test3
)
FOREACH( TESTFILE ${TPT_TESTS} )
    add_executable( ${TESTFILE} ${TESTFILE}.cxx )
    target_link_libraries( ${TESTFILE} ${TPT_LIB} )
ENDFOREACH()