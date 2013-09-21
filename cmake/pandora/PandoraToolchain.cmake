# this one is important
SET(CMAKE_SYSTEM_NAME Linux)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /opt/pandora-toolchain/bin/arm-none-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER /opt/pandora-toolchain/bin/arm-none-linux-gnueabi-g++)

SET(CMAKE_BUILD_TYPE release)
SET(CMAKE_CXX_FLAGS "-O3 -pipe -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ftree-vectorize -ffast-math -fsingle-precision-constant -fsigned-char -I${PND_SDK}/usr/include")
SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS}")
SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe -march=armv7-a -mtune=cortex-a8 -mfpu=neon -mfloat-abi=softfp -ftree-vectorize -ffast-math -fsingle-precision-constant -fsigned-char -I${PND_SDK}/usr/include")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")
set(CXX_DEFINES "-DPANDORA -DHAVE_GLES")

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /opt/pandora-toolchain/)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

