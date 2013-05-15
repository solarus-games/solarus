#include <lowlevel/apple/AppleInterface.h>

#if defined(SOLARUS_OSX)
#  import <Cocoa/Cocoa.h>
#elif defined(SOLARUS_IOS)
#  import <Foundation/Foundation.h>
#  import <UIKit/UIKit.h>
#endif


// WORKAROUND : enum NSApplicationSupportDirectory and NSUserDomainMask are not defined on older OSX frameworks, 
// so we'll use a macro to get the Application Support folder from the User Domain by the official way
#if MAC_OS_X_VERSION_10_4 <= MAC_OS_X_VERSION_MAX_ALLOWED || __IPHONE_2_0 <= __IPHONE_OS_VERSION_MAX_ALLOWED
#  define USER_APP_SUPPORT [[[[[NSFileManager defaultManager] URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] objectAtIndex:0] path] UTF8String];
#else
// Should never be use, just avoid to report the "undefined" error on older versions
#  define USER_APP_SUPPORT 0;
#endif


/**
 * @brief Return "~/Library/Application Support/" or equivalent from the official way, which is available in OSX 10.6+ and iOS 4.0+.
 *
 * Return an OSX 10.0+ and iOS 1.0+ (not compatible with Iphone Simulator) hardcoded equivalent workaround if URLsForDirectory:inDomains: is not found.
 *
 * @return The Application Support folder from the User Domain
 */
const char* getUserApplicationSupportDirectory()
{
    // If the running OS implement URLsForDirectory:inDomains: method
    if ([NSFileManager respondsToSelector:@selector(URLsForDirectory:inDomains:)])
        return USER_APP_SUPPORT;
    else
        return [[NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support/"]
                UTF8String];
}
