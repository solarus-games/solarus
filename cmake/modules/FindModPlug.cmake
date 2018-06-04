# - Find modplug
# Find the native modplug includes and libraries
#
#  MODPLUG_INCLUDE_DIRS - where to find modplug.h, etc.
#  MODPLUG_LIBRARIES    - List of libraries when using libmodplug.
#  MODPLUG_FOUND        - True if modplug found.

find_path(MODPLUG_INCLUDE_DIR modplug.h
    HINTS /usr/local/angstrom/arm/arm-angstrom-linux-gnueabi/usr
    PATH_SUFFIXES modplug libmodplug)

find_library(MODPLUG_LIBRARY NAMES modplug)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MODPLUG
    FOUND_VAR MODPLUG_FOUND
    REQUIRED_VARS
        MODPLUG_INCLUDE_DIR
        MODPLUG_LIBRARY)

if(MODPLUG_FOUND)
    set(MODPLUG_INCLUDE_DIRS ${MODPLUG_INCLUDE_DIR})
    set(MODPLUG_LIBRARIES ${MODPLUG_LIBRARY})
endif()

mark_as_advanced(MODPLUG_INCLUDE_DIR MODPLUG_LIBRARY)
