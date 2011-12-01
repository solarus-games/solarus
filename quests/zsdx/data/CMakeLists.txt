set(quest_name "zsdx")

# data files list
file(GLOB_RECURSE data_files
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  *.spc *.ogg *.it *.png *.dat *.lua *.ttf *.fon)

# add other data to zip archive
add_custom_command(
  OUTPUT data.solarus
  DEPENDS ${data_files}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND zip -q ${CMAKE_CURRENT_BINARY_DIR}/data.solarus ${data_files}
)

add_custom_target(${quest_name}_data
  ALL
  DEPENDS data.solarus
)

# create a script that executes the game with this quest
add_custom_command(
  OUTPUT ${quest_name}
  COMMAND echo '\#!/bin/bash' > ${quest_name}
  COMMAND echo '${CMAKE_INSTALL_PREFIX}/bin/solarus ${CMAKE_INSTALL_PREFIX}/share/solarus/${quest_name} $*' >> ${quest_name}
)
add_custom_target(${quest_name}_command
  ALL
  DEPENDS ${quest_name}
)

# install the data archive
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/data.solarus
  DESTINATION share/solarus/${quest_name}
)

# install the script
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${quest_name}
  DESTINATION bin
)

