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
#ifndef SOLARUS_LOGGER_H
#define SOLARUS_LOGGER_H

#include "solarus/core/Common.h"
#include <iostream>
#include <string>

namespace Solarus {

/**
 * \brief Provides features for logging output and errors.
 *
 * Messages logged are all prepended by "[Solarus] [t] " where t is the current
 * simulated time.
 * This allows to better distinguish messages from the engine and messages
 * from the quest.
 */
namespace Logger {

SOLARUS_API void print(const std::string& message, std::ostream& out = std::cout);

SOLARUS_API void debug(const std::string& message);
SOLARUS_API void info(const std::string& message);
SOLARUS_API void warning(const std::string& message);
SOLARUS_API void error(const std::string& message);
SOLARUS_API void fatal(const std::string& message);

}  // namespace Logger

}  // namespace Solarus

#endif
