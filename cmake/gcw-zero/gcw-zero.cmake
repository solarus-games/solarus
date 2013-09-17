# the name of the target operating system
SET(CMAKE_SYSTEM_NAME Linux)

# which C and C++ compiler to use
SET(CMAKE_C_COMPILER   "/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-gcc")
SET(CMAKE_CXX_COMPILER "/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-g++")
SET(CMAKE_RANLIB "/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-ranlib")
SET(CMAKE_LINKER "/opt/gcw0-toolchain/usr/bin/mipsel-gcw0-linux-uclibc-ld")

# here is the target environment located
SET(CMAKE_FIND_ROOT_PATH  /opt/gcw0-toolchain/usr/mipsel-gcw0-linux-uclibc/sysroot)


#SET(CMAKE_EXE_LINKER_FLAGS "-Wl,-rpath,./libs -Wl,")

#SET(CMAKE_CXX_FLAGS "-D_REENTRANT -DGCWZERO  -fomit-frame-pointer -ffunction-sections -fsingle-precision-constant -G0  -mips32r2")
#SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
#SET(CMAKE_CXX_FLAGS_RELEASE "")

#SET(CMAKE_CXX_FLAGS_RELEASE "-D_REENTRANT -DGCWZERO  -fomit-frame-pointer -ffunction-sections -fsingle-precision-constant -G0 -mips32r2")


#####

# adjust the default behaviour of the FIND_XXX() commands:
# search headers and libraries in the target environment, search
# programs in the host environment
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
