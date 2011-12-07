if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(SDL)
find_package(SDL_image)
find_package(SDL_ttf)
find_package(OpenAL)
find_package(VorbisFile)
find_package(ModPlug)
find_package(Lua51)
find_package(PhysFS)

include_directories(
  ${SOLARUS_SOURCE_DIR}/include
  ${SOLARUS_SOURCE_DIR}/include/snes_spc
  ${SDL_INCLUDE_DIR}
  ${OPENAL_INCLUDE_DIR}
  ${VORBISFILE_INCLUDE_DIR}
  ${MODPLUG_INCLUDE_DIR}
  ${LUA_INCLUDE_DIR}
  ${PHYSFS_INCLUDE_DIR}
)

file(
  GLOB_RECURSE
  source_files
  *.cpp
  ../include/*.h
)

set(main_source_file Main.cc)

add_library(solarus_static
  STATIC
  ${source_files}
)

add_executable(solarus
  ${main_source_file}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

if (DEFINED DEFAULT_QUEST)
  add_definitions(-DDEFAULT_QUEST=\"${DEFAULT_QUEST}\")
endif (DEFINED DEFAULT_QUEST)

target_link_libraries(solarus
  solarus_static
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLTTF_LIBRARY}
  ${OPENAL_LIBRARY}
  ${LUA_LIBRARY}
  ${PHYSFS_LIBRARY}
  ${VORBISFILE_LIBRARY}
  ${MODPLUG_LIBRARY}
)

install(TARGETS	solarus
  RUNTIME DESTINATION bin
)

