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
#ifndef __APPLEINTERFACE_H__
#define __APPLEINTERFACE_H__

#include "solarus/core/Common.h"
#include <string>

#if defined(SOLARUS_OSX) || defined(SOLARUS_IOS)

// This are C++ "trampoline" function that will be used
// to invoke a specific Objective-C method from C++.
std::string get_user_application_support_directory();

#endif

#endif
