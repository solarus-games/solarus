cmake_minimum_required(VERSION 2.6)
project(SOLARUS_ENGINE)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_MODULE_PATH "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/modules/")
set(SOLARUS_INSTALL_DESTINATION "bin")

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
  include/snes_spc/*.h
)

# additional source files for Apple systems
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(source_files
    ${source_files}
    src/lowlevel/apple/AppleInterface.mm
    include/lowlevel/apple/AppleInterface.h
  )
  # SDL 1.3 avoid to link SDLMain
  if(SDL_VERSION VERSION_LESS "1.3")
    set(source_files
      ${source_files}
      src/lowlevel/apple/SDLMain.m
      include/lowlevel/apple/SDLMain.h
    )
  endif()
endif()

# build the executable
set(main_source_file src/lowlevel/Main.cc)
add_library(solarus_static
  STATIC
  ${source_files}
)

#Add executable target, into CFBundle form if requested or iOS build
if(SOLARUS_BUNDLE OR SOLARUS_IOS_BUILD)
  include(addCFBundleTarget)
else()
  add_executable(solarus
    ${main_source_file}
  )
endif()

# generate -I flags
include_directories(
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
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Force to be the less pedantic possible with the Release build
  set(CMAKE_CXX_FLAGS "-Wno-error -Wno-all -Wno-fatal-errors -Wno-extra -Wno-unknown-pragmas")
else()
  set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-Werror ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

# Configuration for OSX and iOS build and deployment
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(NOT SOLARUS_IOS_BUILD)
    include(OSX)
  else()
    include(iOS)
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

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(DEFAULT_BUFFERING 0)
else()
  set(DEFAULT_BUFFERING 1)
endif()
set(SCREEN_DOUBLEBUF ${DEFAULT_BUFFERING} CACHE STRING "Define if the current platform support double buffering.")
if(SCREEN_DOUBLEBUF)
  add_definitions(-DSOLARUS_SCREEN_DOUBLEBUF =\"${SCREEN_DOUBLEBUF}\")
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
if(SOLARUS_BUNDLE)
  install(TARGETS	solarus
    BUNDLE DESTINATION ${SOLARUS_INSTALL_DESTINATION}
  )
else()
  install(TARGETS	solarus
    RUNTIME DESTINATION ${SOLARUS_INSTALL_DESTINATION}
  )
endif()

