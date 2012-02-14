# - Find ogg
# Find the native ogg includes and libraries
#
#  OGG_INCLUDE_DIR - where to find ogg.h, etc.
#  OGG_LIBRARIES   - List of libraries when using libogg.
#  OGG_FOUND       - True if ogg was found.

if(OGG_INCLUDE_DIR)
    # Already in cache, be silent
    set(OGG_FIND_QUIETLY TRUE)
endif(OGG_INCLUDE_DIR)

find_path(OGG_INCLUDE_DIR ogg.h
    PATH_SUFFIXES include include/ogg)

find_library(OGG_LIBRARY NAMES ogg)

# Handle the QUIETLY and REQUIRED arguments and set OGG_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OGG DEFAULT_MSG
    OGG_INCLUDE_DIR OGG_LIBRARY)

if(OGG_FOUND)
  set(OGG_LIBRARIES ${OGG_LIBRARY})
else(OGG_FOUND)
  set(OGG_LIBRARIES)
endif(OGG_FOUND)

mark_as_advanced(OGG_INCLUDE_DIR OGG_LIBRARY)
