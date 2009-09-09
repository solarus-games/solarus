/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * This header should be included by each class header file.
 */

#ifndef ZSDX_COMMON_H
#define ZSDX_COMMON_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <stdexcept>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "Types.h"

#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

/**
 * Debug level.
 * 0: no debug (release mode): no error messages; pick the data only from the archive file; no debug keys; use clear text (.lua) scripts
 * >= 1: enable the error messages; pick the data also from the physical data directory; use only compiled (.luac) scripts
 * >= 2: enable the debug keys
 */
#define ZSDX_DEBUG_LEVEL 2

/**
 * This macro should be used to exit the program properly on an error message.
 * The message parameter can contain several elements separated by the '<<' operator.
 */
//#if ZSDX_DEBUG_LEVEL == 0
//#define DIE(message) do { throw std::logic_error(""); } while (false)
//#else
#define DIE(message) do { std::ostringstream oss; oss << message; throw std::logic_error(oss.str()); } while (false)
//#endif

#endif

