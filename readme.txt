This file explains how to compile Zelda Solarus Deluxe.
Only the developing team has to compile the project. To preserve the secrets of the game, the source code is not public.
The users download directly a compiled binary for Windows, Linux or Mac OS.

-----------------
Required packages
-----------------

The following libraries are required to make ZSDX work:

SDL
SDLmain
SDL_image
SDL_ttf
fmodex
lua5.1
physfs

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

--------------------
FMOD Ex installation
--------------------

Windows users:

Download the latest FMOD Ex stable release on http://www.fmod.org/index.php/download.
The installation program does everything for you. At the end of the installation, you should
have an "fmodex" subdirectory of your compiler's include directory (for example
C:\Program Files\Microsoft Visual Studio 9.0\VC\include\fmodex), an fmodex.lib file
in your compiler's lib directory (for example
C:\Program Files\Microsoft Visual Studio 9.0\VC\lib) and an fmodex.dll file in 
your system32 directory (which may be C:\WINDOWS\system32).


Linux users:

There is no FMOD Ex package, so you have to install the library manually from the .tar.gz archive:
- Download the latest FMOD Ex stable release on http://www.fmod.org/index.php/download
- Uncompress the archive (example: tar xvzf fmodapi41400linux.tar.gz)
- Go to the subdirectory created (example: cd fmodapi41400linux)
- In the beginning of the Makefile, ensure the DESTLIBDIR and DESTHDRDIR variables are correct.
Note that with Ubuntu, these variables should be:
DESTLIBDIR = /usr/lib
DESTHDRDIR = /usr/include/fmodex
- Install the FMOD Ex librairies and headers with 'sudo make install'

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


------------------
Notes à traduire / vérifier / prendre en compte :

- Si SDL et SDL_image sont installés avec fink, un export CPPFLAGS="-I/sw/include -L/sw/lib" est nécessaire avant de lancer le configure
- utiliser sdl-config
- lien statique avec /usr/lib/libfmod.a
- option de compilation à l'édition de liens : -framework Carbon
