#include <OSXInterface.h>
#import <Cocoa/Cocoa.h>

/**
 * @brief Return "~/Library/Application Support/" or equivalent from the official way, which is an 10.6+ Obj-C function.
 *
 * Return an 10.0+ hardcoded equivalent workaround if run on older version
 *
 * @return The Application Support folder from the User Domain
 */
const char* getUserApplicationSupportDirectory()
{     
    const char* path;
    
    // If we're building against a compatible SDK
    if ([NSFileManager respondsToSelector:@selector(URLsForDirectory:inDomains:)])
        path = [[[[[NSFileManager defaultManager] 
                   URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] 
                  objectAtIndex:0] 
                 path]
                UTF8String];
    else
        path = [[NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support/"]
                UTF8String];
    
    return path;
}