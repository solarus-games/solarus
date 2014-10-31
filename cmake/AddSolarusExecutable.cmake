# Source file of the solarus_run executable.
set(main_source_file
  src/main/Main.cpp
)

# Build the executable,
if(SOLARUS_BUNDLE OR SOLARUS_IOS_BUILD)
  # Build into CFBundle form if requested or iOS build.
  include(addCFBundleTarget)
else()
  add_executable(solarus_run
    ${main_source_file}
  )
endif()

target_link_libraries(solarus_run
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

