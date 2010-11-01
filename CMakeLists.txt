cmake_minimum_required(VERSION 2.6)
project(SOLARUS)

# the engine
add_subdirectory(src)

# quests
set(quests "zsdx") # put here your list of quests 

foreach(quest_name ${quests})
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/quests/${quest_name})
    add_subdirectory(quests/${quest_name}/data)
  endif(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/quests/${quest_name})
endforeach(quest_name ${quests})
 
# tests
add_subdirectory(tests)

