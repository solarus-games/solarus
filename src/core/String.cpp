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
#include "solarus/core/String.h"
#include <sstream>

namespace Solarus {

namespace String {

/**
 * \brief Converts an integer to a string.
 *
 * This function replaces std::to_string which is missing on some systems.
 *
 * \param value The value to convert.
 * \return String representation of the value.
 */
SOLARUS_API std::string to_string(int value) {

  std::ostringstream oss;
  oss << value;
  return oss.str();
}

}  // namespace String

}  // namespace Solarus
