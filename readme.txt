This file explains how to compile Zelda: Mystery Solarus DX.
Only the developing team has to compile the project.
The users download directly a compiled binary for Windows, Linux or Mac OS.

---------------------
External dependencies
---------------------

The following libraries are required to compile and to execute:

SDL
SDLmain
SDL_image
SDL_ttf
OpenAL
alut
lua5.1
physfs

The following dynamic library is recommanded to execute the game, but not required: openspc.
If you have OpenSPC (which does not seem to be available for all systems),
the program will try to load it dynamically as a plugin, and then use
it to decode the SPC musics. Otherwise, to play SPC musis, it will use a slower library (Snes_SPC)
whose source code is included in the project.

If the sound played delayed: try reducing the "refresh" value in OpenAL configuration file (usually /etc/openal/alsoft.conf)

----------------
SDL installation
----------------

Download the lastest version of the SDL libraries:
	- SDL and SDLmain: http://www.libsdl.org/download-1.2.php
	- SDL_image: http://www.libsdl.org/projects/SDL_image
	- SDL_ttf: http://www.libsdl.org/projects/SDL_ttf
Make sure you pick the development version of these librairies.


Linux users:

If you use Ubuntu, you can directly install the SDL packages:

libsdl1.2-dev
libsdl-image1.2-dev
libsdl-ttf2.0-dev

Other Linux packages are available on the pages linked above.


Windows users:

You must install the header files (the .h files contained in the include
directory of the archive) in an "SDL" subdirectory of your compiler's include
directory. This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\include\SDL.
Similarly, install the library files (the .lib files) in your compiler's lib directory.
This may be something like C:\Program Files\Microsoft Visual Studio 9.0\VC\lib.
Install the dynamic library files (the .dll files) in your system32 directory
(which may be C:\WINDOWS\system32).

------------------------
Compilation instructions
------------------------

ZSDX uses CMake to manage the compilation process. CMake permits to compile
the project on any OS. On Unix, it generates the appropriate makefiles; on Windows,
it can generate a project for most of the IDEs, including Visual C++, Code::Blocks
or MinGW.
You can also compile ZSDX without CMake, by creating a project from the sources with
your IDE.


Linux users:

Go to the zsdx directory and just type:

cmake .

This generates the appropriate Makefiles for your system.
Then you can compile with:

$ make

and install with:

# make install

Of course you need make and a compiler like gcc.
This will install the zsdx binary in a default directory such as /usr/local/bin. So you must be root to run the last command. Then you can launch the game with the simple command:

$ zsdx

You can instead install the binary in another directory, so that no root access is necessary. You can specify this directory as a parameter of cmake like this:

$ cmake -D CMAKE_INSTALL_PREFIX=/home/your_directory .
$ make
$ make install

This will install the zsdx binary in /home/your_directory/bin. Then you can launch the game with:

$ /home/your_directory/bin/zsdx

or just with:

$ zsdx

if /home/your_directory/bin is in your PATH.
There is no need to be root.


Windows users:

I have managed to compile ZSDX with Code::Blocks, without using CMake.

------------------
Mac OS developers
------------------

The same procedure should work.

Notes :

- Si SDL et SDL_image sont installés avec fink, un export CPPFLAGS="-I/sw/include -L/sw/lib" est nécessaire avant de lancer le configure
- option de compilation à l'édition de liens : -framework Carbon
