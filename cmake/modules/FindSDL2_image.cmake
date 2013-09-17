# - Find SDL2_image
# Find the native SDL2_image includes and libraries
#
#  SDL2_image_INCLUDE_DIR - where to find SDL.h, etc.
#  SDL2_image_LIBRARIES   - List of libraries when using libSDL.
#  SDL2_image_FOUND       - True if SDL2_image was found.

if(SDL2_image_INCLUDE_DIR)
    # Already in cache, be silent
    set(SDL2_image_FIND_QUIETLY TRUE)
endif(SDL2_image_INCLUDE_DIR)

find_path(SDL2_image_INCLUDE_DIR NAMES SDL_image.h
    HINTS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr
    PATH_SUFFIXES SDL_image)

find_library(SDL2_image_LIBRARY NAMES SDL2_image)

# Handle the QUIETLY and REQUIRED arguments and set SDL2_image_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image DEFAULT_MSG
    SDL2_image_INCLUDE_DIR SDL2_image_LIBRARY)

if(SDL2_image_FOUND)
  set(SDL2_image_LIBRARIES ${SDL2_image_LIBRARY})
else(SDL2_image_FOUND)
  set(SDL2_image_LIBRARIES)
endif(SDL2_image_FOUND)

mark_as_advanced(SDL2_image_INCLUDE_DIR SDL2_image_LIBRARY)
