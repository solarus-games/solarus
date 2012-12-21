#ifndef __OSXINTERFACE_H__
#define __OSXINTERFACE_H__

// This are C "trampoline" function that will be used
// to invoke a specific Objective-C method FROM C++
const char * getUserApplicationSupportDirectory();
const char * getOSXVersion();

#endif