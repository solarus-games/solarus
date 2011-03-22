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
find_package(ModPlug)

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

set(main_source_file Main.cc)

add_library(solarus_static
  STATIC
  ${source_files}
)

add_executable(solarus
  ${main_source_file}
)

set_target_properties(solarus_static PROPERTIES COMPILE_FLAGS "-Wall -Werror")
set_target_properties(solarus PROPERTIES COMPILE_FLAGS "-Wall -Werror")

if (DEFINED DATAPATH)
  add_definitions(-DDATAPATH=\"${DATAPATH}\")
endif (DEFINED DATAPATH)

target_link_libraries(solarus
  solarus_static
  ${SDL_LIBRARY}
  ${SDLIMAGE_LIBRARY}
  ${SDLTTF_LIBRARY}
  ${OPENAL_LIBRARY}
  ${LUA_LIBRARY}
  ${PHYSFS_LIBRARY}
  ${SNDFILE_LIBRARY}
  ${MODPLUG_LIBRARY}
)

install(TARGETS	solarus
  RUNTIME DESTINATION bin
)

