# Source file of the solarus-run executable.
set(solarus_run_SOURCES
  src/main/Main.cpp
)

# Add the Solarus icon in Windows.
if(WIN32)
  set(solarus_run_SOURCES
    ${solarus_run_SOURCES}
    cmake/win32/resources.rc
  )
endif()

add_executable(solarus-run
  ${solarus_run_SOURCES}
)

target_link_libraries(solarus-run
  solarus
  "${SDL2_LIBRARY}"
  "${SDL2_IMAGE_LIBRARY}"
  "${SDL2_TTF_LIBRARY}"
  "${OPENAL_LIBRARY}"
  "${LUA_LIBRARY}"
  "${DL_LIBRARY}"
  "${PHYSFS_LIBRARY}"
  "${VORBISFILE_LIBRARY}"
  "${OGG_LIBRARY}"
  "${MODPLUG_LIBRARY}"
)

