#include <OSXInterface.h>
#import <Cocoa/Cocoa.h>

/**
 * @brief Return "~/Library/Application Support/" or equivalent from the official way, which is an 10.6+ Obj-C function.
 *
 * Only works if the SDK version is greater or equal to 10.6
 * Define SOLARUS_DONT_USE_OSX_SANDBOX constant to use an hardcoded equivalent workaround
 *
 * @return The Application Support folder from the User Domain
 */
const char* getUserApplicationSupportDirectory()
{
#ifndef SOLARUS_DONT_USE_OSX_SANDBOX          
    return [[[[[NSFileManager defaultManager] 
               URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] 
              objectAtIndex:0] 
             path]
            UTF8String];
#else
    return "~/Library/Application Support/";
#endif
}