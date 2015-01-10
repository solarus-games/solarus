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
#ifndef SOLARUS_SIZE_H
#define SOLARUS_SIZE_H

#include "solarus/Common.h"
#include <iosfwd>

namespace Solarus {

/**
 * \brief Represents a 2D size.
 *
 * This class represents a generic size with
 * width and height properties.
 */
class Size {

  public:
    ////////////////////////////////////////////////////////////
    // Constructors

    constexpr Size();
    constexpr Size(int width, int height);

    ////////////////////////////////////////////////////////////
    // Operators

    Size& operator+=(const Size& other);
    Size& operator-=(const Size& other);

    Size& operator*=(int factor);
    Size& operator/=(int divisor);

    ////////////////////////////////////////////////////////////
    // Miscellaneous functions

    constexpr bool is_flat() const;
    constexpr bool is_square() const;

    ////////////////////////////////////////////////////////////
    // Member data

    int width = 0;    /**< Width of an object */
    int height = 0;   /**< Height of an object */

};

////////////////////////////////////////////////////////////
// Free functions

// Comparison operators
bool operator==(const Size& lhs, const Size& rhs);
bool operator!=(const Size& lhs, const Size& rhs);

// Arithmetic operators
Size operator+(Size lhs, const Size& rhs);
Size operator-(Size lhs, const Size& rhs);

Size operator*(Size size, int factor);
Size operator*(int factor, Size size);
Size operator/(Size size, int divisor);

// Output to stream
std::ostream& operator<<(std::ostream& stream, const Size& size);

}

#include "solarus/lowlevel/Size.inl"

#endif

