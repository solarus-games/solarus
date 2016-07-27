# - Find vorbis
# Find the native vorbis library
#
#  VORBIS_LIBRARIES   - List of libraries when using libvorbis.
#  VORBIS_FOUND       - True if vorbis was found.

find_library(VORBIS_LIBRARY NAMES vorbis)

# Handle the QUIETLY and REQUIRED arguments and set VORBIS_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VORBIS DEFAULT_MSG
    VORBIS_LIBRARY)

if(VORBIS_FOUND)
  set(VORBIS_LIBRARIES ${VORBIS_LIBRARY})
else(VORBIS_FOUND)
  set(VORBIS_LIBRARIES)
endif(VORBIS_FOUND)

mark_as_advanced(VORBIS_LIBRARY)
