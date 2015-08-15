/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_RECTANGLE_H
#define SOLARUS_RECTANGLE_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/lowlevel/Size.h"
#include <SDL_rect.h>
#include <iosfwd>

namespace Solarus {

/**
 * \brief Represents a rectangle.
 *
 * This class represents a rectangle with its upper-left corner, width and height.
 * This class can also be used to represent a single point
 * (then the width and the height are 1).
 * This class basically encapsulates a library-dependent rectangle object.
 */
class Rectangle {

  // low-level classes allowed to manipulate directly the internal SDL rectangle encapsulated
  friend class Surface;
  friend class Video;

  public:

    constexpr Rectangle();
    constexpr Rectangle(int x, int y);
    explicit constexpr Rectangle(const Point& xy);
    explicit constexpr Rectangle(const Size& size);
    constexpr Rectangle(int x, int y, int width, int height);
    constexpr Rectangle(const Point& xy, const Size& size);

    int get_x() const;
    int get_y() const;
    Point get_xy() const;
    int get_width() const;
    int get_height() const;
    Size get_size() const;
    bool is_flat() const;
    Point get_center() const;

    void set_x(int x);
    void set_y(int y);
    void set_width(int width);
    void set_height(int height);

    void set_xy(int x, int y);
    void set_xy(const Point& xy);
    void set_size(int width, int height);
    void set_size(const Size& other);
    void set_center(int x, int y);
    void set_center(const Point& xy);

    void add_x(int dx);
    void add_y(int dy);
    void add_width(int dw);
    void add_height(int dh);
    void add_xy(int dx, int dy);
    void add_xy(const Point& other);

    bool contains(int x, int y) const;
    bool contains(const Point& point) const;
    bool contains(const Rectangle& other) const;
    bool overlaps(const Rectangle& other) const;

    Rectangle get_intersection(const Rectangle& other) const;

  private:

    SDL_Rect* get_internal_rect();
    const SDL_Rect* get_internal_rect() const;

    SDL_Rect rect;      /**< the SDL_Rect encapsulated */

};

bool operator==(const Rectangle& lhs, const Rectangle& rhs);
bool operator!=(const Rectangle& lhs, const Rectangle& rhs);

SOLARUS_API std::ostream& operator<<(std::ostream& stream, const Rectangle& rectangle);

}

#include "solarus/lowlevel/Rectangle.inl"

#endif
