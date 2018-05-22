# - Find ogg
# Find the native ogg includes and libraries
#
#  OGG_INCLUDE_DIRS - where to find ogg.h, etc.
#  OGG_LIBRARIES    - List of libraries when using libogg.
#  OGG_FOUND        - True if ogg was found.

find_path(OGG_INCLUDE_DIR ogg.h
    HINTS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr
    PATH_SUFFIXES ogg)

find_library(OGG_LIBRARY NAMES ogg)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OGG
    FOUND_VAR OGG_FOUND
    REQUIRED_VARS
        OGG_INCLUDE_DIR
        OGG_LIBRARY)

if(OGG_FOUND)
    set(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR})
    set(OGG_LIBRARIES ${OGG_LIBRARY})
endif()

mark_as_advanced(OGG_INCLUDE_DIR OGG_LIBRARY)
