# this one is important
SET(CMAKE_SYSTEM_NAME Linux)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /opt/pandora-toolchain/bin/arm-none-linux-gnueabi-gcc)
SET(CMAKE_CXX_COMPILER /opt/pandora-toolchain/bin/arm-none-linux-gnueabi-g++)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH  /opt/pandora-toolchain/)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM BOTH)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

#usage sample with the sample case "wesnoth":
#current 1.10 setup:
#PATH=/home/christophe/pandora-dev/arm-2011.09/usr/bin:$PATH CFLAGS="-DPANDORA -O3 -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp" CXXFLAGS="-DPANDORA -O3 -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp" cmake -DCMAKE_BUILD_TYPE=release -DENABLE_STRICT_COMPILATION=off -DCMAKE_TOOLCHAIN_FILE=/home/christophe/pandora-dev/sdk_utils/PandoraToolchain.cmake -DPKG_CONFIG_EXECUTABLE=/home/christophe/pandora-dev/arm-2011.09/bin/arm-none-linux-gnueabi-pkg-config -DSDL_CONFIG=/home/christophe/pandora-dev/arm-2011.09/usr/bin/sdl-config -DLIBINTL_INCLUDE_DIR=/home/christophe/pandora-dev/arm-2011.09/arm-none-linux-gnueabi/libc/usr/include/ -DPREFERENCES_DIR=wesnoth-1.10_userdata ../wesnoth-1.10.x
#
#current 1.11 setup:
#PATH=/home/christophe/pandora-dev/arm-2011.09/usr/bin:$PATH CFLAGS="-DPANDORA -O3 -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp" CXXFLAGS="-DPANDORA -O3 -pipe -march=armv7-a -mcpu=cortex-a8 -mtune=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp" cmake -DCMAKE_BUILD_TYPE=release -DENABLE_STRICT_COMPILATION=off -DCMAKE_TOOLCHAIN_FILE=/home/christophe/pandora-dev/sdk_utils/PandoraToolchain.cmake -DPKG_CONFIG_EXECUTABLE=/home/christophe/pandora-dev/arm-2011.09/bin/arm-none-linux-gnueabi-pkg-config -DSDL_CONFIG=/home/christophe/pandora-dev/arm-2011.09/usr/bin/sdl-config -DLIBINTL_INCLUDE_DIR=/home/christophe/pandora-dev/arm-2011.09/arm-none-linux-gnueabi/libc/usr/include/ -DPREFERENCES_DIR=wesnoth-1.11_userdata ../wesnoth-1.11.x
