set(quest_name "debug_quest")

# compile the Lua scripts
file(GLOB lua_source_files 
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  maps/*.lua
  items/*.lua)

set(lua_bin_files "")

MAKE_DIRECTORY("${CMAKE_CURRENT_BINARY_DIR}/maps")
MAKE_DIRECTORY("${CMAKE_CURRENT_BINARY_DIR}/items")

# convert to Lua bytecode
foreach(lua_source_file ${lua_source_files})
  add_custom_command(
    OUTPUT ${lua_source_file}c
    MAIN_DEPENDENCY ${lua_source_file}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND luac -o ${CMAKE_CURRENT_BINARY_DIR}/${lua_source_file}c ${lua_source_file})
  set(lua_bin_files ${lua_bin_files} ${lua_source_file}c)
endforeach(lua_source_file ${lua_source_files})

# add lua bytecode to the zip archive
add_custom_command(
  OUTPUT compiled_lua
  DEPENDS ${lua_bin_files}
  COMMAND zip -q data.solarus ${lua_bin_files}
)

# data files list
file(GLOB_RECURSE data_files
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  *.spc *.ogg *.png *.dat *.ttf *.fon)

# add other data to zip archive
add_custom_command(
  OUTPUT data.solarus
  DEPENDS ${data_files}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMAND zip -q ${CMAKE_CURRENT_BINARY_DIR}/data.solarus ${data_files}
)

add_custom_target(${quest_name}_data
  ALL
  DEPENDS data.solarus compiled_lua
)

# create a script that executes the game with this quest
add_custom_command(
  OUTPUT ${quest_name}
  COMMAND echo '\#!/bin/bash' > ${quest_name}
  COMMAND echo '${CMAKE_INSTALL_PREFIX}/bin/solarus -datapath=${CMAKE_INSTALL_PREFIX}/share/solarus/${quest_name} $*' >> ${quest_name}
)
add_custom_target(${quest_name}_command
  ALL
  DEPENDS ${quest_name}
)

# install the data archive
install(FILES data.solarus
  DESTINATION share/solarus/${quest_name}
)

# install the script
install(PROGRAMS ${quest_name}
  DESTINATION bin
)

