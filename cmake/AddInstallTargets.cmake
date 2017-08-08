# Default installation directories.
if(CMAKE_LIBRARY_ARCHITECTURE)
  # Handle standard multi-architecture library directory names like x86_64-linux-gnu
  set(SOLARUS_LIBRARY_DIRECTORY_NAME "lib/${CMAKE_LIBRARY_ARCHITECTURE}")
else()
  set(SOLARUS_LIBRARY_DIRECTORY_NAME "lib${LIB_SUFFIX}")
endif()

set(SOLARUS_LIBRARY_INSTALL_DESTINATION "${SOLARUS_LIBRARY_DIRECTORY_NAME}" CACHE PATH "Library install destination")
# Install location for Debian-based systems
if(EXISTS '/usr/bin/apt')
  set(SOLARUS_EXECUTABLE_INSTALL_DESTINATION "games" CACHE PATH "Binary install destination")
else()
  set(SOLARUS_EXECUTABLE_INSTALL_DESTINATION "bin" CACHE PATH "Binary install destination")
endif()
set(SOLARUS_MANUAL_INSTALL_DESTINATION "share/man" CACHE PATH "Manual install destination")
set(SOLARUS_HEADERS_INSTALL_DESTINATION "include" CACHE PATH "Headers install destination")

# Files to install with make install.
# Install the shared library and the solarus-run executable.
install(TARGETS solarus solarus-run
  LIBRARY DESTINATION ${SOLARUS_LIBRARY_INSTALL_DESTINATION}
  RUNTIME DESTINATION ${SOLARUS_EXECUTABLE_INSTALL_DESTINATION}
)
# Install headers: useful for projects that use Solarus as a library.
install(DIRECTORY
  "${CMAKE_BINARY_DIR}/include/solarus"  # For config.h.
  "${CMAKE_SOURCE_DIR}/include/solarus"
  DESTINATION ${SOLARUS_HEADERS_INSTALL_DESTINATION}
)
# Linux Manpage
if(UNIX AND NOT APPLE)
  install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/solarus-run.6
    DESTINATION ${SOLARUS_MANUAL_INSTALL_DESTINATION}/man6)
endif(UNIX AND NOT APPLE)
