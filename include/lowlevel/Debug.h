/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_DEBUG_H
#define SOLARUS_DEBUG_H

#include "Common.h"
#include <string>

#ifndef NDEBUG
#define SOLARUS_ASSERT(condition, message) debug::check_assertion(condition, message)
#else
#define SOLARUS_ASSERT(condition, message)
#endif

namespace solarus {

/**
 * \brief Provides features for printing error messages or making runtime checks.
 */
namespace debug {

void warning(const std::string& message);
void error(const std::string& message);
void check_assertion(bool assertion, const char* error_message);
void check_assertion(bool assertion, const std::string& error_message);
void die(const std::string& error_message);

}

}

#endif

