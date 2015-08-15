# Unit tests.
enable_testing()
add_subdirectory(tests)

# Add RPath on testing target
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set_target_properties(solarus_testing PROPERTIES 
    BUILD_WITH_INSTALL_RPATH           1 
    INSTALL_NAME_DIR                   "@rpath/"
  )
endif()