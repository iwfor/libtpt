
project("libtpt")

cmake_minimum_required(VERSION 2.6)

SET(TPT_LIB tpt)
SET(TPT_EXE tptmpl)
SET(LIB_MODE STATIC)

link_directories(${CMAKE_BINARY_DIR}/src/lib)
include_directories(${CMAKE_SOURCE_DIR}/inc)
add_subdirectory(src/lib)
add_subdirectory(src/cli)
add_subdirectory(test)
