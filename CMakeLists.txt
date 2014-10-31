cmake_minimum_required(VERSION 2.8.11)
project(SOLARUS_ENGINE)

# Default settings.
option(SOLARUS_USE_LUAJIT "Use LuaJIT instead of default Lua (recommended)" ON)
set(SOLARUS_LIBRARY_INSTALL_DESTINATION "lib" CACHE PATH "Library install destination")
set(SOLARUS_EXECUTABLE_INSTALL_DESTINATION "bin" CACHE PATH "Binary install destination")
set(SOLARUS_HEADERS_INSTALL_DESTINATION "include" CACHE PATH "Headers install destination")

# Default compilation flags.

# Compile as C++11.
if(MINGW)
  # To avoid a compilation error in vorbisfile.h with fseeko64.
  set(CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Release")
  # Force to be the less pedantic possible with the Release build
  set(CMAKE_CXX_FLAGS "-Wno-error -Wno-all -Wno-unknown-pragmas -Wno-fatal-errors -Wno-extra ${CMAKE_CXX_FLAGS}")
else()
  set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "-Werror -Wextra -pedantic ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

if(WIN32)
  # Windows: disable the console by default.
  if(MSVC)
    set_target_properties(SOLARUS_ENGINE PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "-mwindows ${CMAKE_CXX_FLAGS}")
  endif()
endif()

# Solarus default settings.
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

# Find dependencies.
set(CMAKE_MODULE_PATH "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/modules/")
#find_package(OpenGL)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(OpenAL REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Ogg REQUIRED)
find_package(ModPlug REQUIRED)
find_package(PhysFS REQUIRED)
if(SOLARUS_USE_LUAJIT)
  find_package(LuaJit REQUIRED)
else()
  find_package(Lua51 REQUIRED)
endif()

# Explicit link to libdl is needed for Lua on some systems.
find_library(DL_LIBRARY dl)
if("${DL_LIBRARY}" MATCHES DL_LIBRARY-NOTFOUND)
  set(DL_LIBRARY "")
endif()

# Generate config.h with useful configuration info.
include(CheckFunctionExists)
check_function_exists(mkstemp HAVE_MKSTEMP)

include(CheckIncludeFiles)
check_include_files(unistd.h HAVE_UNISTD_H)
configure_file("${CMAKE_SOURCE_DIR}/include/solarus/config.h.in" "${CMAKE_BINARY_DIR}/include/solarus/config.h")

# Source files.
file(
  GLOB
  source_files
  src/*.cpp
  src/third_party/snes_spc/*.cpp
  src/third_party/hqx/*.c
  src/entities/*.cpp
  src/hero/*.cpp
  src/hud/*.cpp
  src/lowlevel/*.cpp
  src/lowlevel/shaders/*.cpp
  src/lua/*.cpp
  src/menus/*.cpp
  src/movements/*.cpp
  src/containers/*.cpp
  include/*.h
  include/*.inl
  include/entities/*.h
  include/entities/*.inl
  include/hero/*.h
  include/hero/*.inl
  include/hud/*.h
  include/hud/*.inl
  include/lowlevel/*.h
  include/lowlevel/*.inl
  include/lowlevel/shaders*.h
  include/lowlevel/shaders*.inl
  include/lua/*.h
  include/lua/*.inl
  include/menus/*.h
  include/menus/*.inl
  include/movements/*.h
  include/movements/*.inl
  include/containers/*.h
  include/containers/*.inl
  include/third_party/snes_spc/*.h
  include/third_party/hqx/*.h
)
set(main_source_file
  src/main/Main.cpp
)

# Configuration for OSX and iOS build and deployment.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(NOT SOLARUS_IOS_BUILD)
    include(OSX)
  else()
    include(iOS)
  endif()
endif()

# Additional source files for Apple systems.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(source_files
    ${source_files}
    src/lowlevel/apple/AppleInterface.mm
    include/lowlevel/apple/AppleInterface.h
  )
endif()

# Build the Solarus library.
add_library(solarus
  SHARED
  ${source_files}
)
# Add executable target, into CFBundle form if requested or iOS build.
if(SOLARUS_BUNDLE OR SOLARUS_IOS_BUILD)
  include(addCFBundleTarget)
else()
  add_executable(solarus_run
    ${main_source_file}
  )
endif()

# Include paths.
include_directories(
  "${CMAKE_BINARY_DIR}/include"
  "${SOLARUS_ENGINE_SOURCE_DIR}/include"
  "${SOLARUS_ENGINE_SOURCE_DIR}/include/solarus/third_party"
  "${SOLARUS_ENGINE_SOURCE_DIR}/include/solarus/third_party/snes_spc"
)

# External include paths, marked as system ones to disable their warnings.
include_directories(SYSTEM  
  "${MODPLUG_INCLUDE_DIR}"  # Before SDL2 because we want the sndfile.h of ModPlug.
  "${SDL2_INCLUDE_DIR}"
  "${SDL2_TTF_INCLUDE_DIR}"
  "${SDL2_IMAGE_INCLUDE_DIR}"
  "${OPENAL_INCLUDE_DIR}"
  "${VORBISFILE_INCLUDE_DIR}"
  "${OGG_INCLUDE_DIR}"
  "${LUA_INCLUDE_DIR}"
  "${PHYSFS_INCLUDE_DIR}"
)

# Libraries.
target_link_libraries(solarus
  "${SDL2MAIN_LIBRARY}"
  "${SDL2_LIBRARY}"
  "${SDL2_IMAGE_LIBRARY}"
  "${SDL2_TTF_LIBRARY}"
  "${OPENAL_LIBRARY}"
  "${LUA_LIBRARY}"
  "${DL_LIBRARY}"
  "${PHYSFS_LIBRARY}"
  "${VORBISFILE_LIBRARY}"
  "${OGG_LIBRARY}"
  "${MODPLUG_LIBRARY}"
)

target_link_libraries(solarus_run
  solarus
  "${SDL2_LIBRARY}"
  "${SDL2_IMAGE_LIBRARY}"
  "${SDL2_TTF_LIBRARY}"
  "${OPENAL_LIBRARY}"
  "${LUA_LIBRARY}"
  "${DL_LIBRARY}"
  "${PHYSFS_LIBRARY}"
  "${VORBISFILE_LIBRARY}"
  "${OGG_LIBRARY}"
  "${MODPLUG_LIBRARY}"
)

# Files to install with make install.
# install the bundle if requested, or only the library and binary else
if(SOLARUS_BUNDLE)
  install(TARGETS solarus_run
    BUNDLE DESTINATION ${SOLARUS_EXECUTABLE_INSTALL_DESTINATION}
  )
else()
  # Install the shared library and the solarus_run executable.
  install(TARGETS solarus solarus_run
    LIBRARY DESTINATION ${SOLARUS_LIBRARY_INSTALL_DESTINATION}
    RUNTIME DESTINATION ${SOLARUS_EXECUTABLE_INSTALL_DESTINATION}
  )
  # Install headers: useful for projects that use Solarus as a library.
  install(DIRECTORY
    "${CMAKE_BINARY_DIR}/include/solarus"  # For config.h.
    "${SOLARUS_ENGINE_SOURCE_DIR}/include/solarus"
    DESTINATION ${SOLARUS_HEADERS_INSTALL_DESTINATION}
  )
endif()

# Unit tests.
enable_testing()
add_subdirectory(tests)

