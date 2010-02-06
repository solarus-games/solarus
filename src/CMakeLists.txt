if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")

find_package(SDL)
find_package(SDL_image)
find_package(SDL_ttf)
find_package(OpenAL)
find_package(Lua51)
find_package(PhysFS)
find_package(Sndfile)

include_directories(
  ${SOLARUS_SOURCE_DIR}/include
  ${SOLARUS_SOURCE_DIR}/include/snes_spc
  ${LUA_INCLUDE_DIR}
)

file(
  GLOB_RECURSE
  source_files
  *.cpp
  ../include/*.h
)

add_executable(solarus
  ${source_files}
)

if(NOT WIN32)
  set_target_properties(solarus PROPERTIES COMPILE_FLAGS "-g -ansi -pedantic -Wall -Werror")
endif(NOT WIN32)

if (DEFINED DATAPATH)
  add_definitions(-DDATAPATH=\"${DATAPATH}\")
endif (DEFINED DATAPATH)

target_link_libraries(solarus
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLTTF_LIBRARY}
  ${OPENAL_LIBRARY}
  ${LUA_LIBRARY}
  ${PHYSFS_LIBRARY}
  ${SNDFILE_LIBRARY}
)

install(TARGETS	solarus
  RUNTIME DESTINATION bin
)

