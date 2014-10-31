# Generate config.h with useful configuration info.
include(CheckFunctionExists)
check_function_exists(mkstemp HAVE_MKSTEMP)

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)

configure_file("${CMAKE_SOURCE_DIR}/include/solarus/config.h.in" "${CMAKE_BINARY_DIR}/include/solarus/config.h")

