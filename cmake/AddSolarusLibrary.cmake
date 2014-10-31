# Solarus library source files.
file(
  GLOB
  source_files
  src/*.cpp
  src/third_party/snes_spc/*.cpp
  src/third_party/hqx/*.c
  src/entities/*.cpp
  src/hero/*.cpp
  src/hud/*.cpp
  src/lowlevel/*.cpp
  src/lowlevel/shaders/*.cpp
  src/lua/*.cpp
  src/menus/*.cpp
  src/movements/*.cpp
  src/containers/*.cpp
  include/*.h
  include/*.inl
  include/entities/*.h
  include/entities/*.inl
  include/hero/*.h
  include/hero/*.inl
  include/hud/*.h
  include/hud/*.inl
  include/lowlevel/*.h
  include/lowlevel/*.inl
  include/lowlevel/shaders*.h
  include/lowlevel/shaders*.inl
  include/lua/*.h
  include/lua/*.inl
  include/menus/*.h
  include/menus/*.inl
  include/movements/*.h
  include/movements/*.inl
  include/containers/*.h
  include/containers/*.inl
  include/third_party/snes_spc/*.h
  include/third_party/hqx/*.h
)

# Additional source files for Apple systems.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(source_files
    ${source_files}
    src/lowlevel/apple/AppleInterface.mm
    include/lowlevel/apple/AppleInterface.h
  )
endif()

# Build the Solarus library.
add_library(solarus
  SHARED
  ${source_files}
)

# Configuration for OSX and iOS build and deployment.
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  if(NOT SOLARUS_IOS_BUILD)
    include(OSX)
  else()
    include(iOS)
  endif()
endif()

