# - Find SDL2_image
# Find the native SDL2_image includes and libraries
#
#  SDL2_IMAGE_INCLUDE_DIR - where to find SDL_image.h, etc.
#  SDL2_IMAGE_LIBRARIES   - List of libraries when using libSDL_image.
#  SDL2_IMAGE_FOUND       - True if SDL2_image was found.

if(SDL2_IMAGE_INCLUDE_DIR)
    # Already in cache, be silent
    set(SDL2_IMAGE_FIND_QUIETLY TRUE)
endif(SDL2_IMAGE_INCLUDE_DIR)

find_path(SDL2_IMAGE_INCLUDE_DIR NAMES SDL_image.h
    HINTS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr
    PATH_SUFFIXES SDL_image)

find_library(SDL2_IMAGE_LIBRARY NAMES SDL2_image)

# Handle the QUIETLY and REQUIRED arguments and set SDL2_IMAGE_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_image DEFAULT_MSG
    SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)

if(SDL2_IMAGE_FOUND)
  set(SDL2_IMAGE_LIBRARIES ${SDL2_IMAGE_LIBRARY})
else(SDL2_IMAGE_FOUND)
  set(SDL2_IMAGE_LIBRARIES)
endif(SDL2_IMAGE_FOUND)

mark_as_advanced(SDL2_IMAGE_INCLUDE_DIR SDL2_IMAGE_LIBRARY)
