####
#
# This CMakeList can be included inside the main CMakeList.txt to generate 
# a CFBundle. The engine is build from corresponding ${main_source_file} files. 
# The quest, icon and eventually an info.plist template file can be added to the project.
#
# Since the MACOSX_BUNDLE property only affect TARGET, which are build or imported 
# files by the current project, you have to build the engine if you build
# the Bundle.
# You can edit the Bundle configuration by passing some flags.
# SOLARUS_OSX_BUNDLE represent the Bundle name. It's the only obligatory flag.
# SOLARUS_OSX_BUNDLE_QUEST represent the quest path.
# SOLARUS_OSX_BUNDLE_INFOPLIST represent the info.plist template file path.
# SOLARUS_OSX_BUNDLE_ICON represent the icon path.
#
# There is a default icon and info.plist template provided, but no quest, so if you 
# don't specify a quest path, you'll have to add one manually to your generic bundle.
#
# Exportable to XCode
#
####

set(SOLARUS_BUNDLE_SOURCE_DIR "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/osx/bundle/")
set(EXECUTABLE_NAME "solarus")

# Assertions
if(NOT SOLARUS_OSX_BUNDLE)
  message(FATAL_ERROR "Bundle name not correctly specified. Set a valid SOLARUS_OSX_BUNDLE value")
endif()
if(SOLARUS_OSX_BUNDLE STREQUAL "${EXECUTABLE_NAME}")
  message(FATAL_ERROR "There is already a target named ${EXECUTABLE_NAME}, please choose another one for the Bundle")
endif()
if(NOT SOLARUS_OSX_BUNDLE_QUEST)
  message(WARNING "Creating generic bundle. You should add a quest later")
endif()

# Default files if not specified
if(NOT SOLARUS_OSX_BUNDLE_INFOPLIST)
  set(SOLARUS_OSX_BUNDLE_INFOPLIST 	${SOLARUS_BUNDLE_SOURCE_DIR}/Info.plist)
endif()
if(NOT SOLARUS_OSX_BUNDLE_ICON)
  set(SOLARUS_OSX_BUNDLE_ICON         ${SOLARUS_BUNDLE_SOURCE_DIR}/Solarus.icns)
endif()

# Remove the hardcoded additional link on SDL path
set(SDL_FRAMEWORK 			${SDL_LIBRARY})
string(REPLACE "-framework Cocoa" "" SDL_FRAMEWORK ${SDL_FRAMEWORK}) 

# Specify Bundle files
if(CMAKE_VERSION VERSION_LESS 2.8.11) 
  message(WARNING ".framework embed library will not be correctly copied. See http://public.kitware.com/Bug/view.php?id=13784")
endif()
add_executable(${SOLARUS_OSX_BUNDLE} MACOSX_BUNDLE
		${main_source_file}
		${SOLARUS_OSX_BUNDLE_QUEST}
		${SOLARUS_OSX_BUNDLE_INFOPLIST}
		${SOLARUS_OSX_BUNDLE_ICON} 

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
		${SOLARUS_OSX_BUNDLE_QUEST} 
		${SOLARUS_OSX_BUNDLE_ICON} 
		PROPERTY MACOSX_PACKAGE_LOCATION Resources)
set_property(TARGET ${SOLARUS_OSX_BUNDLE} PROPERTY 
		MACOSX_BUNDLE_INFO_PLIST "${SOLARUS_OSX_BUNDLE_INFOPLIST}")

# Additional Info.plist lines
get_filename_component(ICON_NAME "${SOLARUS_OSX_BUNDLE_ICON}" NAME)
set_property(TARGET ${SOLARUS_OSX_BUNDLE} PROPERTY 
		MACOSX_BUNDLE_BUNDLE_NAME ${SOLARUS_OSX_BUNDLE}
		MACOSX_BUNDLE_ICON_FILE ${ICON_NAME})

# install
install(PROGRAMS	${SOLARUS_OSX_BUNDLE}
  BUNDLE DESTINATION  product/
) 
