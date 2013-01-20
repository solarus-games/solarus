####
#
# This module can be included inside the main CMakeList.txt to add a
# target which generate the CFBundle when building the engine. 
# The engine is build from ${main_source_file} file, and solarus and solarus_static target. 
# The quest, icon and an info.plist template file can be added to the project.
#
# You can edit the Bundle configuration by passing some flags.
# SOLARUS_BUNDLE represent the Bundle name. It's the only obligatory flag.
# SOLARUS_BUNDLE_QUEST represent the quest path.
# SOLARUS_BUNDLE_INFOPLIST represent the info.plist template file path.
# SOLARUS_BUNDLE_ICON represent the icon path.
# SOLARUS_BUNDLE_VERSION represent the version string
#
# There is a default icon and info.plist template provided, but no quest, so if you 
# don't specify a quest path, you'll have to add one manually to your generic bundle.
#
# If ${SOLARUS_IOS_BUILD} is defined, an IPone Bundle will be created.
#
# Exportable to XCode.
#
####

# Assertions
if(NOT SOLARUS_BUNDLE)
  message(FATAL_ERROR "Bundle name not correctly specified. Set a valid SOLARUS_BUNDLE value")
endif()
if(NOT SOLARUS_BUNDLE_QUEST)
  message(STATUS "Creating generic bundle. You should add a quest later")
endif()

# Configuration variable
set(EXECUTABLE_NAME                   "solarus")
set(EXECUTABLE_STATIC_NAME            "solarus_static")
set(COMPANY_IDENTIFIER                "${EXECUTABLE_NAME}-team")

# OS-specific configuration variable
if(SOLARUS_IOS_BUILD)
  set(SOLARUS_OS_SOFTWARE             "iOS")
  set(SOLARUS_OS_HARDWARE             "IPhone")
  set(SOLARUS_BUNDLE_DESTINATION      "usr/share")
  set(SOLARUS_BUNDLE_COPIED_LIBRARIES "")
else()
  set(SOLARUS_OS_SOFTWARE             "OSX")
  set(SOLARUS_OS_HARDWARE             "MacOSX")
  set(SOLARUS_BUNDLE_DESTINATION      "/Applications")

  # Remove the hardcoded additional link on SDL path
  string(REPLACE "-framework Cocoa" "" SDL_FRAMEWORK "${SDL_LIBRARY}") 

  set(SOLARUS_BUNDLE_COPIED_LIBRARIES
    ${SDL_FRAMEWORK} 
    ${SDLIMAGE_LIBRARY} 
    ${SDLTTF_LIBRARY}
    ${VORBISFILE_LIBRARY} 
    ${OGG_LIBRARY} 
    ${PHYSFS_LIBRARY} 
    ${MODPLUG_LIBRARY}
  )
endif()

# Default files if not specified
if(NOT SOLARUS_BUNDLE_INFOPLIST)
  set(SOLARUS_BUNDLE_INFOPLIST      "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/apple/${SOLARUS_OS_SOFTWARE}-Info.plist")
endif()
if(NOT SOLARUS_BUNDLE_ICON)
  set(SOLARUS_BUNDLE_ICON           "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/icons/Solarus.icns")
endif()
if(NOT SOLARUS_BUNDLE_VERSION)
  set(SOLARUS_BUNDLE_VERSION        "1.0")
endif()

# Specify Bundle files
add_executable(${SOLARUS_BUNDLE} MACOSX_BUNDLE
  ${main_source_file}
  ${SOLARUS_BUNDLE_QUEST}
  ${SOLARUS_BUNDLE_INFOPLIST}
  ${SOLARUS_BUNDLE_ICON} 
  ${SOLARUS_BUNDLE_COPIED_LIBRARIES}
)

# Regenerate -l flags for the Bundle target
target_link_libraries(${SOLARUS_BUNDLE}
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

# Set right properties on copied files
set_property(SOURCE 
  ${SOLARUS_BUNDLE_QUEST} 
  ${SOLARUS_BUNDLE_ICON} 
  PROPERTY MACOSX_PACKAGE_LOCATION Resources
)

# Workaround : copy libraries with add_custom_command() for Makefile Generator.
# TODO : Remove when http://public.kitware.com/Bug/view.php?id=13784 will be accepted.
macro(copy_into_bundle target library_path destination_directory)
  if(IS_DIRECTORY ${library_path})
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND cp 
      ARGS -R -L -n ${library_path} "${PROJECT_BINARY_DIR}/${target}.app/Contents/${destination_directory}/"
    )
  else()
    add_custom_command(
      TARGET ${target}
      POST_BUILD
      COMMAND cp 
      ARGS -n ${library_path} "${PROJECT_BINARY_DIR}/${target}.app/Contents/${destination_directory}/"
    )	
  endif()
endmacro()
if(NOT XCODE)
  if(NOT SOLARUS_IOS_BUILD)
    add_custom_command(TARGET ${SOLARUS_BUNDLE} POST_BUILD COMMAND mkdir ARGS -p "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/Frameworks")
  endif()
  foreach(LIB ${SOLARUS_BUNDLE_COPIED_LIBRARIES})
    copy_into_bundle(${SOLARUS_BUNDLE} ${LIB} Frameworks)
  endforeach()
  
# Proper way, buggy with Makefile Generator for now.
else()
  foreach(LIB ${SOLARUS_BUNDLE_COPIED_LIBRARIES})
    set_source_properties(${LIB} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
  endforeach()
endif()

# Info.plist template and additional lines
get_filename_component(SOLARUS_BUNDLE_ICON_NAME "${SOLARUS_BUNDLE_ICON}" NAME)
set_target_properties(${SOLARUS_BUNDLE} PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST             "${SOLARUS_BUNDLE_INFOPLIST}"

		MACOSX_BUNDLE_BUNDLE_NAME            ${SOLARUS_BUNDLE}
		MACOSX_BUNDLE_ICON_FILE              ${SOLARUS_BUNDLE_ICON_NAME}
		MACOSX_BUNDLE_BUNDLE_VERSION         ${SOLARUS_BUNDLE_VERSION}

		MACOSX_BUNDLE_GUI_IDENTIFIER         "${COMPANY_IDENTIFIER}.${MACOSX_BUNDLE_BUNDLE_NAME}"
		MACOSX_BUNDLE_SHORT_VERSION_STRING   "${MACOSX_BUNDLE_BUNDLE_VERSION}"
		MACOSX_BUNDLE_LONG_VERSION_STRING    "${MACOSX_BUNDLE_BUNDLE_NAME} Version ${MACOSX_BUNDLE_SHORT_VERSION_STRING}"
		MACOSX_BUNDLE_COPYRIGHT              "Copyright 2013, ${COMPANY_IDENTIFIER}."
		MACOSX_BUNDLE_INFO_STRING            "${MACOSX_BUNDLE_LONG_VERSION_STRING}, ${MACOSX_BUNDLE_COPYRIGHT}"
)

# Embed library search path
if(NOT SOLARUS_IOS_BUILD)
  if(NOT CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS "10.5")
    set(CMAKE_EXE_LINKER_FLAGS         "${CMAKE_EXE_LINKER_FLAGS} -Xlinker -rpath -Xlinker @loader_path/../Frameworks/" CACHE STRING "Embed frameworks search path" FORCE)
    set(SOLARUS_RPATH                  "@rpath/")
  else()
    set(SOLARUS_RPATH                  "@executable_path/../Frameworks/")
  endif()
  set_target_properties(${SOLARUS_BUNDLE} PROPERTIES 
    BUILD_WITH_INSTALL_RPATH           1 
    INSTALL_NAME_DIR                   ${SOLARUS_RPATH}	
  )
endif()

# Use the bundle's resources path for the bundle's executable
if(DEFAULT_QUEST)
  remove_definitions(-DSOLARUS_DEFAULT_QUEST=\"${DEFAULT_QUEST}\")
  unset(DEFAULT_QUEST CACHE)
endif()
set(DEFAULT_QUEST "../Resources" CACHE STRING "Path to the quest to launch with a bundle" FORCE)
add_definitions(-DSOLARUS_DEFAULT_QUEST=\"../Resources\")

# Code signing
if(XCODE)
  set_target_properties(${NAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${SOLARUS_OS_HARDWARE} Developer: ${COMPANY_IDENTIFIER}")
endif()

# install
install(PROGRAMS                     ${SOLARUS_BUNDLE}
  BUNDLE DESTINATION                 ${SOLARUS_BUNDLE_DESTINATION}
)
