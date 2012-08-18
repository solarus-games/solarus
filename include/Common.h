/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
 * @file Common.h
 * @brief This header should be included by each class header file.
 */

#ifndef SOLARUS_COMMON_H
#define SOLARUS_COMMON_H

#ifndef DEFAULT_QUEST
/**
 * @brief Path of the quest to run when none is specified in the command line
 */
#define DEFAULT_QUEST "."
#endif

#ifndef SOLARUS_WRITE_DIR
/**
 * @brief Where savegames are stored, relative to the user's home directory.
 */
#define SOLARUS_WRITE_DIR ".solarus"
#endif

// Game size.
#ifndef SOLARUS_SCREEN_WIDTH
#  ifdef PANDORA
#    define SOLARUS_SCREEN_WIDTH 400
#  else
#    define SOLARUS_SCREEN_WIDTH 320
#  endif
#endif

#ifndef SOLARUS_SCREEN_HEIGHT
#  define SOLARUS_SCREEN_HEIGHT 240
#endif

// Bits per pixel.
#ifndef SOLARUS_COLOR_DEPTH
#  if defined(CAANOO) || defined(PANDORA)
#    define SOLARUS_COLOR_DEPTH 16
#  else
#    define SOLARUS_COLOR_DEPTH 32
#  endif
#endif

#define SOLARUS_SCREEN_WIDTH_MIDDLE (SOLARUS_SCREEN_WIDTH / 2)
#define SOLARUS_SCREEN_HEIGHT_MIDDLE (SOLARUS_SCREEN_HEIGHT / 2)

// Bits per pixel.
#ifndef SOLARUS_COLOR_DEPTH
#  if defined(CAANOO) || defined(PANDORA)
#    define SOLARUS_COLOR_DEPTH 16
#  else
#    define SOLARUS_COLOR_DEPTH 32
#  endif
#endif

// Force a unique video mode.
#ifndef SOLARUS_SCREEN_FORCE_MODE
#  ifdef(CAANOO)
#    define SOLARUS_SCREEN_FORCE_MODE 2
#  elif defined(PANDORA)
#    define SOLARUS_SCREEN_FORCE_MODE 5
#  endif
#endif

// Force a software surface for the screen.
#ifndef SOLARUS_SCREEN_SOFTWARE_SURFACE
#  ifdef __APPLE__
// On Mac OS X the SDL hardware surfaces are buggy.
#    define SOLARUS_SCREEN_SOFTWARE_SURFACE
#  endif
#endif

#include "Types.h"

#endif

