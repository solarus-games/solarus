#ifndef __APPLEINTERFACE_H__
#define __APPLEINTERFACE_H__

#include "Common.h"

// This are C "trampoline" function that will be used
// to invoke a specific Objective-C method FROM C++
const char * getUserApplicationSupportDirectory();

#endif
