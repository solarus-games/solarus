#include <OSXInterface.h>
#import <Cocoa/Cocoa.h>

/**
 * @brief Return "~/Library/Application Support/" or equivalent from the official way, which is an 10.6+ Obj-C function.
 *
 * Only works if developer's SDK version greater or equal to 10.6
 * Define SOLARUS_DONT_USE_OSX_SANDBOX constant to use an hardcoded workaround
 *
 * @return The Application Support folder from the User Domain
 */
const char* getUserApplicationSupportDirectory()
{
#ifndef SOLARUS_DONT_USE_OSX_SANDBOX
    return [[[[[[NSFileManager defaultManager] 
                URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] 
               objectAtIndex:0] 
              absoluteString] 
             stringByAppendingString:@"/"]
            cStringUsingEncoding:NSUnicodeStringEncoding];
#else
    return "~/Library/Application Support/";
#endif
}

    
/**
 * @brief Get the Mac OSX version number from /System/Library/CoreServices/SystemVersion.plist
 *
 * This actually was the recommended way to get the OS version briefly circa 10.0/10.1, and the best way currently available since 10.8
 * Remove this function once Apple release an official way to get the OSX current version, and available from 10.3 to 10.8+
 *
 * @return Get the Mac OSX version number
 */
const char* getOSXVersion()
{
    NSDictionary * sv = [NSDictionary dictionaryWithContentsOfFile:@"/System/Library/CoreServices/SystemVersion.plist"];
    return [[sv objectForKey:@"ProductVersion"] cStringUsingEncoding:NSUnicodeStringEncoding];
}