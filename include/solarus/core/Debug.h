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
#ifndef SOLARUS_DEBUG_H
#define SOLARUS_DEBUG_H

#include "solarus/core/Common.h"
#include <string>

#ifndef NDEBUG
#define SOLARUS_ASSERT(condition, message) Debug::check_assertion(condition, message)
#else
#define SOLARUS_ASSERT(condition, message)
#endif

namespace Solarus {

class CommandLine;

/**
 * \brief Provides features for handling errors.
 */
namespace Debug {

SOLARUS_API void set_die_on_error(bool die);
SOLARUS_API void set_show_popup_on_die(bool show);
SOLARUS_API void set_abort_on_die(bool abort);

SOLARUS_API void warning(const std::string& message);
SOLARUS_API void error(const std::string& message);
SOLARUS_API void check_assertion(bool assertion, const char* error_message);
SOLARUS_API void check_assertion(bool assertion, const std::string& error_message);
[[noreturn]] SOLARUS_API void die(const std::string& error_message);

/**
 * \brief Execute an arbitrary function in debug mode.
 */
template<typename Function>
void execute_if_debug(Function&& func)
{
#ifndef NDEBUG
    func();
#else
    (void) func;
#endif
}
}

}

#endif

