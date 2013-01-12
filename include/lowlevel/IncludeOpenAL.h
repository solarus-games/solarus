/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_OSX_INCLUDE_OPENAL_H
#define SOLARUS_OSX_INCLUDE_OPENAL_H


//This is a hack to fix a pragma warning in the OSX implementation of OpenAL headers, works with GCC, llvm-gcc and Clang
#if defined(TARGET_OS_MAC)
#  pragma GCC system_header
#endif

#include <al.h>
#include <alc.h>



// How the workaround should be done directly inside Sound.h, if there weren't different preprocessing behavior with C++ build
/*#if defined(TARGET_OS_MAC)
 #  if __GNUC__ && ( __GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ > 5 )
 #    pragma GCC diagnostic push
 #  endif
 #  if __GNUC__ && ( __GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ > 1 )
 #    pragma GCC diagnostic ignored "-Wunknown-pragmas"
 #  else
 #    pragma GCC warning "You should manually compile with -Wno-unknown-pragmas flag to avoid warnings"
 #  endif
 #endif
 
 #include <al.h>
 #include <alc.h>
 
 #if defined(TARGET_OS_MAC)
 #  if __GNUC__ && ( __GNUC__ > 4 || __GNUC__ == 4 && __GNUC_MINOR__ > 5 )
 #    pragma GCC diagnostic pop
 #  endif
 #  if __GNUC__ && (__GNUC__ == 4 && ( __GNUC_MINOR__ > 1 || __GNUC_MINOR__ < 6))
 #    pragma GCC diagnostic warning "-Wunknown-pragmas"
 #  endif
 #endif*/


#endif

