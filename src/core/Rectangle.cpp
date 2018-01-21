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
#include "solarus/core/Rectangle.h"
#include <ostream>

namespace Solarus {

/**
 * \brief Prints a rectangle to an output stream.
 * \param stream The stream.
 * \param rectangle A rectangle.
 * \return The output stream.
 */
std::ostream& operator<<(std::ostream& stream, const Rectangle& rectangle) {

  stream << rectangle.get_xy() << "x" << rectangle.get_size();
  return stream;
}

}

