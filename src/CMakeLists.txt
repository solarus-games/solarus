if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(SDL)
find_package(SDL_image)
find_package(SDL_ttf)
find_package(OpenAL)
find_package(VorbisFile)
find_package(Ogg)
find_package(ModPlug)
find_package(Lua51)
find_package(PhysFS)

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

set(main_source_file lowlevel/Main.cc)

add_library(solarus_static
  STATIC
  ${source_files}
)

add_executable(solarus
  ${main_source_file}
)

# compilation options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic")

# compilation options for Mac OS X (10.5+ Intel) deployment
if(MACOS_DEPLOYMENT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmacosx-version-min=10.5 -arch i386 -arch x86_64 -force_cpusubtype_ALL")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=10.5 -arch i386 -arch x86_64 -Xlinker -rpath -Xlinker @loader_path/../Frameworks/ )
  set(ENV{MACOS_DEPLOYMENT_TARGET} 10.5)
endif(MACOS_DEPLOYMENT)

option(DEBUG_KEYS "Enable the debugging keys." OFF)

if(DEBUG_KEYS)
  add_definitions(-DSOLARUS_DEBUG_KEYS)
endif(DEBUG_KEYS)

if(DEFAULT_QUEST)
  set(DEFAULT_QUEST "." CACHE STRING "Path to the quest to launch if none is specified at runtime.")
  add_definitions(-DDEFAULT_QUEST=\"${DEFAULT_QUEST}\")
endif(DEFAULT_QUEST)

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

install(TARGETS	solarus
  RUNTIME DESTINATION bin
)

