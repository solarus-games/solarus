Every XCode projects are currently hardcoded for our environment, but you easily can modify them to match yours, or read the following instructions to use the same environment than us.


Install header files :
In addition to the existing project, you have to install all dependencies. 
Header files should be the same ones for each platform, so you install them once and for all into /usr/local/include (use separate folder for each libraries, see the "Header Search Path" option in XCode to know exact expected names).


Compile the iOS-Simulator project :

The last step to run solarus on iOS-Simulator is to copy/paste every library into /usr/local/libiossim4.3 .
Look into the Frameworks group of the XCode project to know exact expected names.
Don't forget that iOS-simulator uses the i386 architecture.


Compile the iOS project :

Do the same as for iOS-Simulator, except you place armv6/armv7 libraries into /usr/local/libios4.3


Compile the OSX project :

If you place all OSX frameworks into /Library/Frameworks/ , the OSX project shouldn't need these extra configuration step (neither header or library part), except for lua5.1 and modplug that we still use as .a and .dylib .
Place libraries into /usr/local/lib .