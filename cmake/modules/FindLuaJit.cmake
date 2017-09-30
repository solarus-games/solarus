# Locate LuaJIT library
# This module defines
#  LUA51_FOUND, if false, do not try to link to Lua
#  LUA_LIBRARIES
#  LUA_INCLUDE_DIR, where to find lua.h
#  LUA_VERSION_STRING, the version of Lua found (since CMake 2.8.8)
#
# This module is similar to FindLua51.cmake except that it finds LuaJit instead.

FIND_PATH(LUA_INCLUDE_DIR luajit.h
  HINTS
  $ENV{LUA_DIR}
  PATH_SUFFIXES include/luajit-2.1 include/luajit-2.0 include/luajit-5_1-2.0 include/luajit-5_1-2.1 include
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /sw # Fink
  /opt/local # DarwinPorts
  /opt/csw # Blastwave
  /opt
)

FIND_LIBRARY(LUA_LIBRARY
  NAMES luajit-5.1
  HINTS
  $ENV{LUA_DIR}
  PATH_SUFFIXES lib64 lib
  PATHS
  ~/Library/Frameworks
  /Library/Frameworks
  /sw
  /opt/local
  /opt/csw
  /opt
)

IF(LUA_LIBRARY)
  # include the math library for Unix
  IF(UNIX AND NOT APPLE)
    FIND_LIBRARY(LUA_MATH_LIBRARY m)
    SET( LUA_LIBRARIES "${LUA_LIBRARY};${LUA_MATH_LIBRARY}" CACHE STRING "Lua Libraries")
  # For Windows and Mac, don't need to explicitly include the math library
  ELSE(UNIX AND NOT APPLE)
    SET( LUA_LIBRARIES "${LUA_LIBRARY}" CACHE STRING "Lua Libraries")
  ENDIF(UNIX AND NOT APPLE)
ENDIF(LUA_LIBRARY)

IF(LUA_INCLUDE_DIR AND EXISTS "${LUA_INCLUDE_DIR}/luajit.h")
  FILE(STRINGS "${LUA_INCLUDE_DIR}/luajit.h" lua_version_str REGEX "^#define[ \t]+LUA_RELEASE[ \t]+\"LuaJIT .+\"")

  STRING(REGEX REPLACE "^#define[ \t]+LUA_RELEASE[ \t]+\"LuaJIT ([^\"]+)\".*" "\\1" LUA_VERSION_STRING "${lua_version_str}")
  UNSET(lua_version_str)
ENDIF()

INCLUDE(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LUA_FOUND to TRUE if
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LuaJit
                                  REQUIRED_VARS LUA_LIBRARIES LUA_INCLUDE_DIR
                                  VERSION_VAR LUA_VERSION_STRING)

MARK_AS_ADVANCED(LUA_INCLUDE_DIR LUA_LIBRARIES LUA_LIBRARY LUA_MATH_LIBRARY)

