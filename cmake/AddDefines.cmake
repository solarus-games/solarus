# Solarus-specific -D preprocessor constants.

# Quest to launch if none is specified at runtime.
set(DEFAULT_QUEST "." CACHE STRING "Path to the quest to launch if none is specified at runtime.")
if(DEFAULT_QUEST)
  add_definitions(-DSOLARUS_DEFAULT_QUEST=\"${DEFAULT_QUEST}\")
endif()

# Directory where to write savegames and other files saved by quests.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(INITIAL_WRITE_DIR "Solarus")
else()
  set(INITIAL_WRITE_DIR ".solarus")
endif()
set(WRITE_DIR ${INITIAL_WRITE_DIR} CACHE STRING "Directory where Solarus savegames are stored, relative to the user's base write directory.")
if(WRITE_DIR)
  add_definitions(-DSOLARUS_WRITE_DIR=\"${WRITE_DIR}\")
endif()

# Quest size.
if(PANDORA)
  set(INITIAL_DEFAULT_QUEST_WIDTH 400)
else()
  set(INITIAL_DEFAULT_QUEST_WIDTH 320)
endif()
set(DEFAULT_QUEST_WIDTH ${INITIAL_DEFAULT_QUEST_WIDTH} CACHE INTEGER "Default width of the quest screen in pixels.")
set(DEFAULT_QUEST_HEIGHT 240 CACHE INTEGER "Default height of the quest screen in pixels.")
if(DEFAULT_QUEST_WIDTH)
  add_definitions(-DSOLARUS_DEFAULT_QUEST_WIDTH=${DEFAULT_QUEST_WIDTH})
endif()
if(DEFAULT_QUEST_HEIGHT)
  add_definitions(-DSOLARUS_DEFAULT_QUEST_HEIGHT=${DEFAULT_QUEST_HEIGHT})
endif()

# Color depth.
if(CAANOO OR GCWZERO)
  set(INITIAL_COLOR_DEPTH 16)
else()
  set(INITIAL_COLOR_DEPTH 32)
endif()
set(COLOR_DEPTH ${INITIAL_COLOR_DEPTH} CACHE INTEGER "Number of bits per pixel for surfaces.")
if(COLOR_DEPTH)
  add_definitions(-DSOLARUS_COLOR_DEPTH=${COLOR_DEPTH})
endif()

