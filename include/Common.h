/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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

#include "Types.h"

/**
 * @brief Debug level.
 *
 * - 0 (no debug): no error messages; pick the data only from the archive file; no debug keys;
 * - >= 1: enable the error messages; pick the data also from the physical data directory;
 * - >= 2: enable the debug keys
 *
 * 0 or 1 are ok for a release.
 * TODO: allow to change this value at configure time with a cmake option
 * TODO: remove level 0 which we never use: error messages are always useful
 * and forcing the access by archive is a stupid restriction
 */
#define SOLARUS_DEBUG_LEVEL 1

#endif

