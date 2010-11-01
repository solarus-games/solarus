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

if (DEFINED DATAPATH)
  add_definitions(-DDATAPATH=\"${DATAPATH}\")
endif (DEFINED DATAPATH)

file(
  GLOB_RECURSE
  src_files
  ../src/*.cpp
  ../include/*.h
)

enable_testing()

# each .cpp file of the 'tests' directory is a test with a main() function
add_definitions(-DSOLARUS_NOMAIN)
file(
  GLOB
  tests_main_files
  *.cpp
)

foreach(test_main_file ${tests_main_files})

  get_filename_component(test_bin_file ${test_main_file} NAME_WE)
  set(test_src_files ${src_files})
  add_executable(${test_bin_file} ${test_src_files} ${test_main_file})
  set(tests_bin_files ${tests_bin_files} ${test_bin_file})

  if(NOT WIN32)
    set_target_properties(${test_bin_file} PROPERTIES COMPILE_FLAGS "-g -ansi -pedantic -Wall -Werror")
  endif(NOT WIN32)

  target_link_libraries(${test_bin_file}
    ${SDL_LIBRARY}
    ${SDLIMAGE_LIBRARY}
    ${SDLTTF_LIBRARY}
    ${OPENAL_LIBRARY}
    ${LUA_LIBRARY}
    ${PHYSFS_LIBRARY}
    ${SNDFILE_LIBRARY}
  )

  add_test(${test_main_file} ${EXECUTABLE_OUTPUT_PATH}/${test_bin_file} -no-audio -no-video)

endforeach(test_main_file ${test_main_files})

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${test_bin_files})

