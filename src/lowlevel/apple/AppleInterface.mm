#include <AppleInterface.h>

#if defined(SOLARUS_IOS)
#  import <Cocoa/Cocoa.h>
#elif defined(SOLARUS_OSX)
#  import <Foundation/Foundation.h>
#  import <UIKit/UIKit.h>
#endif

/**
 * @brief Return "~/Library/Application Support/" or equivalent from the official way, which is available in OSX 10.6+ and iOS 4.0+.
 *
 * Return a 10.0+ hardcoded equivalent workaround if URLsForDirectory:inDomains: is not found.
 *
 * @return The Application Support folder from the User Domain
 */
const char* getUserApplicationSupportDirectory()
{
    // If the running OS implement URLsForDirectory:inDomains: method
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