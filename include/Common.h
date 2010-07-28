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

#include <string>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include "Types.h"

/**
 * @brief Debug level.
 *
 * - 0: no debug (release mode): no error messages; pick the data only from the archive file; no debug keys; use only compiled (.luac) scripts
 * - >= 1: enable the error messages; pick the data also from the physical data directory; use clear text (.lua) scripts
 * - >= 2: enable the debug keys
 */
#define SOLARUS_DEBUG_LEVEL 2

/**
 * @brief Stops the program on an error message.
 *
 * This macro should be used to exit the program properly on an error message.
 * The message parameter can contain several elements separated by the '<<' operator.
 */
//#if SOLARUS_DEBUG_LEVEL == 0
//#define DIE(message) do { throw std::logic_error(""); } while (false)
//#else
#define DIE(message) do { std::cerr << message << std::endl; std::ostringstream oss; oss << message; throw std::logic_error(oss.str()); } while (false)
//#endif

#endif

