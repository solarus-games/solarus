####
#
# This CMakeList can be included inside the main CMakeList.txt to add a
# target which generate the CFBundle when building the engine. 
# The engine is build from corresponding ${main_source_file} files. 
# The quest, icon and an info.plist template file can be added to the project.
#
# You can edit the Bundle configuration by passing some flags.
# SOLARUS_OSX_BUNDLE represent the Bundle name. It's the only obligatory flag.
# SOLARUS_OSX_BUNDLE_QUEST represent the quest path.
# SOLARUS_OSX_BUNDLE_INFOPLIST represent the info.plist template file path.
# SOLARUS_OSX_BUNDLE_ICON represent the icon path.
# SOLARUS_OSX_BUNDLE_VERSION represent the version string
#
# There is a default icon and info.plist template provided, but no quest, so if you 
# don't specify a quest path, you'll have to add one manually to your generic bundle.
#
# Exportable to XCode.
#
####

set(EXECUTABLE_NAME                     "solarus")
set(EXECUTABLE_STATIC_NAME              "${EXECUTABLE_NAME}_static")
set(COMPANY_IDENTIFIER                  "${EXECUTABLE_NAME}-team")

set(SOLARUS_BUNDLE_SOURCE_DIR "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/osx/bundle/")

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
  set(SOLARUS_OSX_BUNDLE_INFOPLIST      ${SOLARUS_BUNDLE_SOURCE_DIR}/Info.plist)
endif()
if(NOT SOLARUS_OSX_BUNDLE_ICON)
  set(SOLARUS_OSX_BUNDLE_ICON           ${SOLARUS_BUNDLE_SOURCE_DIR}/Solarus.icns)
endif()
if(NOT SOLARUS_OSX_BUNDLE_VERSION)
  set(SOLARUS_OSX_BUNDLE_VERSION        "1.0")
endif()

# Regenerate -l flags for the Bundle target
target_link_libraries(${SOLARUS_OSX_BUNDLE}
  ${EXECUTABLE_STATIC_NAME}
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

# Remove the hardcoded additional link on SDL path
string(REPLACE "-framework Cocoa" "" SDL_FRAMEWORK "${SDL_LIBRARY}") 

# Specify Bundle files
if(CMAKE_VERSION VERSION_LESS 2.8.11) 
  message(WARNING ".framework embed library will not be correctly copied with the Makefile Generator. See http://public.kitware.com/Bug/view.php?id=13784")
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
		PROPERTY MACOSX_PACKAGE_LOCATION Frameworks
)
set_property(SOURCE 
		${SOLARUS_OSX_BUNDLE_QUEST} 
		${SOLARUS_OSX_BUNDLE_ICON} 
		PROPERTY MACOSX_PACKAGE_LOCATION Resources
)

# Info.plist template and additional lines
get_filename_component(SOLARUS_OSX_BUNDLE_ICON_NAME "${SOLARUS_OSX_BUNDLE_ICON}" NAME)
set_target_properties(${SOLARUS_OSX_BUNDLE} PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST             "${SOLARUS_OSX_BUNDLE_INFOPLIST}"

		MACOSX_BUNDLE_BUNDLE_NAME            ${SOLARUS_OSX_BUNDLE}
		MACOSX_BUNDLE_ICON_FILE              ${SOLARUS_OSX_BUNDLE_ICON_NAME}
		MACOSX_BUNDLE_BUNDLE_VERSION         ${SOLARUS_OSX_BUNDLE_VERSION}

		MACOSX_BUNDLE_GUI_IDENTIFIER         "${COMPANY_IDENTIFIER}.${MACOSX_BUNDLE_BUNDLE_NAME}"
		MACOSX_BUNDLE_SHORT_VERSION_STRING   "${MACOSX_BUNDLE_BUNDLE_VERSION}"
		MACOSX_BUNDLE_LONG_VERSION_STRING    "${MACOSX_BUNDLE_BUNDLE_NAME} Version ${MACOSX_BUNDLE_SHORT_VERSION_STRING}"
		MACOSX_BUNDLE_COPYRIGHT              "Copyright 2013, ${COMPANY_IDENTIFIER}."
		MACOSX_BUNDLE_INFO_STRING            "${MACOSX_BUNDLE_LONG_VERSION_STRING}, ${MACOSX_BUNDLE_COPYRIGHT}"
)

# Embed library search path
if(NOT CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS "10.5")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Xlinker -rpath -Xlinker @loader_path/../Frameworks/" CACHE STRING "Embed frameworks search path" FORCE)
  set(SOLARUS_OSX_RPATH "@rpath/")
else()
  set(SOLARUS_OSX_RPATH "@executable_path/../Frameworks/")
endif()
set_target_properties(${SOLARUS_OSX_BUNDLE} PROPERTIES 
	BUILD_WITH_INSTALL_RPATH 1 
	INSTALL_NAME_DIR ${SOLARUS_OSX_RPATH}	
)

# CFBundle default quest directory.
set(DEFAULT_QUEST "../Resources" CACHE STRING "Default quest directory")

# install
install(PROGRAMS                          ${SOLARUS_OSX_BUNDLE}
        BUNDLE DESTINATION                share/solarus/
) 
