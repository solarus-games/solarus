cmake_minimum_required(VERSION 2.6)
project(SOLARUS_TOOLS)

if(COMMAND maven)
  if(COMMAND bash) # *NIX
    exec_program(COMMAND compile)
  else() # WINDOWS
    exec_program(COMMAND compile.bat)
  endif()
endif()

install(TARGETS solarus_tools
  RUNTIME DESTINATION ${SOLARUS_TOOLS_INSTALL_DESTINATION})
