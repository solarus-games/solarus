if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

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
  *.cpp
  entities/*.cpp
  hero/*.cpp
  hud/*.cpp
  lowlevel/*.cpp
  lua/*.cpp
  menus/*.cpp
  movements/*.cpp
  snes_spc/*.cpp
  ../include/*.h
  ../include/entities/*.h
  ../include/hero/*.h
  ../include/hud/*.h
  ../include/lowlevel/*.h
  ../include/lua/*.h
  ../include/menus/*.h
  ../include/movements/*.h
  ../include/simpleini/*.h
  ../include/snes_spc/*.h
)

# additional source files for Mac OS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include_directories(
    ${SOLARUS_SOURCE_DIR}/include/lowlevel/osx
  )
  set(source_files
    ${source_files}
    lowlevel/osx/SDLMain.m
    ../include/lowlevel/osx/SDLMain.h
  )
endif()

# build the executable
set(main_source_file lowlevel/Main.cc)
add_library(solarus_static
  STATIC
  ${source_files}
)
add_executable(solarus
  ${main_source_file}
)

# generate -I flags
include_directories(
  ${SOLARUS_SOURCE_DIR}/include
  ${SOLARUS_SOURCE_DIR}/include/snes_spc
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

# compilation flags
set(CMAKE_CXX_FLAGS "-Wall -Werror ${CMAKE_CXX_FLAGS}")

# compilation flags for Mac OS X (Intel) deployment
if(MACOSX_DEPLOYMENT)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${MACOSX_DEPLOYMENT}")
  set(CMAKE_OSX_ARCHITECTURES "i386;x86_64") 
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=${MACOSX_DEPLOYMENT} -Xlinker -rpath -Xlinker @loader_path/../Frameworks/" )

#maybe do an export before running cmake instead of the following line if it doesn't work
  set(ENV{MACOSX_DEPLOYMENT_TARGET} "${MACOSX_DEPLOYMENT}")

endif(MACOSX_DEPLOYMENT)

# solarus options
option(DEBUG_KEYS "Enable the debugging keys." OFF)
if(DEBUG_KEYS)
  add_definitions(-DSOLARUS_DEBUG_KEYS)
endif(DEBUG_KEYS)

if(DEFAULT_QUEST)
  set(DEFAULT_QUEST "." CACHE STRING "Path to the quest to launch if none is specified at runtime.")
  add_definitions(-DDEFAULT_QUEST=\"${DEFAULT_QUEST}\")
endif(DEFAULT_QUEST)

# files to install with make install
install(TARGETS	solarus
  RUNTIME DESTINATION bin
)

