/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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

/**
 * \file Common.h
 * \brief This header should be included by each class header file.
 */

#ifndef SOLARUS_COMMON_H
#define SOLARUS_COMMON_H

#include "solarus/config.h"

/**
 * \cond doxygen_ignore
 */
#define SOLARUS_STRINGIFY1(x) #x
#define SOLARUS_STRINGIFY(x) SOLARUS_STRINGIFY1(x)
/**
 * \endcond
 */

/**
 * \brief Solarus major version.
 *
 * A change of major version is a important step in the maturity of the
 * project.
 * Data files other than scripts can be upgraded automatically.
 * Lua API changes may sometimes break compatibility, so check the ChangeLog
 * before upgrading.
 */
#define SOLARUS_MAJOR_VERSION 1

/**
 * \brief Solarus minor version.
 *
 * Minor versions add new features and may change the format of data files.
 * Data files other than scripts can be upgraded automatically.
 * Lua API changes may sometimes break compatibility, so check the ChangeLog
 * before upgrading.
 */
#define SOLARUS_MINOR_VERSION 5

/**
 * \brief Solarus patch version.
 *
 * Patch versions are only bug fixes.
 * Data files compatibility is always preserved between changes of the patch
 * version.
 */
#define SOLARUS_PATCH_VERSION 0

/**
 * \brief The Solarus version as a string.
 *
 * This string has the form "x.y.z" where x is the major version, y is the
 * minor version and z is the patch version.
 */
#define SOLARUS_VERSION SOLARUS_STRINGIFY(SOLARUS_MAJOR_VERSION) "." SOLARUS_STRINGIFY(SOLARUS_MINOR_VERSION) "." SOLARUS_STRINGIFY(SOLARUS_PATCH_VERSION)

/**
 * \brief The Solarus version as a string, without patch number.
 *
 * This string has the form "x.y" where x is the major version and y is the
 * minor version.
 */
#define SOLARUS_VERSION_WITHOUT_PATCH SOLARUS_STRINGIFY(SOLARUS_MAJOR_VERSION) "." SOLARUS_STRINGIFY(SOLARUS_MINOR_VERSION)

// Windows specific.
/**
 * \def SOLARUS_API
 * \brief Windows DLL import/export specifications for the Solarus library symbols.
 */
#ifdef _WIN32
#  ifdef solarus_EXPORTS  // Define automatically added by cmake.
#    define SOLARUS_API __declspec(dllexport)
#  else
#    define SOLARUS_API __declspec(dllimport)
#  endif
#else
#  define SOLARUS_API
#endif

// Define the current platform constants on Apple Systems.

/**
 * \cond doxygen_ignore
 * Define the current platform constants on Apple Systems.
 */
#if defined(__APPLE__)
#  include "TargetConditionals.h"
#  if TARGET_OS_IPHONE == 1
#    define SOLARUS_IOS
// TARGET_OS_MAC is set to 1 on both IPhone, IPhone simulator and Mac OS.
#  elif TARGET_OS_MAC == 1
#    define SOLARUS_OSX
#  endif
#endif
/**
 * \endcond
 */

/**
 * \def SOLARUS_HAVE_OPENGL
 * \brief Whether the system supports OpenGL as renderer driver.
 * This is optional, but allows to use advanced display features.
 *
 * This is not used for now and always disabled.
 * TODO use the standard configuration way
 */
#ifdef SOLARUS_HAVE_OPENGL
#  undef SOLARUS_HAVE_OPENGL
#endif
#define SOLARUS_HAVE_OPENGL 0

/**
 * \def SOLARUS_DEFAULT_QUEST
 * \brief Path of the quest to run is none is specified at runtime.
 */
#ifndef SOLARUS_DEFAULT_QUEST
// if no default quest was specified at compilation time,
// use the current directory
#  define SOLARUS_DEFAULT_QUEST "."
#endif

/**
 * \def SOLARUS_WRITE_DIR
 * \brief Where savegames are stored, relative to the user base write directory.
 */
#ifndef SOLARUS_WRITE_DIR
#  if defined(SOLARUS_OSX) || defined(SOLARUS_IOS)
#    define SOLARUS_WRITE_DIR "Solarus"
#  else
#    define SOLARUS_WRITE_DIR ".solarus"
#  endif
#endif

// Game size.

/**
 * \def SOLARUS_DEFAULT_QUEST_WIDTH
 * \brief Width of the quest screen in pixels if not set at runtime.
 */
#ifndef SOLARUS_DEFAULT_QUEST_WIDTH
#  if defined(PANDORA)
#    define SOLARUS_DEFAULT_QUEST_WIDTH 400
#  else
#    define SOLARUS_DEFAULT_QUEST_WIDTH 320
#  endif
#endif

/**
 * \def SOLARUS_DEFAULT_QUEST_HEIGHT
 * \brief Height of the quest screen in pixels if not set at runtime.
 */
#ifndef SOLARUS_DEFAULT_QUEST_HEIGHT
#  define SOLARUS_DEFAULT_QUEST_HEIGHT 240
#endif

// TODO see if this flag still works/is needed with 16bpp surface on Caanoo.
/**
 * \def SOLARUS_COLOR_DEPTH
 * \brief Number of bits per pixel for surfaces.
 */
#ifndef SOLARUS_COLOR_DEPTH
#  if defined(CAANOO)
#    define SOLARUS_COLOR_DEPTH 16
#  else
#    define SOLARUS_COLOR_DEPTH 32
#  endif
#endif

#endif

