# Unit tests.
enable_testing()
add_subdirectory(tests)

# Set up the root path on OSX testing target.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(solarus_testing PROPERTIES
    MACOSX_RPATH                       ON
    BUILD_WITH_INSTALL_RPATH           1
    INSTALL_NAME_DIR                   "@rpath"
  )
endif()
