# This CMakeList can be used to generate a CFBundle with the specified
# engine, quest and icon
#
# Uses CPack.
# Exportable to XCode.
#
# For now, the engine and the quest will just be copied if exist.

cmake_minimum_required(VERSION 2.6)
project(SOLARUS_ENGINE)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_MODULE_PATH "${SOLARUS_ENGINE_SOURCE_DIR}/../../modules/")

# find the headers and the libraries of dependencies
find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(OpenAL REQUIRED)
find_package(VorbisFile REQUIRED)
find_package(Ogg REQUIRED)
find_package(ModPlug REQUIRED)
find_package(PhysFS REQUIRED)
#find_package(Lua51 REQUIRED) supposed to be statically linked on OSX

if(NOT BUNDLE_NAME)
  message(FATAL_ERROR "Bundle name not specified")
endif()
if(NOT QUEST_PATH)
  message(FATAL_ERROR "Quest path not specified")
endif()

# Default files, if not specified
if(NOT ENGINE_PATH)
  set(ENGINE_PATH "/usr/local/bin/solarus")
endif()
if(NOT INFOPLIST_PATH)
  set(INFOPLIST_PATH 	${SOLARUS_ENGINE_SOURCE_DIR}/Info.plist)
endif()
if(NOT ICON_PATH)
  set(ICON_PATH 	${SOLARUS_ENGINE_SOURCE_DIR}/Solarus.icns)
endif()

# Remove the hardcoded additional link on SDL path
set(SDL_FRAMEWORK 			${SDL_LIBRARY})
string(REPLACE "-framework Cocoa" "" SDL_FRAMEWORK ${SDL_FRAMEWORK}) 

# Specify Bundle files
if(CMAKE_VERSION VERSION_LESS 2.8.11) 
  message(WARNING ".framework embed library will not be correctly copied. See http://public.kitware.com/Bug/view.php?id=13784")
endif()

INCLUDE(InstallRequiredSystemLibraries)

# Info.plist construction from INFOPLIST_PATH template
set(CPACK_BUNDLE_PLIST ${INFOPLIST_PATH})
set(CPACK_BUNDLE_ICON ${ICON_PATH})
set(CPACK_BUNDLE_NAME ${BUNDLE_NAME})
set(CPACK_BUNDLE_STARTUP_COMMAND ${ENGINE_PATH})

set(CPACK_PACKAGE_ICON ${ICON_PATH})
set(CPACK_PACKAGE_FILE_NAME ${BUNDLE_NAME})

INCLUDE(CPackBundle)

#install(PROGRAMS ${QUEST_PATH})
#install(FILE ${ICON_PATH})