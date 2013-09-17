# - Find SDL2_ttf
# Find the native SDL2_ttf includes and libraries
#
#  SDL2_TTF_INCLUDE_DIR - where to find SDL.h, etc.
#  SDL2_TTF_LIBRARIES   - List of libraries when using libSDL.
#  SDL2_TTF_FOUND       - True if SDL2_ttf was found.

if(SDL2_TTF_INCLUDE_DIR)
    # Already in cache, be silent
    set(SDL2_TTF_FIND_QUIETLY TRUE)
endif(SDL2_TTF_INCLUDE_DIR)

find_path(SDL2_TTF_INCLUDE_DIR NAMES SDL_ttf.h
    HINTS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr
    PATH_SUFFIXES SDL_image libSDL_image)

find_library(SDL2_TTF_LIBRARY NAMES SDL2_TTF)

# Handle the QUIETLY and REQUIRED arguments and set SDL2_TTF_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_TTF DEFAULT_MSG
    SDL2_TTF_INCLUDE_DIR SDL2_TTF_LIBRARY)

if(SDL2_TTF_FOUND)
  set(SDL2_TTF_LIBRARIES ${SDL2_TTF_LIBRARY})
else(SDL2_TTF_FOUND)
  set(SDL2_TTF_LIBRARIES)
endif(SDL2_TTF_FOUND)

mark_as_advanced(SDL2_TTF_INCLUDE_DIR SDL2_TTF_LIBRARY)
