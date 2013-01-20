####
# By default, you will build a binary with the maximum of compatibility that your current version can provide.
# You may want to produce an optimized (or exotic) binary instead of a standard one.
# To do so, overload the following options :
# 
# SOLARUS_ARCH represent the build (multi-)architecture.
# SOLARUS_SYSROOT represent the path to the OSX SDK.
# SOLARUS_DEPLOYMENT represent the oder OSX version supported.
#
# Exportable to XCode.
####


# Get the current OSX version as X.X.X and X.X form
execute_process(COMMAND sw_vers -productVersion
  OUTPUT_VARIABLE SOLARUS_CURRENT_OSX_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.[0-9]+[ \t\r\n]*" "\\1.\\2" SOLARUS_CURRENT_OSX_MAJOR_MINOR_VERSION ${SOLARUS_CURRENT_OSX_VERSION})

# Build architectures
if(NOT SOLARUS_ARCH)
  if(${SOLARUS_CURRENT_OSX_VERSION} VERSION_LESS "10.6")
    set(SOLARUS_ARCH "${SOLARUS_ARCH}ppc;")
  endif()
  if(NOT ${SOLARUS_CURRENT_OSX_VERSION} VERSION_LESS "10.4.4")
    set(SOLARUS_ARCH "${SOLARUS_ARCH}i386;")
  endif()
  if(NOT ${SOLARUS_CURRENT_OSX_VERSION} VERSION_LESS "10.5")
    set(SOLARUS_ARCH "${SOLARUS_ARCH}x86_64;")
  endif()
endif()
set(CMAKE_OSX_ARCHITECTURES "${SOLARUS_ARCH}" CACHE STRING "Build architecture" FORCE)

# SDK version
if(NOT SOLARUS_SYSROOT)
  # Default SDKs, in order from the earlier naming convention to the older one
  if(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION}.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION}.sdk")
  elseif(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION}u.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION}u.sdk")
  elseif(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_MAJOR_MINOR_VERSION}.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_MAJOR_MINOR_VERSION}.sdk")
  endif()
endif()
set(CMAKE_OSX_SYSROOT "${SOLARUS_SYSROOT}" CACHE STRING "Build SDK" FORCE)

# Deployment version
if(NOT SOLARUS_DEPLOYMENT)
  if(NOT ${SOLARUS_CURRENT_OSX_VERSION} VERSION_LESS "10.6")
    set(SOLARUS_DEPLOYMENT "10.6")
  else()
    set(SOLARUS_DEPLOYMENT "10.2")
  endif()
endif()
set(CMAKE_OSX_DEPLOYMENT_TARGET "${SOLARUS_DEPLOYMENT}" CACHE STRING "Oldest OS version supported" FORCE)

# Compatibility options
if(SOLARUS_DEPLOYMENT VERSION_LESS "10.6")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-no_compact_linkedit" CACHE STRING "Disable 10.6+ features if deploy on older version" FORCE)
endif()
