/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Debug.h"
#include "solarus/core/AppleInterface.h"

#if defined(SOLARUS_OSX) || defined(SOLARUS_IOS)

#if defined(SOLARUS_OSX)
#  import <Cocoa/Cocoa.h>
#elif defined(SOLARUS_IOS)
#  import <Foundation/Foundation.h>
#  import <UIKit/UIKit.h>
#endif


/**
 * @brief Create and return the Application Support folder from the User Domain
 * which doesn't exist by default in the iOS sandboxed environment.
 *
 * @return The Application Support folder from the User Domain.
 */
std::string get_user_application_support_directory()
{
  @autoreleasepool {

    NSString* app_support_directory = [
        NSSearchPathForDirectoriesInDomains(NSApplicationSupportDirectory, NSUserDomainMask, YES)
        objectAtIndex:0];

    // Create the Application Support folder if it doesn't exist yet.
    if (![[NSFileManager defaultManager]
        fileExistsAtPath:app_support_directory
        isDirectory:NULL]) {

      NSError* error = nil;
      if (![[NSFileManager defaultManager]
          createDirectoryAtPath:app_support_directory
          withIntermediateDirectories:YES
          attributes:nil
          error:&error]) {
        Solarus::Debug::error("Cannot create " + std::string([app_support_directory UTF8String]));
      }
    }

    return [app_support_directory UTF8String];
  }
}

#endif
