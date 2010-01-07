include_directories (${ZSDX_SOURCE_DIR}/include ${ZSDX_SOURCE_DIR}/include/SDL_Config ${ZSDX_SOURCE_DIR}/include/snes_spc ${LUA_INCLUDE_DIR})

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

file(
  GLOB_RECURSE
  source_files
  *.cpp
  ../include/*.h
)

add_executable (zsdx
  ${source_files}
)

if (NOT WIN32)
   SET_TARGET_PROPERTIES(zsdx PROPERTIES
			      COMPILE_FLAGS "-g -ansi -Wall -Werror")
endif (NOT WIN32)

target_link_libraries (zsdx
	${SDL_LIBRARY}
	${SDLIMAGE_LIBRARY}
	${SDLTTF_LIBRARY}
	${OPENAL_LIBRARY}
	${LUA_LIBRARY}
	${PHYSFS_LIBRARY}
	${SNDFILE_LIBRARY}
)

INSTALL(TARGETS	zsdx
	RUNTIME DESTINATION bin
)

