This file explains how to compile Zelda: Mystery Solarus DX.
Only the developing team has to compile the project. The users download directly a compiled binary for Windows, Linux or Mac OS.

---------------------
External dependencies
---------------------

The following libraries are required to compile and to execute:

SDL
SDLmain
SDL_image
SDL_ttf
lua5.1
physfs
openal
alut
openspc or snes_spc

Note that OpenSPC (the library for playing SPC musics) is available only for i386 32-bit architectures.
For architectures incompatible with OpenSPC, you can use an alternative (but slower) SPC library, snes_spc.
More details can be found in file include/SpcDecoder.h.


Linux developers:

Install the corresponding packages. For example, with Ubuntu or Debian:
libsdl1.2-dev libsdl-image1.2-dev libsdl-ttf2.0-dev liblua5.1-0-dev libphysfs-dev libopenal-dev libalut-dev libopenspc-dev

The directory libraries/linux-i386 contains a compiled version of the OpenSPC library (static and dynamic libraries)
for the i386 architecture if you want to install it yourself.


Windows developers:

Download the binary version of each one of the development libraries listed above, on their official websites.
Install the header files (the .h files) in your compiler's include directory.
This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\include.
Similarly, install the library files (the .lib files) in your compiler's lib directory.
This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\lib.
Install the dynamic library files (the .dll files) in your system32 directory
(which may be C:\WINDOWS\system32).

The directory win32/libraries contain all static and dynamic libraries that are not provided as binaries
by their authors.
For SDL, SDL_image and SDL_ttf, the header files must be placed in an "SDL" subdirectory of the include directory.
For OpenAL and ALUT, the header files must be placed in an "AL" subdirectory of the include directory.
For OpenSPC, you will probably not find a binary release (I did not). Pick the 32-bit version in the directory win32/libraries.
You may need to additionally link with libz.


------------------------
Compilation instructions
------------------------

ZSDX uses CMake to manage the compilation process. CMake permits to compile
the project on any OS. On Unix, it generates the appropriate makefiles; on Windows,
it can generate a project for most of the IDEs, including Visual C++, Code::Blocks or MinGW.
You can also compile ZSDX without CMake, by creating a project from the sources with your IDE
and link with the libraries listed above.


Linux users:

Go to the zsdx directory and just type:

cmake .

This generates the appropriate Makefiles for your system.
Then you can compile with:

$ make

and install with:

# make install

Of course you need make and a compiler like gcc.
This will install the zsdx binary in a default directory such as /usr/local/bin. So you must be root to run the last command.
Then, you can launch the game with the simple command:

$ zsdx

You can instead install the binary in another directory, so that no root access is necessary. You can specify this directory as a parameter of cmake like this:

$ cmake -D CMAKE_INSTALL_PREFIX=/home/your_directory .
$ make
$ make install

This will install the zsdx binary in /home/your_directory/bin. Then, you can launch the game with:

$ /home/your_directory/bin/zsdx

or just with:

$ zsdx

if /home/your_directory/bin is in your PATH. There is no need to be root.

Note that for now, the current directory must be the one containing the progam itselft, otherwise the data will not be found. 


Windows users:

I have managed to compile ZSDX with Code::Blocks, without using CMake.


------------------
Mac OS developers
------------------

I don't have Mac OS, so this readme is for now very incomplete in this section.

Notes:

- If SDL and SDL_image are installed with fink, an 'export CPPFLAGS="-I/sw/include -L/sw/lib"' is necessary before launching the configure script
- When linking, use the following option: -framework Carbon


------------------
Known sound issues
------------------

With linux, you may have sound issues, which I experienced under Ubuntu 9.04 (Jaunty) and was able to fix as explained below.
Edit the OpenAL configuration file (normally /etc/openal/alsoft.conf) and try the following settings:

- If the sounds play always with a delay (typically, a fraction of second), try reducing the "refresh" value to 2048 (the default value is 8192 with old versions of OpenAL).
- If the sound plays scratched in the first few seconds or longer, try forcing the driver to OSS, by setting the "driver" list value to "oss" (without the quotes).

Newer versions of OpenAL and PulseAudio seem to work better, such as those of Ubuntu 9.10 (Karmic).

