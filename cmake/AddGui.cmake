# Whether the user wants to build the launcher GUI additionally to the batch
# executable (requires Qt).
# Building the GUI is optional to allow systems to directly
# run a game without a GUI launcher window.
option(SOLARUS_GUI "Generate the launcher GUI" ON)

if(SOLARUS_GUI)
  # The Solarus GUI executable has its own CMakeLists.txt file
  # because dependencies and includes are different.
  add_subdirectory("gui" "${CMAKE_BINARY_DIR}/gui")
endif()

