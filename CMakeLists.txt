cmake_minimum_required(VERSION 2.6)
project(SOLARUS)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/modules/")


find_package(SDL)
find_package(SDL_image)
find_package(SDL_ttf)
find_package(OpenAL)
find_package(Lua51)
find_package(PhysFS)
find_package(Sndfile)

add_subdirectory(src)
add_subdirectory(quests/zsdx)

