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
#ifndef SOLARUS_COLOR_H
#define SOLARUS_COLOR_H

#include "solarus/Common.h"
#include <cstdint>
#include <SDL.h>

namespace Solarus {

/**
 * \brief Represents a color.
 *
 * This class simply wraps the RGBA components of a color.
 */
class SOLARUS_API Color {

  public:

    Color();
    Color(int r, int g, int b, int a = 255);

    uint8_t get_alpha() const;
    void set_alpha(uint8_t alpha);
    void get_components(uint8_t& r, uint8_t& g, uint8_t& b, uint8_t& a) const;
    void set_components(int r, int g, int b, int a = 255);

    // Some predefined colors.
    static const Color transparent;
    static const Color black;
    static const Color white;
    static const Color red;
    static const Color green;
    static const Color blue;
    static const Color yellow;
    static const Color magenta;
    static const Color cyan;

  private:

    friend bool operator==(const Color& lhs, const Color& rhs);

    uint8_t r;     /**< The red component. */
    uint8_t g;     /**< The green component. */
    uint8_t b;     /**< The blue component. */
    uint8_t a;     /**< The alpha (opacity) component. 255 is opaque. */
};

bool operator==(const Color& lhs, const Color& rhs);

}

#endif

