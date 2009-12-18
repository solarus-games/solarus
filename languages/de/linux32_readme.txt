This archive contains a binary version of Zelda Mystery of Solarus DX (demo) for Linux 32-bit i386 architectures.
This file explains how to run the program. Note that for some distributions, we provide binary packages.
The source code is also available if you want to compile the program yourself.
See http://www.zelda-solarus.com/jeu-zsdx-english for more details.


------------
Dependencies
------------

The following dynamic libraries are required to to execute the game:

SDL
SDL_image
SDL_ttf
lua5.1
physfs
openal
libsndfile


----------------
Running the game
----------------

Once the archive is uncompressed, to run the game, execute the zsdx binary (assuming you are in its directory):

$ ./zsdx

The game relies on the data.zsdx file to get its data files. If data.zsdx is not in the current directory,
you have to give the -datapath=<path> option to indicate the location of data.zsdx. For example:

$ ./zsdx -datapath=some/path

where some/path corresponds to the directory containing data.zsdx.


------------------
Known sound issues
------------------

You may have sound issues, which I experienced under Ubuntu 9.04 (Jaunty) and was able to fix as explained below.
Edit the OpenAL configuration file (normally /etc/openal/alsoft.conf) and try the following settings:

- If the sounds play always with a delay (typically, a fraction of second), try reducing the "refresh" value to 2048 (the default value is 8192 with old versions of OpenAL).
- If the sound plays scratched in the first few seconds or longer, try forcing the driver to OSS, by setting the "driver" list value to "oss" (without the quotes).

Newer versions of OpenAL and PulseAudio may work better.

Please feel free to share your experience on our boards, where you can also find some help: http://forums.zelda-solarus.com

