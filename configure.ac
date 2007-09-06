#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(zsdx, 1.0, christopho128@zelda-solarus.com)
AM_INIT_AUTOMAKE(zsdx, 1.0)

AC_CONFIG_SRCDIR(src/ZSDX.cpp)
AC_CONFIG_HEADERS(config.h)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB(SDL, SDL_Init)
AC_CHECK_LIB(SDL_image, IMG_Load)
AC_CHECK_LIB(fmod, FSOUND_Init)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_CHECK_FUNCS([sqrt])

AC_CONFIG_FILES(Makefile src/Makefile)

AC_OUTPUT
