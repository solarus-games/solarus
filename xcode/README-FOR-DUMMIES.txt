Every XCode projects are currently hardcoded for my environment, but you easily can modify them to match yours, or read the following instructions to use the same environment than me.


Install header files :
In addition to the existing project, you have to install all dependencies. 
All header files should be the same ones on each platform, so you install them once and for all into /usr/local/include (use separate folder for each libraries, see the "Header Search Path" option in XCode to know the exact name).


Compile the iOS-Simulator project :

The last step to run solarus on iOS-Simulator is to copy/paste every library into /usr/local/libiossim4.3 .
Don't forget that iOS-sim uses i386 architecture.


Compile the iOS project :

Do the same as for iOS-Simulator, except you place armv6/armv7 libraries into /usr/local/libios4.3


Compile the OSX project :

If you place your OSX frameworks into /Library/Frameworks/ , the OSX project shouldn't need these extra configuration step (neither header or library).