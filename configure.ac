#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([zsdx], [1.0], [christopho128@zelda-solarus.com])
AM_INIT_AUTOMAKE([zsdx], [1.0])

AC_CONFIG_SRCDIR([src/ZSDX.cpp])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_CHECK_LIB([fmod], [main], , AC_MSG_ERROR([The FMOD library is missing]))
AC_CHECK_LIB([SDL_image], [IMG_Load], , AC_MSG_ERROR([The SDL_image library is missing]))
AC_CHECK_LIB([SDL], [SDL_Init], , AC_MSG_ERROR([The SDL library is missing]))
AC_CHECK_LIB([SDLmain], [main], , AC_MSG_ERROR([The SDLmain library is missing]))
AC_CHECK_LIB([mingw32], [main])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h], , AC_MSG_ERROR([Cannot locate stdlib.h]))
AC_CHECK_HEADERS([SDL/SDL.h], , AC_MSG_ERROR([The SDL header file (SDL/SDL.h) is missing]))
AC_CHECK_HEADERS([SDL/SDL_image.h], , AC_MSG_ERROR([The SDL_image header file (SDL/SDL_image.h) is missing]))
AC_CHECK_HEADERS([fmod/fmod.h], , AC_MSG_ERROR([The FMOD header file (fmod/fmod.h) is missing]))

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE

# Checks for library functions.
AC_SEARCH_LIBS([sqrt], [m], , AC_MSG_ERROR([Cannot locate the sqrt function]))

AC_CONFIG_FILES(Makefile src/Makefile)

AC_OUTPUT
