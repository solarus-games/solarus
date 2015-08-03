####
# This module can be included inside the main CMakeList.txt to add a
# target which generate the CFBundle when building the engine. 
# The bundle is build as 'solarus' target and from ${source_files}. 
# A custom icon and info.plist template file can be added to the project.
#
# You can edit the bundle configuration by passing some flags.
# SOLARUS_BUNDLE represent the bundle name. It's the only obligatory flag.
# SOLARUS_BUNDLE_INFOPLIST represent the info.plist template file path.
# SOLARUS_BUNDLE_ICON represent the icon path.
# SOLARUS_BUNDLE_CODESIGN represent the codesign identity.
# SOLARUS_BUNDLE_VERSION represent the version string
#
# If SOLARUS_IOS_BUILD is defined, an iPhone bundle will be created.
#
# Some options set to the cache also need the FORCE parameter for obscure reasons.
#
# Exportable to XCode.
####

# Assertions
if(NOT SOLARUS_BUNDLE)
  message(FATAL_ERROR "Bundle name not correctly specified. Set a valid SOLARUS_BUNDLE value")
endif()

# Configuration variable
set(SOLARUS_BUNDLE_TARGET_NAME        "solarus_run")
set(SOLARUS_LIBRARY                   "${CMAKE_SHARED_LIBRARY_PREFIX}solarus${CMAKE_SHARED_LIBRARY_SUFFIX}")
if(SOLARUS_BUNDLE_CODESIGN)
  set(SOLARUS_BUNDLE_GUI_IDENTIFIER   "${SOLARUS_BUNDLE_CODESIGN}")
else()
  set(SOLARUS_BUNDLE_GUI_IDENTIFIER   "${SOLARUS_BUNDLE_TARGET_NAME}-team")
endif()

# OS-specific configuration variable
if(SOLARUS_IOS_BUILD)
  set(SOLARUS_OS_NAME                 "iOS")
  set(SOLARUS_INSTALL_DESTINATION     "local/share")
  if(NOT SOLARUS_BUNDLE_INFOPLIST)
    set(SOLARUS_BUNDLE_INFOPLIST      "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/apple/bundle_content/iOS-Info.plist")
  endif()
  set(SOLARUS_BUNDLE_COPIED_LIBRARIES "")

else()
  set(SOLARUS_OS_NAME                 "OSX")
  set(SOLARUS_INSTALL_DESTINATION     "/Applications")
  if(NOT SOLARUS_BUNDLE_INFOPLIST)
    set(SOLARUS_BUNDLE_INFOPLIST      "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/apple/bundle_content/OSX-Info.plist")
  endif()

  # Remove the hardcoded additional link on SDL2 path
  string(REPLACE "-framework Cocoa" "" SDL2_FRAMEWORK "${SDL2_LIBRARY}")

  # Find libraries to copy into the bundle
  macro(add_to_copied_libraries library_path)
    # Assuming that static libraries never match ".framework" or ".dylib" strings
    if(${library_path} MATCHES ".framework" OR ${library_path} MATCHES ".dylib")
      set(SOLARUS_BUNDLE_COPIED_LIBRARIES ${SOLARUS_BUNDLE_COPIED_LIBRARIES} ${library_path})
    endif()
  endmacro()

  add_to_copied_libraries(${SOLARUS_LIBRARY})
  add_to_copied_libraries(${SDL2_FRAMEWORK})
  add_to_copied_libraries(${SDL2_IMAGE_LIBRARY})
  add_to_copied_libraries(${SDL2_TTF_LIBRARY})
  add_to_copied_libraries(${VORBISFILE_LIBRARY})
  add_to_copied_libraries(${OGG_LIBRARY})
  add_to_copied_libraries(${PHYSFS_LIBRARY})
  add_to_copied_libraries(${MODPLUG_LIBRARY})
  add_to_copied_libraries(${LUA_LIBRARY})
endif()

# Default files if not specified
if(NOT SOLARUS_BUNDLE_ICON)
  set(SOLARUS_BUNDLE_ICON           "${SOLARUS_ENGINE_SOURCE_DIR}/cmake/icons/Solarus.icns")
endif()
if(NOT SOLARUS_BUNDLE_VERSION)
  set(SOLARUS_BUNDLE_VERSION        "1.0")
endif()

# Add executable target into CFBundle form and rename it as requested
add_executable(${SOLARUS_BUNDLE_TARGET_NAME} MACOSX_BUNDLE
  ${source_files}
  ${SOLARUS_BUNDLE_ICON} 
  ${SOLARUS_BUNDLE_COPIED_LIBRARIES}
)
set_target_properties(${SOLARUS_BUNDLE_TARGET_NAME} PROPERTIES
  OUTPUT_NAME   "${SOLARUS_BUNDLE}"
)

# Set right properties on copied files
set_property(SOURCE
  ${SOLARUS_BUNDLE_ICON} 
  PROPERTY MACOSX_PACKAGE_LOCATION Resources
)

# Workaround : copy libraries with add_custom_command() for Makefile Generator.
# TODO : Remove when http://public.kitware.com/Bug/view.php?id=13784 will be accepted.
macro(copy_into_bundle library_path destination_directory)
get_filename_component(library_name ${library_path} NAME)
  # Encapsulate cp with a condition instead of using -n flag, which is buggy on some OSX versions
  if(IS_DIRECTORY ${library_path})
    add_custom_command(
      TARGET ${SOLARUS_BUNDLE_TARGET_NAME}
      POST_BUILD
      COMMAND if 
      ARGS [ ! -d "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/${destination_directory}/${library_name}" ] \; then cp -a "${library_path}" "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/${destination_directory}/" \; fi
    )
  else()
    # Get original name if it is a symbolic link
    execute_process(COMMAND readlink ${library_path}
      OUTPUT_VARIABLE ORIGINAL_NAME
      OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(NOT ORIGINAL_NAME OR ORIGINAL_NAME STREQUAL "")
      set(ORIGINAL_NAME ${library_name})
    endif()

    add_custom_command(
      TARGET ${SOLARUS_BUNDLE_TARGET_NAME}
      POST_BUILD
      COMMAND if
      ARGS [ ! -f "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/${destination_directory}/${ORIGINAL_NAME}" ] \; then cp "${library_path}" "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/${destination_directory}/${ORIGINAL_NAME}" \; fi
    )
  endif()
endmacro()
if(NOT XCODE)
  if(NOT SOLARUS_IOS_BUILD)
    add_custom_command(TARGET ${SOLARUS_BUNDLE_TARGET_NAME} POST_BUILD COMMAND mkdir ARGS -p "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/Frameworks")
  endif()
  foreach(LIB ${SOLARUS_BUNDLE_COPIED_LIBRARIES})
    copy_into_bundle(${LIB} Frameworks)
  endforeach()
  
# Proper way, buggy with Makefile Generator for now.
else()
  foreach(LIB ${SOLARUS_BUNDLE_COPIED_LIBRARIES})
    set_source_files_properties(${LIB} PROPERTIES MACOSX_PACKAGE_LOCATION Frameworks)
  endforeach()
endif()

# Info.plist template and additional lines
get_filename_component(SOLARUS_BUNDLE_ICON_NAME "${SOLARUS_BUNDLE_ICON}" NAME)
set_target_properties(${SOLARUS_BUNDLE_TARGET_NAME} PROPERTIES
  MACOSX_BUNDLE_INFO_PLIST             "${SOLARUS_BUNDLE_INFOPLIST}"
  MACOSX_BUNDLE_BUNDLE_NAME            "${SOLARUS_BUNDLE}"
  MACOSX_BUNDLE_ICON_FILE              "${SOLARUS_BUNDLE_ICON_NAME}"
  MACOSX_BUNDLE_BUNDLE_VERSION         "${SOLARUS_BUNDLE_VERSION}"

  MACOSX_BUNDLE_GUI_IDENTIFIER         "${SOLARUS_BUNDLE_GUI_IDENTIFIER}.${SOLARUS_BUNDLE}"
  MACOSX_BUNDLE_SHORT_VERSION_STRING   "${SOLARUS_BUNDLE_VERSION}"
  MACOSX_BUNDLE_LONG_VERSION_STRING    "${SOLARUS_BUNDLE} Version ${SOLARUS_BUNDLE_VERSION}"
  MACOSX_BUNDLE_COPYRIGHT              "Copyright 2013, ${SOLARUS_BUNDLE_GUI_IDENTIFIER}."
  MACOSX_BUNDLE_INFO_STRING            "${SOLARUS_BUNDLE} Version ${SOLARUS_BUNDLE_VERSION}, Copyright 2013, ${SOLARUS_BUNDLE_GUI_IDENTIFIER}."
)

# Embed library search path
if(NOT SOLARUS_IOS_BUILD)
  if(NOT CMAKE_OSX_DEPLOYMENT_TARGET VERSION_LESS "10.5")
    if(NOT CMAKE_EXE_LINKER_FLAGS MATCHES "-Xlinker -rpath")
      set(CMAKE_EXE_LINKER_FLAGS         "${CMAKE_EXE_LINKER_FLAGS} -Xlinker -rpath -Xlinker @loader_path/../Frameworks/" CACHE STRING "Embed frameworks search path" FORCE)
    endif()
    set(SOLARUS_RPATH                  "@rpath/")
  else()
    set(SOLARUS_RPATH                  "@executable_path/../Frameworks/")
  endif()
  set_target_properties(${SOLARUS_BUNDLE_TARGET_NAME} PROPERTIES 
    BUILD_WITH_INSTALL_RPATH           1 
    INSTALL_NAME_DIR                   ${SOLARUS_RPATH}	
  )
endif()

# Code signing
if(XCODE)
  set_target_properties(${SOLARUS_BUNDLE_TARGET_NAME} PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "${SOLARUS_OS_NAME} Developer: ${SOLARUS_BUNDLE_GUI_IDENTIFIER}")
elseif(SOLARUS_BUNDLE_CODESIGN)
  add_custom_command(
    TARGET ${SOLARUS_BUNDLE_TARGET_NAME}
    POST_BUILD
    COMMAND codesign
    ARGS -s  "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app"
  )
endif()

# Move the main binary into resource folder, and substitute it
# by a wrapper which call it with the resource path as parameter
add_custom_command(
  TARGET ${SOLARUS_BUNDLE_TARGET_NAME}
  POST_BUILD
  COMMAND mv 
  ARGS "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/MacOS/${SOLARUS_BUNDLE}" "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/Resources/solarus"
  COMMAND cp 
  ARGS "${PROJECT_BINARY_DIR}/cmake/apple/bundle_content/OSX-wrapper.sh" "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/MacOS/${SOLARUS_BUNDLE}"
)

# Copy the PkgInfo file
if(NOT XCODE)
  add_custom_command(
    TARGET ${SOLARUS_BUNDLE_TARGET_NAME}
    POST_BUILD
    COMMAND cp
    ARGS "${PROJECT_BINARY_DIR}/cmake/apple/bundle_content/PkgInfo" "${PROJECT_BINARY_DIR}/${SOLARUS_BUNDLE}.app/Contents/"
  )
endif()
