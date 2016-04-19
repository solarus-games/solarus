# Generate config.h with useful configuration info.
include(CheckFunctionExists)
check_function_exists(mkstemp MKSTEMP_FUNCTION_EXIST)

include(CheckCxxSymbolExists)
check_cxx_symbol_exists(mkstemp "cstdlib" MKSTEMP_SYMBOL_EXIST)

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)

if(MKSTEMP_FUNCTION_EXIST AND MKSTEMP_SYMBOL_EXIST)
	set(HAVE_MKSTEMP)
endif()

configure_file("${CMAKE_SOURCE_DIR}/include/solarus/config.h.in" "${CMAKE_BINARY_DIR}/include/solarus/config.h")

