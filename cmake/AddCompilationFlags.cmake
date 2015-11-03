# Default compilation flags.

# Compile as C++11.
if(MINGW)
  # To avoid a compilation error in vorbisfile.h with fseeko64.
  set(CMAKE_CXX_FLAGS "-std=gnu++11 ${CMAKE_CXX_FLAGS}")
elseif(MSVC)
  # c++11 Enabled by default.
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++ ${CMAKE_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
else()
  set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS}")
endif()

# Compile in release mode by default.
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif()

# Warnings and errors.

if (MSVC)
  # Only show useful warnings in MSVC.
  set(CMAKE_CXX_FLAGS_DEBUG "/W4 ${CMAKE_CXX_FLAGS_DEBUG}")
else()
  # Be less pedantic in release builds for users.
  set(CMAKE_CXX_FLAGS_RELEASE "-Wno-error -Wno-all -Wno-unknown-pragmas -Wno-fatal-errors -Wno-extra ${CMAKE_CXX_FLAGS_RELEASE}")

  # Be more pedantic in debug mode for developers.
  set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror -Wextra -pedantic ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

# Platform-specific flags.
if(WIN32)
  #windows: disable the console by default.
  if(MSVC)
   #set_target_properties(solarus_engine properties link_flags_release "/subsystem:windows")
  elseif(cmake_compiler_is_gnucxx)
   set(cmake_cxx_flags "-mwindows ${cmake_cxx_flags}")
  endif()
endif()

if(GCWZERO)
  add_definitions(-DGCWZERO)
endif()

