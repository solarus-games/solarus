cmake_minimum_required(VERSION 2.6)
project(SOLARUS_ENGINE)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_MODULE_PATH "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/modules/")
set(SOLARUS_INSTALL_DESTINATION "bin")

# find the headers and the libraries of dependencies
find_package(OpenGL)
find_package(SDL2 "2" REQUIRED)
find_package(SDL2_image "2" REQUIRED)
find_package(SDL2_ttf "2.0.12" REQUIRED)
find_package(OpenAL REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Ogg REQUIRED)
find_package(ModPlug REQUIRED)
find_package(Lua51 REQUIRED)
find_package(PhysFS REQUIRED)

# Explicit link to libdl is needed for Lua on some systems.
find_library(DL_LIBRARY dl)
if(${DL_LIBRARY} MATCHES DL_LIBRARY-NOTFOUND)
  set(DL_LIBRARY "")
endif()

include(CheckFunctionExists)
check_function_exists(mkstemp HAVE_MKSTEMP)
configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in ${CMAKE_BINARY_DIR}/include/config.h)

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)
configure_file(${CMAKE_SOURCE_DIR}/include/config.h.in ${CMAKE_BINARY_DIR}/include/config.h)

# source files
file(
  GLOB
  source_files
  src/*.cpp
  src/third_party/snes_spc/*.cpp
  src/third_party/hqx/*.c
  src/main/*.cpp
  src/entities/*.cpp
  src/hero/*.cpp
  src/hud/*.cpp
  src/lowlevel/*.cpp
  src/lua/*.cpp
  src/menus/*.cpp
  src/movements/*.cpp
  include/*.h
  include/entities/*.h
  include/hero/*.h
  include/hud/*.h
  include/lowlevel/*.h
  include/lua/*.h
  include/menus/*.h
  include/movements/*.h
  include/third_party/snes_spc/*.h
  include/third_party/hqx/*.h
)

# Configuration for OSX and iOS build and deployment
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(NOT SOLARUS_IOS_BUILD)
    include(OSX)
  else()
    include(iOS)
  endif()
endif()

# additional source files for Apple systems
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(source_files
    ${source_files}
    src/lowlevel/apple/AppleInterface.mm
    include/lowlevel/apple/AppleInterface.h
  )
endif()

# build the executable

# Add executable target, into CFBundle form if requested or iOS build
if(SOLARUS_BUNDLE OR SOLARUS_IOS_BUILD)
  include(addCFBundleTarget)
else()
  add_executable(solarus
    ${source_files}
  )
endif()

# generate -I flags
if (OPENGL_FOUND)
  include_directories(${OPENGL_INCLUDE_DIR})
endif()
include_directories(
  ${CMAKE_BINARY_DIR}/include
  ${SOLARUS_ENGINE_SOURCE_DIR}/include
  ${SOLARUS_ENGINE_SOURCE_DIR}/include/third_party
  ${SOLARUS_ENGINE_SOURCE_DIR}/include/third_party/snes_spc
  ${MODPLUG_INCLUDE_DIR}  # Before SDL because we want the sndfile.h of ModPlug.
  ${SDL2_INCLUDE_DIR}
  ${SDL2_TTF_INCLUDE_DIR}
  ${SDL2_IMAGE_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${VORBISFILE_INCLUDE_DIR}
  ${OGG_INCLUDE_DIR}
  ${LUA_INCLUDE_DIR}
  ${PHYSFS_INCLUDE_DIR}
)

# generate -l flags
if (OPENGL_FOUND)
  target_link_libraries(solarus ${OPENGL_gl_LIBRARY})
endif()
target_link_libraries(solarus
  ${OPENGL_gl_LIBRARY}
  ${SDL2_LIBRARY}
  ${SDL2_IMAGE_LIBRARY}
  ${SDL2_TTF_LIBRARY}
  ${OPENAL_LIBRARY}
  ${LUA_LIBRARY}
  ${DL_LIBRARY}
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
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error -Wno-all -Wno-unknown-pragmas")

  # Some "no-warning" flags are not supported with gcc < 4.0
  if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    exec_program(gcc ARGS --version OUTPUT_VARIABLE CMAKE_C_COMPILER_VERSION)
    if(NOT CMAKE_C_COMPILER_VERSION MATCHES "(^|[^\\.])[0-3]\\.[0-9]")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-fatal-errors -Wno-extra")
    endif()
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-fatal-errors -Wno-extra")
  endif()

else()
  set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-Werror ${CMAKE_CXX_FLAGS_DEBUG}")
endif()


if(OPENGL_FOUND)
  set(HAVE_OPENGL_OR_ES 1)
else()
  set(HAVE_OPENGL_OR_ES 0)
endif()
add_definitions(-DSOLARUS_HAVE_OPENGL_OR_ES=${HAVE_OPENGL_OR_ES})

set(DEFAULT_QUEST "." CACHE STRING "Path to the quest to launch if none is specified at runtime.")
if(DEFAULT_QUEST)
  add_definitions(-DSOLARUS_DEFAULT_QUEST=\"${DEFAULT_QUEST}\")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(INITIAL_WRITE_DIR "Solarus")
else()
  set(INITIAL_WRITE_DIR ".solarus")
endif()
set(WRITE_DIR ${INITIAL_WRITE_DIR} CACHE STRING "Directory where Solarus savegames are stored, relative to the user's base write directory.")
if(WRITE_DIR)
  add_definitions(-DSOLARUS_WRITE_DIR=\"${WRITE_DIR}\")
endif()

if(PANDORA)
  set(INITIAL_DEFAULT_QUEST_WIDTH 400)
else()
  set(INITIAL_DEFAULT_QUEST_WIDTH 320)
endif()
set(DEFAULT_QUEST_WIDTH ${INITIAL_DEFAULT_QUEST_WIDTH} CACHE INTEGER "Default width of the quest screen in pixels.")
set(DEFAULT_QUEST_HEIGHT 240 CACHE INTEGER "Default height of the quest screen in pixels.")
if(DEFAULT_QUEST_WIDTH)
  add_definitions(-DSOLARUS_DEFAULT_QUEST_WIDTH=${DEFAULT_QUEST_WIDTH})
endif()
if(DEFAULT_QUEST_HEIGHT)
  add_definitions(-DSOLARUS_DEFAULT_QUEST_HEIGHT=${DEFAULT_QUEST_HEIGHT})
endif()

if(GCWZERO)
  add_definitions(-DGCWZERO)
endif()

if(CAANOO OR GCWZERO)
  set(INITIAL_COLOR_DEPTH 16)
else()
  set(INITIAL_COLOR_DEPTH 32)
endif()
set(COLOR_DEPTH ${INITIAL_COLOR_DEPTH} CACHE INTEGER "Number of bits per pixel for surfaces.")
if(COLOR_DEPTH)
  add_definitions(-DSOLARUS_COLOR_DEPTH=${COLOR_DEPTH})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(INITIAL_SCREEN_DOUBLEBUF OFF)
else()
  set(INITIAL_SCREEN_DOUBLEBUF ON)
endif()
option(SCREEN_DOUBLEBUF "Define if your plaform supports double buffering." ${INITIAL_SCREEN_DOUBLEBUF})
if(SCREEN_DOUBLEBUF)
  add_definitions(-DSOLARUS_SCREEN_DOUBLEBUF)
endif()

# files to install with make install
# install the bundle if requested, or only the binary else
if(SOLARUS_BUNDLE)
  install(TARGETS solarus
    BUNDLE DESTINATION ${SOLARUS_INSTALL_DESTINATION}
  )
else()
  install(TARGETS solarus
    RUNTIME DESTINATION ${SOLARUS_INSTALL_DESTINATION}
  )
endif()

