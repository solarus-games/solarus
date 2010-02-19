set(quest_name "zsdx")

# compile the Lua scripts
file(GLOB lua_source_files 
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  maps/*.lua)
set(lua_bin_files "")
foreach(lua_source_file ${lua_source_files})
  add_custom_command(
    OUTPUT ${lua_source_file}c
    MAIN_DEPENDENCY ${lua_source_file}
    COMMAND luac -o ${lua_source_file}c ${lua_source_file})
  set(lua_bin_files ${lua_bin_files} ${lua_source_file}c)
endforeach(lua_source_file ${lua_source_files})

# data files list
file(GLOB_RECURSE data_files
  RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
  *.spc *.ogg *.png *.dat *.ttf *.fon)
set(data_files ${data_files} ${lua_bin_files})

# build the data archive
add_custom_command(
  OUTPUT data.solarus
  DEPENDS ${data_files}
  COMMAND zip -q data.solarus ${data_files}
)
add_custom_target(${quest_name}_data
  ALL
  DEPENDS data.solarus
)

# create a script that executes the game with this quest
add_custom_command(
  OUTPUT ${quest_name}
  COMMAND echo '\#!/bin/bash' > ${quest_name}
  COMMAND echo 'solarus -datapath=${CMAKE_INSTALL_PREFIX}/share/solarus/${quest_name} $*' >> ${quest_name}
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

