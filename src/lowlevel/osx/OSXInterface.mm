#include <OSXInterface.h>
#import <Cocoa/Cocoa.h>

/**
 * @brief Return "~/Library/Application Support/" or equivalent from the official way, which is an 10.6+ Obj-C function.
 *
 * Return a 10.0+ hardcoded equivalent workaround if run on older version
 *
 * @return The Application Support folder from the User Domain
 */
const char* getUserApplicationSupportDirectory()
{
    // If current OSX version implement URLsForDirectory:inDomains: method
    if ([NSFileManager respondsToSelector:@selector(URLsForDirectory:inDomains:)])
        return [[[[[NSFileManager defaultManager] 
                   URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] 
                  objectAtIndex:0] 
                 path]
                UTF8String];
    else
        return [[NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support/"]
                UTF8String];
}