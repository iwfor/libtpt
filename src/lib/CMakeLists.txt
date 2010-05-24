
file(GLOB TPT_SOURCE *.cxx)

add_library(${TPT_LIB} ${LIB_MODE} ${TPT_SOURCE})
