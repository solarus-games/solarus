# - Find vorbisfile
# Find the native modplug includes and libraries
#
#  VORBISFILE_INCLUDE_DIR - where to find vorbisfile.h, etc.
#  VORBISFILE_LIBRARIES   - List of libraries when using libvorbisfile.
#  VORBISFILE_FOUND       - True if vorbisfile was found.

if(VORBISFILE_INCLUDE_DIR)
    # Already in cache, be silent
    set(VORBISFILE_FIND_QUIETLY TRUE)
endif(VORBISFILE_INCLUDE_DIR)

find_path(VORBISFILE_INCLUDE_DIR vorbisfile.h
    PATH_SUFFIXES include include/vorbis)

find_library(VORBISFILE_LIBRARY NAMES vorbisfile)

# Handle the QUIETLY and REQUIRED arguments and set VORBISFILE_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(VORBISFILE DEFAULT_MSG
    VORBISFILE_INCLUDE_DIR VORBISFILE_LIBRARY)

if(VORBISFILE_FOUND)
  set(VORBISFILE_LIBRARIES ${VORBISFILE_LIBRARY})
else(VORBISFILE_FOUND)
  set(VORBISFILE_LIBRARIES)
endif(VORBISFILE_FOUND)

mark_as_advanced(VORBISFILE_INCLUDE_DIR VORBISFILE_LIBRARY)
