cmake_minimum_required(VERSION 2.6)
project(SOLARUS_ENGINE)

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif()

set(CMAKE_MODULE_PATH "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/modules/")

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
  src/*.cpp
  src/entities/*.cpp
  src/hero/*.cpp
  src/hud/*.cpp
  src/lowlevel/*.cpp
  src/lua/*.cpp
  src/menus/*.cpp
  src/movements/*.cpp
  src/snes_spc/*.cpp
  include/*.h
  include/entities/*.h
  include/hero/*.h
  include/hud/*.h
  include/lowlevel/*.h
  include/lua/*.h
  include/menus/*.h
  include/movements/*.h
  include/simpleini/*.h
  include/snes_spc/*.h
)

# additional source files for Mac OS
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  include_directories(
    ${SOLARUS_ENGINE_SOURCE_DIR}/include/lowlevel/osx
  )
  set(source_files
    ${source_files}
    src/lowlevel/osx/SDLMain.m
    src/lowlevel/osx/OSXInterface.mm
    include/lowlevel/osx/SDLMain.h
    include/lowlevel/osx/OSXInterface.h
  )
endif()

# build the executable
set(main_source_file src/lowlevel/Main.cc)
add_library(solarus_static
  STATIC
  ${source_files}
)

if(NOT SOLARUS_OSX_BUNDLE)
  add_executable(solarus
    ${main_source_file}
  )
else()
  # OSX Bundle only works with Release mode
  if(NOT ${CMAKE_BUILD_TYPE} MATCHES "Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Bundle only works with Release mode" FORCE)
  endif()

  # Build location, need to be accessible with user privilege
  set(CMAKE_INSTALL_PREFIX 		${SOLARUS_ENGINE_SOURCE_DIR}/)

  # Path to default files
  if(NOT SOLARUS_OSX_QUEST)
    set(SOLARUS_OSX_QUEST 		${SOLARUS_ENGINE_SOURCE_DIR}/quests/zsdx/data.solarus)
  endif()  
  if(NOT SOLARUS_OSX_ICON)
    set(SOLARUS_OSX_ICON 		${SOLARUS_ENGINE_SOURCE_DIR}/osx/Solarus.icns)
  endif()
  if(NOT SOLARUS_OSX_INFOPLIST)
    set(SOLARUS_OSX_INFOPLIST 		${SOLARUS_ENGINE_SOURCE_DIR}/osx/Info.plist)
  endif()

  # Path to quest
  get_filename_component(SOLARUS_OSX_QUEST_NAME "${SOLARUS_OSX_QUEST_NAME}" NAME)
  set(DEFAULT_QUEST "../Resources/${SOLARUS_OSX_QUEST_NAME}" CACHE STRING "Default quest launched")

  # Remove the hardcoded additional link on SDL path
  set(SDL_FRAMEWORK 			${SDL_LIBRARY})
  string(REPLACE "-framework Cocoa" "" SDL_FRAMEWORK ${SDL_FRAMEWORK}) 

  # Specify Bundle files
  add_executable(solarus
    MACOSX_BUNDLE
    ${main_source_file}
    ${SDL_FRAMEWORK}
    ${SDLIMAGE_LIBRARY}
    ${SDLTTF_LIBRARY}
    ${VORBISFILE_LIBRARY}
    ${OGG_LIBRARY}
    ${PHYSFS_LIBRARY}
    ${MODPLUG_LIBRARY}
    ${SOLARUS_OSX_QUEST}
    ${SOLARUS_OSX_ICON}
    ${SOLARUS_OSX_INFOPLIST}
  )

  # Set right properties on copied files
  if(CMAKE_VERSION VERSION_LESS 2.8.10.3) 
    message(WARNING ".framework embed library will not be correctly copied. See http://public.kitware.com/Bug/view.php?id=13784")
  endif()
  set_property(SOURCE 
		${SDL_FRAMEWORK} 
		${SDLIMAGE_LIBRARY} 
		${SDLTTF_LIBRARY} 
		${VORBISFILE_LIBRARY} 
		${OGG_LIBRARY} 
		${PHYSFS_LIBRARY} 
		${MODPLUG_LIBRARY} 
		PROPERTY MACOSX_PACKAGE_LOCATION Frameworks)
  set_property(SOURCE 
		${SOLARUS_OSX_QUEST} 
		${SOLARUS_OSX_ICON} 
		PROPERTY MACOSX_PACKAGE_LOCATION Resources)
  set_property(TARGET solarus 
		PROPERTY MACOSX_BUNDLE_INFO_PLIST ${SOLARUS_OSX_INFOPLIST})
endif()

# generate -I flags
include_directories(
  ${SOLARUS_ENGINE_SOURCE_DIR}/include
  ${SOLARUS_ENGINE_SOURCE_DIR}/include/snes_spc
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

# default compilation flags
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()
set(CMAKE_CXX_FLAGS "-Wall ${CMAKE_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "-Werror ${CMAKE_CXX_FLAGS_DEBUG}")

# Compilation flags for Mac OS X deployment
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" AND ${CMAKE_BUILD_TYPE} MATCHES "Release")
   # Build architectures
  if(SOLARUS_ARCH)
    set(CMAKE_OSX_ARCHITECTURES "${SOLARUS_ARCH}" CACHE STRING "Chosen build architecture" FORCE)
  else()
    set(CMAKE_OSX_ARCHITECTURES "i386;x86_64;" CACHE STRING "Intel build architecture" FORCE)
  endif()

  # SDK version
  if(SOLARUS_SYSROOT)
    set(CMAKE_OSX_SYSROOT "/Developer/SDKs/MacOSX${SOLARUS_SYSROOT}.sdk" CACHE STRING "Chosen OS version whose headers you'll use" FORCE)
    if(SOLARUS_SYSROOT VERSION_LESS 10.6)
      add_definitions(-DSOLARUS_DONT_USE_OSX_SANDBOX) 
    endif()
  endif()

  # Deployment version
  if(SOLARUS_DEPLOYMENT)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "${SOLARUS_DEPLOYMENT}" CACHE STRING "Chosen oldest OS version supported" FORCE)
  else()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.6" CACHE STRING "Default oldest OS version supported" FORCE)
  endif()

  # Embed library search path
  if(NOT CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS "10.5")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Xlinker -rpath -Xlinker @loader_path/../Frameworks/" CACHE STRING "Embed frameworks search path" FORCE)
    set(SOLARUS_OSX_RPATH "@rpath/")
  else()
    set(SOLARUS_OSX_RPATH "@executable_path/../Frameworks/")
  endif()
  set_target_properties(solarus PROPERTIES 
			BUILD_WITH_INSTALL_RPATH 1 
			INSTALL_NAME_DIR ${SOLARUS_OSX_RPATH}
  )

 # Compatiblity settings
  if(CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS "10.6")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-no_compact_linkedit" CACHE STRING "Disable 10.6+ features if deploy on older version" FORCE)
  endif()
endif()

# solarus options
option(DEBUG_KEYS "Enable the debugging keys." OFF)
if(DEBUG_KEYS)
  add_definitions(-DSOLARUS_DEBUG_KEYS)
endif()

set(DEFAULT_QUEST "." CACHE STRING "Path to the quest to launch if none is specified at runtime.")
if(DEFAULT_QUEST)
  add_definitions(-DSOLARUS_DEFAULT_QUEST=\"${DEFAULT_QUEST}\")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DSOLARUS_USE_APPLE_PREFIXED_WRITE_DIR)
  set(DEFAULT_WRITE_DIR "Solarus")
else()
  set(DEFAULT_WRITE_DIR ".solarus")
endif()
set(WRITE_DIR ${DEFAULT_WRITE_DIR} CACHE STRING "Directory where Solarus savegames are stored, relative to the user's home directory.")
if(WRITE_DIR)
  add_definitions(-DSOLARUS_WRITE_DIR=\"${WRITE_DIR}\")
endif()

if(PANDORA)
  set(DEFAULT_SCREEN_WIDTH 400)
else()
  set(DEFAULT_SCREEN_WIDTH 320)
endif()
set(SCREEN_WIDTH ${DEFAULT_SCREEN_WIDTH} CACHE INTEGER "Width of the logical screen in pixels.")
set(SCREEN_HEIGHT 240 CACHE INTEGER "Height of the logical screen in pixels.")
if(SCREEN_WIDTH)
  add_definitions(-DSOLARUS_SCREEN_WIDTH=${SCREEN_WIDTH})
endif()
if(SCREEN_HEIGHT)
  add_definitions(-DSOLARUS_SCREEN_HEIGHT=${SCREEN_HEIGHT})
endif()

if(PANDORA OR CAANOO)
  set(DEFAULT_COLOR_DEPTH 16)
else()
  set(DEFAULT_COLOR_DEPTH 32)
endif()
set(COLOR_DEPTH ${DEFAULT_COLOR_DEPTH} CACHE INTEGER "Number of bits per pixel for surfaces.")
if(COLOR_DEPTH)
  add_definitions(-DSOLARUS_COLOR_DEPTH=${COLOR_DEPTH})
endif()

if(CAANOO)
  set(DEFAULT_SCREEN_FORCE_MODE 2)
elseif(PANDORA)
  set(DEFAULT_SCREEN_FORCE_MODE 5)
else()
  set(DEFAULT_SCREEN_FORCE_MODE -1)
endif()
set(SCREEN_FORCE_MODE ${DEFAULT_SCREEN_FORCE_MODE} CACHE INTEGER "Video mode to force (-1 means no restriction).")
if(SCREEN_FORCE_MODE)
  add_definitions(-DSOLARUS_SCREEN_FORCE_MODE=${SCREEN_FORCE_MODE})
endif()

# files to install with make install
if(NOT SOLARUS_OSX_BUNDLE)
  install(TARGETS	solarus
    RUNTIME DESTINATION bin
  ) 
else()
  install(TARGETS	solarus
    BUNDLE DESTINATION  osx
  )  
endif()

