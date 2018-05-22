# Include paths.
include_directories(
    "${CMAKE_BINARY_DIR}/include"  # For config.h.
    "${CMAKE_SOURCE_DIR}/include"
)

# Third-party include paths, marked as system ones to disable their warnings.
include_directories(SYSTEM
    "${CMAKE_SOURCE_DIR}/include/solarus/third_party"
    "${CMAKE_SOURCE_DIR}/include/solarus/third_party/snes_spc"
)
message("LuaJIT ${LUA_INCLUDE_DIR}")
# External include paths, marked as system ones to disable their warnings.
include_directories(SYSTEM
        "${MODPLUG_INCLUDE_DIRS}"  # Before SDL2 because we want the sndfile.h of ModPlug.
        "${SDL2_INCLUDE_DIR}"
        "${SDL2_TTF_INCLUDE_DIR}"
        "${SDL2_IMAGE_INCLUDE_DIR}"
        "${OPENAL_INCLUDE_DIR}"
        "${VORBISFILE_INCLUDE_DIR}"
        "${OGG_INCLUDE_DIR}"
        "${LUA_INCLUDE_DIR}"
        "${PHYSFS_INCLUDE_DIR}"
        "${GLM_INCLUDE_DIRS}"
)

if (OPENGL_INCLUDE_DIR)
    include_directories(SYSTEM
        ${OPENGL_INCLUDE_DIR}
    )
endif()
