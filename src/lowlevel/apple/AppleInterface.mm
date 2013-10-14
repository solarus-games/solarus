/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include <lowlevel/apple/AppleInterface.h>

#if defined(SOLARUS_OSX)
#  import <Cocoa/Cocoa.h>
#elif defined(SOLARUS_IOS)
#  import <Foundation/Foundation.h>
#  import <UIKit/UIKit.h>
#endif


NSAutoreleasePool* solarus_pool = nil;

/**
 * @brief Initialize and allocate the NSAutoreleasePool object.
 *
 * Allow to use the Cocoa's reference-counted memory management system on Cocoa objects.
 */
void initPool()
{
    if (solarus_pool == nil)
        solarus_pool = [[NSAutoreleasePool alloc] init];
}

/**
 * @brief Drain the NSAutoreleasePool object.
 */
void drainPool()
{
    if (solarus_pool)
        [solarus_pool drain];
}

/**
 * @brief Return "~/Library/Application Support" or equivalent from the official way, which is available in OSX 10.6+ and iOS 4.0+.
 *
 * Return an OSX 10.0+ and iOS 1.0+ (not compatible with Iphone Simulator) hardcoded equivalent workaround if the 
 * build configuration is set for a lower minimum version.
 *
 * @return The Application Support folder from the User Domain.
 */
const char* getUserApplicationSupportDirectory()
{
#if defined(SOLARUS_OSX) && __MAC_OS_X_VERSION_MIN_REQUIRED  >= MAC_OS_X_VERSION_10_6 \
 || defined(SOLARUS_IOS) && __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_4_0
    return [[[[[NSFileManager defaultManager] 
               URLsForDirectory:NSApplicationSupportDirectory inDomains:NSUserDomainMask] 
              objectAtIndex:0] 
             path] 
            UTF8String];
#else
    // WORKAROUND : Avoid to report errors with undefined enum, and warning with undefined functions.
    // ( enum NSApplicationSupportDirectory and NSUserDomainMask are not defined on older OSX frameworks )
    return [[NSHomeDirectory() stringByAppendingPathComponent:@"Library/Application Support"] 
            UTF8String];
#endif
}
