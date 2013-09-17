# - Find SDL2
# Find the native SDL2 includes and libraries
#
#  SDL2_INCLUDE_DIR - where to find SDL.h, etc.
#  SDL2_LIBRARIES   - List of libraries when using libSDL.
#  SDL2_FOUND       - True if SDL2 was found.

if(SDL2_INCLUDE_DIR)
    # Already in cache, be silent
    set(SDL2_FIND_QUIETLY TRUE)
endif(SDL2_INCLUDE_DIR)

find_path(SDL2_INCLUDE_DIR NAMES SDL.h SDL_opengles.h
    HINTS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr
    PATH_SUFFIXES SDL2)

find_library(SDL2_LIBRARY NAMES SDL2)

# Handle the QUIETLY and REQUIRED arguments and set SDL2_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2 DEFAULT_MSG
    SDL2_INCLUDE_DIR SDL2_LIBRARY)

if(SDL2_FOUND)
  set(SDL2_LIBRARIES ${SDL2_LIBRARY})
else(SDL2_FOUND)
  set(SDL2_LIBRARIES)
endif(SDL2_FOUND)

mark_as_advanced(SDL2_INCLUDE_DIR SDL2_LIBRARY)
