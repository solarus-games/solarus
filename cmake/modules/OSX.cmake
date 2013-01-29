####
# By default, you will build a binary with the maximum of compatibility that your current version can provide.
# You may want to produce an optimized (or exotic) binary instead of a standard one.
# To do so, overload the following options :
# 
# SOLARUS_ARCH represent the build (multi-)architecture.
# SOLARUS_SYSROOT represent the path to the OSX SDK.
# SOLARUS_DEPLOYMENT represent the older OSX version supported.
#
# Exportable to XCode.
####

# Get the current OSX version as X.X.X and X.X form
execute_process(COMMAND sw_vers -productVersion
  OUTPUT_VARIABLE SOLARUS_CURRENT_OSX_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)\\[ \t\r\n]*" "\\1.\\2.\\3" SOLARUS_CURRENT_OSX_VERSION_LONG ${SOLARUS_CURRENT_OSX_VERSION})
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.[0-9]+[ \t\r\n]*" "\\1.\\2" SOLARUS_CURRENT_OSX_VERSION_SHORT ${SOLARUS_CURRENT_OSX_VERSION})

# Build architectures
if(NOT SOLARUS_ARCH)
  if(${SOLARUS_CURRENT_OSX_VERSION_LONG} VERSION_LESS "10.6")
    set(SOLARUS_ARCH "${SOLARUS_ARCH}ppc;")
  endif()
  if(NOT ${SOLARUS_CURRENT_OSX_VERSION_LONG} VERSION_LESS "10.4.4")
    set(SOLARUS_ARCH "${SOLARUS_ARCH}i386;")
  endif()
  if(NOT ${SOLARUS_CURRENT_OSX_VERSION_LONG} VERSION_LESS "10.5")
    set(SOLARUS_ARCH "${SOLARUS_ARCH}x86_64;")
  endif()
endif()
set(CMAKE_OSX_ARCHITECTURES "${SOLARUS_ARCH}" CACHE STRING "Build architecture" FORCE)

# SDK version
if(NOT SOLARUS_SYSROOT)
  # Default SDKs, in order from the earlier naming convention to the older one
  if(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}.sdk")
  elseif(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}u.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}u.sdk")
  elseif(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_LONG}.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_LONG}.sdk")
  endif()
endif()
set(CMAKE_OSX_SYSROOT "${SOLARUS_SYSROOT}" CACHE STRING "Build SDK" FORCE)

# Deployment version
if(NOT SOLARUS_DEPLOYMENT)
  if(NOT ${SOLARUS_CURRENT_OSX_VERSION_LONG} VERSION_LESS "10.6")
    set(SOLARUS_DEPLOYMENT "10.6")
  else()
    set(SOLARUS_DEPLOYMENT "10.2")
  endif()
endif()
set(CMAKE_OSX_DEPLOYMENT_TARGET "${SOLARUS_DEPLOYMENT}" CACHE STRING "Oldest OS version supported" FORCE)

# Compatibility options
if(NOT XCODE)
  if(SOLARUS_DEPLOYMENT VERSION_LESS "10.6")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-no_compact_linkedit" CACHE STRING "Disable 10.6+ features if deploy on older version" FORCE)
  endif()
# TODO try to build on 10.6+ with following flags and run on 10.5
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-syslibroot,${CMAKE_OSX_SYSROOT}" CACHE STRING "Link with SDK")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector" CACHE STRING "Emit extra code to check for buffer overflows on function with vulnerable objects")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-all" CACHE STRING "Emit extra code to check for buffer overflows on all functions")
endif()
