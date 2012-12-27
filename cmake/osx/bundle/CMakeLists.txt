####
#
# This CMakeList can be included inside the main CMakeList.txt to generate 
# a CFBundle. The engine is build from corresponding ${main_source_file} files. 
# The quest, icon and eventually an info.plist template file can be added to the project.
#
# Since the MACOSX_BUNDLE only affect TARGET, which are build or imported 
# files by the current project, you have to build the engine if you build
# the Bundle.
# You can add a quest to the project with QUEST_PATH flag, or let it empty 
# to create a generic solarus Bundle, and add a quest later into it.
# A default icon and info.plist template are provided.
#
# Exportable to XCode
#
####

set(SOLARUS_BUNDLE_SOURCE_DIR "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/osx/bundle/")

# Assertions
set(EXECUTABLE_NAME "solarus")
if(NOT BUNDLE_NAME)
  message(FATAL_ERROR "Bundle name not specified")
endif()
if(BUNDLE_NAME STREQUAL "${EXECUTABLE_NAME}")
  message(FATAL_ERROR "There is already a target named ${EXECUTABLE_NAME}, please choose another one for the Bundle")
endif()
if(NOT QUEST_PATH)
  message(WARNING "Creating generic bundle. You should add a quest later")
endif()

# Default files if not specified
if(NOT INFOPLIST_PATH)
  set(INFOPLIST_PATH 	${SOLARUS_BUNDLE_SOURCE_DIR}/Info.plist)
endif()
if(NOT ICON_PATH)
  set(ICON_PATH         ${SOLARUS_BUNDLE_SOURCE_DIR}/Solarus.icns)
endif()

# Remove the hardcoded additional link on SDL path
set(SDL_FRAMEWORK 			${SDL_LIBRARY})
string(REPLACE "-framework Cocoa" "" SDL_FRAMEWORK ${SDL_FRAMEWORK}) 

# Specify Bundle files
if(CMAKE_VERSION VERSION_LESS 2.8.11) 
  message(WARNING ".framework embed library will not be correctly copied. See http://public.kitware.com/Bug/view.php?id=13784")
endif()
add_executable(${BUNDLE_NAME} MACOSX_BUNDLE
		${main_source_file}
		${ICON_PATH} 
		${QUEST_PATH}

		${SDL_FRAMEWORK} 
		${SDLIMAGE_LIBRARY} 
		${SDLTTF_LIBRARY} 
		${VORBISFILE_LIBRARY} 
		${OGG_LIBRARY} 
		${PHYSFS_LIBRARY} 
		${MODPLUG_LIBRARY}
)

# Set right properties on copied files
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
		${QUEST_PATH} 
		${ICON_PATH} 
		PROPERTY MACOSX_PACKAGE_LOCATION Resources)
set_property(TARGET ${EXECUTABLE_TARGET} PROPERTY 
		MACOSX_BUNDLE_INFO_PLIST "${INFOPLIST_PATH}")

# Additional Info.plist lines
get_filename_component(ICON_NAME "${ICON_PATH}" NAME)
set_property(TARGET ${BUNDLE_NAME} PROPERTY 
		MACOSX_BUNDLE_BUNDLE_NAME ${BUNDLE_NAME}
		MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})

# install
install(PROGRAMS	${BUNDLE_NAME}
  BUNDLE DESTINATION  product/
) 
