# - Find modplug
# Find the native modplug includes and libraries
#
#  MODPLUG_INCLUDE_DIR - where to find modplug.h, etc.
#  MODPLUG_LIBRARIES   - List of libraries when using libmodplug.
#  MODPLUG_FOUND       - True if modplug found.

if(MODPLUG_INCLUDE_DIR)
    # Already in cache, be silent
    set(MODPLUG_FIND_QUIETLY TRUE)
endif(MODPLUG_INCLUDE_DIR)

find_path(MODPLUG_INCLUDE_DIR modplug.h
    PATH_SUFFIXES include include/libmodplug)

find_library(MODPLUG_LIBRARY NAMES modplug)

# Handle the QUIETLY and REQUIRED arguments and set SNDFILE_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MODPLUG DEFAULT_MSG
    MODPLUG_INCLUDE_DIR MODPLUG_LIBRARY)

if(MODPLUG_FOUND)
  set(MODPLUG_LIBRARIES ${MODPLUG_LIBRARY})
else(MODPLUG_FOUND)
  set(MODPLUG_LIBRARIES)
endif(MODPLUG_FOUND)

mark_as_advanced(MODPLUG_INCLUDE_DIR MODPLUG_LIBRARY)
