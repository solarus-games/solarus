cmake_minimum_required(VERSION 2.6)
project(SOLARUS_ENGINE)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_MODULE_PATH "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/modules/")

# find the headers and the libraries of dependencies
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(OpenAL REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Ogg REQUIRED)
find_package(ModPlug REQUIRED)
find_package(Lua51 REQUIRED)
find_package(PhysFS REQUIRED)

# source files
file(
  GLOB
  source_files
  src/*.cpp
  src/entities/*.cpp
  src/hero/*.cpp
  src/hud/*.cpp
  src/lowlevel/*.cpp
  src/lua/*.cpp
  src/menus/*.cpp
  src/movements/*.cpp
  src/snes_spc/*.cpp
  include/*.h
  include/entities/*.h
  include/hero/*.h
  include/hud/*.h
  include/lowlevel/*.h
  include/lua/*.h
  include/menus/*.h
  include/movements/*.h
  include/simpleini/*.h
  include/snes_spc/*.h
)

# additional source files for Mac OS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include_directories(
    ${SOLARUS_ENGINE_SOURCE_DIR}/include/lowlevel/osx
  )
  set(source_files
    ${source_files}
    src/lowlevel/osx/SDLMain.m
    src/lowlevel/osx/OSXInterface.mm
    include/lowlevel/osx/SDLMain.h
    include/lowlevel/osx/OSXInterface.h
  )
endif()

# build the executable
set(main_source_file src/lowlevel/Main.cc)
add_library(solarus_static
  STATIC
  ${source_files}
)
add_executable(solarus
  ${main_source_file}
)

# Workaround : -isystem flag isn't correctly set on OSX when using SYSTEM option.
# Remove this when http://public.kitware.com/Bug/view.php?id=10837 will be accepted.
if(APPLE)
  set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif()

# generate -isystem flags, or -I flags if not supported
include_directories(SYSTEM
  ${SOLARUS_ENGINE_SOURCE_DIR}/include
  ${SOLARUS_ENGINE_SOURCE_DIR}/include/snes_spc
  ${SDL_INCLUDE_DIR}
  ${SDLTTF_INCLUDE_DIR}
  ${SDLIMAGE_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${VORBISFILE_INCLUDE_DIR}
  ${OGG_INCLUDE_DIR}
  ${MODPLUG_INCLUDE_DIR}
  ${LUA_INCLUDE_DIR}
  ${PHYSFS_INCLUDE_DIR}
)

# generate -l flags
target_link_libraries(solarus
  solarus_static
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLTTF_LIBRARY}
  ${OPENAL_LIBRARY}
  ${LUA_LIBRARY}
  ${PHYSFS_LIBRARY}
  ${VORBISFILE_LIBRARY}
  ${OGG_LIBRARY}
  ${MODPLUG_LIBRARY}
)

# default compilation flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()
set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-Werror ${CMAKE_CXX_FLAGS_DEBUG}")

# Compilation flags for Mac OS X deployment
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND ${CMAKE_BUILD_TYPE} MATCHES "Release")
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
    # Default SDKs, in order to the earlier naming convention to the older one
    if(EXISTS /Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION}.sdk)
      set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_CURRENT_OSX_VERSION}.sdk")
    elseif(EXISTS /Developer/SDKs/MacOSX10.4u.sdk)
      set(SOLARUS_SYSROOT "/Developer/SDKs/MacOSX10.4u.sdk")
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
      set(SOLARUS_DEPLOYMENT "10.2.8")
    endif()
  endif()
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${SOLARUS_DEPLOYMENT}" CACHE STRING "Oldest OS version supported" FORCE)

  # Compatibility options
  if(SOLARUS_DEPLOYMENT VERSION_LESS "10.6")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-no_compact_linkedit" CACHE STRING "Disable 10.6+ features if deploy on older version" FORCE)
  endif()
endif()

# solarus options
option(DEBUG_KEYS "Enable the debugging keys." OFF)
if(DEBUG_KEYS)
  add_definitions(-DSOLARUS_DEBUG_KEYS)
endif()

set(DEFAULT_QUEST "." CACHE STRING "Path to the quest to launch if none is specified at runtime.")
if(DEFAULT_QUEST)
  add_definitions(-DSOLARUS_DEFAULT_QUEST=\"${DEFAULT_QUEST}\")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(DEFAULT_WRITE_DIR "Solarus")
else()
  set(DEFAULT_WRITE_DIR ".solarus")
endif()
set(WRITE_DIR ${DEFAULT_WRITE_DIR} CACHE STRING "Directory where Solarus savegames are stored, relative to the user's base write directory.")
if(WRITE_DIR)
  add_definitions(-DSOLARUS_WRITE_DIR=\"${WRITE_DIR}\")
endif()

if(PANDORA)
  set(DEFAULT_SCREEN_WIDTH 400)
else()
  set(DEFAULT_SCREEN_WIDTH 320)
endif()
set(SCREEN_WIDTH ${DEFAULT_SCREEN_WIDTH} CACHE INTEGER "Width of the logical screen in pixels.")
set(SCREEN_HEIGHT 240 CACHE INTEGER "Height of the logical screen in pixels.")
if(SCREEN_WIDTH)
  add_definitions(-DSOLARUS_SCREEN_WIDTH=${SCREEN_WIDTH})
endif()
if(SCREEN_HEIGHT)
  add_definitions(-DSOLARUS_SCREEN_HEIGHT=${SCREEN_HEIGHT})
endif()

if(PANDORA OR CAANOO)
  set(DEFAULT_COLOR_DEPTH 16)
else()
  set(DEFAULT_COLOR_DEPTH 32)
endif()
set(COLOR_DEPTH ${DEFAULT_COLOR_DEPTH} CACHE INTEGER "Number of bits per pixel for surfaces.")
if(COLOR_DEPTH)
  add_definitions(-DSOLARUS_COLOR_DEPTH=${COLOR_DEPTH})
endif()

if(CAANOO)
  set(DEFAULT_SCREEN_FORCE_MODE 2)
elseif(PANDORA)
  set(DEFAULT_SCREEN_FORCE_MODE 5)
else()
  set(DEFAULT_SCREEN_FORCE_MODE -1)
endif()
set(SCREEN_FORCE_MODE ${DEFAULT_SCREEN_FORCE_MODE} CACHE INTEGER "Video mode to force (-1 means no restriction).")
if(SCREEN_FORCE_MODE)
  add_definitions(-DSOLARUS_SCREEN_FORCE_MODE=${SCREEN_FORCE_MODE})
endif()

# files to install with make install
install(TARGETS	solarus
  RUNTIME DESTINATION bin
) 

# Also install into CFBundle form
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND SOLARUS_OSX_BUNDLE)
  include(AddOSXBundleTarget)
endif()

