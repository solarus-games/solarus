####
# By default, you will build a binary with the maximum of compatibility that your current version can provide.
# You may want to produce an optimized (or exotic) binary instead of a standard one.
# To do so, overload the following options :
# 
# SOLARUS_ARCH represent the build (multi-)architecture.
# SOLARUS_SYSROOT represent the path to the OSX SDK.
# SOLARUS_DEPLOYMENT represent the oldest OSX version supported.
#
# Some options set to the cache also need the FORCE parameter for obscure reasons.
#
# Exportable to XCode.
####

# Get the current OSX version as X.X.X and X.X form.
execute_process(COMMAND sw_vers -productVersion
  OUTPUT_VARIABLE SOLARUS_CURRENT_OSX_VERSION
  OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REGEX REPLACE "([0-9]+)\\.([0-9]+)\\.([0-9]+)\\[ \t\r\n]*" "\\1.\\2.\\3" SOLARUS_CURRENT_OSX_VERSION_LONG ${SOLARUS_CURRENT_OSX_VERSION})
string(REGEX REPLACE "([0-9]+)\\.([0-9]+).*" "\\1.\\2" SOLARUS_CURRENT_OSX_VERSION_SHORT ${SOLARUS_CURRENT_OSX_VERSION_LONG})

# Build architectures.
if(NOT SOLARUS_ARCH)
  set(SOLARUS_ARCH "x86_64")
endif()
set(CMAKE_OSX_ARCHITECTURES "${SOLARUS_ARCH}" CACHE STRING "Build architecture" FORCE)

# SDK version.
if(NOT SOLARUS_SYSROOT)
  # Default SDKs locations, from the earlier naming convention to the older one.
  if(EXISTS /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}.sdk)
    set(SOLARUS_SYSROOT "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}.sdk")
  elseif(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}.sdk")
  elseif(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}u.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_SHORT}u.sdk")
  elseif(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_LONG}.sdk)
    set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION_LONG}.sdk")
  else()
    message(WARNING "No ${SOLARUS_CURRENT_OSX_VERSION_SHORT} or ${SOLARUS_CURRENT_OSX_VERSION_LONG} SDK found, force deployment version to default.")
  endif()
endif()
set(CMAKE_OSX_SYSROOT "${SOLARUS_SYSROOT}" CACHE STRING "Build SDK" FORCE)

# Deployment version.
if(NOT SOLARUS_DEPLOYMENT AND DEFINED SOLARUS_SYSROOT)
  set(SOLARUS_DEPLOYMENT "10.7")
endif()
set(CMAKE_OSX_DEPLOYMENT_TARGET "${SOLARUS_DEPLOYMENT}" CACHE STRING "Oldest OS version supported" FORCE)

# Configure root path and set it up on library.
set(CMAKE_MACOSX_RPATH ON)
if(NOT CMAKE_EXE_LINKER_FLAGS MATCHES "-Xlinker -rpath")
  set(CMAKE_EXE_LINKER_FLAGS         "${CMAKE_EXE_LINKER_FLAGS} -Xlinker -rpath -Xlinker @loader_path/../Frameworks/" CACHE STRING "Embed frameworks search path" FORCE)
endif()
set_target_properties(solarus PROPERTIES
  MACOSX_RPATH                       ON
  BUILD_WITH_INSTALL_RPATH           1
  INSTALL_NAME_DIR                   "@rpath"
)

# LuaJIT workaround.
# According to LuaJIT doc, OSX needs to link with additional flags if 64bit build is requested
if(SOLARUS_USE_LUAJIT AND SOLARUS_ARCH MATCHES "x86_64")
  if(XCODE)
    set_property(TARGET solarus PROPERTY
      "XCODE_ATTRIBUTE_LINKER_FLAGS[arch=x86_64]" "-pagezero_size 10000 -image_base 100000000"
    )
  elseif(NOT CMAKE_EXE_LINKER_FLAGS MATCHES "-pagezero_size 10000 -image_base 100000000")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pagezero_size 10000 -image_base 100000000" CACHE STRING "LuaJIT woraround" FORCE)
  endif()
endif()
