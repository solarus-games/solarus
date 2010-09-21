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
#ifndef SOLARUS_RECTANGLE_H
#define SOLARUS_RECTANGLE_H

#include "Common.h"
#include <SDL/SDL.h>

/**
 * @brief Represents a rectangle.
 *
 * This class represents a rectangle with its top-left corner, width and height.
 * This class can also be used to represent a single point
 * (then the width and the height can be set to zero, one or even just left unset:
 * this is your decision).
 * This class basically encapsulates a library-dependent rectangle object.
 */
class Rectangle {

  // low-level classes allowed to manipulate directly the internal SDL rectangle encapsulated
  friend class Surface;

  private:

    SDL_Rect rect;		/**< the SDL_Rect encapsulated */

    SDL_Rect * get_internal_rect();

  public:

    Rectangle(int x = 0, int y = 0, int width = 0, int height = 0);
    Rectangle(const Rectangle &other);
    ~Rectangle();

    int get_x() const;
    int get_y() const;
    int get_width()  const;
    int get_height() const;

    void set_x(int x);
    void set_y(int y);
    void set_width(int width);
    void set_height(int height);

    void set_xy(int x, int y);
    void set_xy(const Rectangle &xy);
    void set_size(int width, int height);
    void set_size(const Rectangle &other);

    void add_x(int dx);
    void add_y(int dy);
    void add_width(int dw);
    void add_height(int dh);
    void add_xy(int dx, int dy);
    void add_xy(const Rectangle &other);

    bool contains(int x, int y) const;
    bool contains(const Rectangle &other) const;
    bool overlaps(const Rectangle &other) const;
    Rectangle get_center();
};

std::ostream & operator <<(std::ostream &stream, const Rectangle &rectangle);

/**
 * @brief Returns the x coordinate of the top-left corner of this rectangle.
 * @return the x coordinate of the top-left corner
 */
inline int Rectangle::get_x() const {
  return rect.x;
}

/**
 * @brief Returns the y coordinate of the top-left corner of this rectangle.
 * @return the y coordinate of the top-left corner
 */
inline int Rectangle::get_y() const {
  return rect.y;
}

/**
 * @brief Returns the width of this rectangle.
 * @return the width
 */
inline int Rectangle::get_width()  const {
  return rect.w;
}

/**
 * @brief Returns the height of this rectangle.
 * @return the height
 */
inline int Rectangle::get_height() const {
  return rect.h;
}

/**
 * @brief Sets the x coordinate of the top-left corner of this rectangle.
 * @param x the new x coordinate
 */
inline void Rectangle::set_x(int x) {
  rect.x = x;
}

/**
 * @brief Sets the y coordinate of the top-left corner of this rectangle.
 * @param y the new y coordinate
 */
inline void Rectangle::set_y(int y) {
  rect.y = y;
}

/**
 * @brief Sets the width of this rectangle.
 * @param width the new width
 */
inline void Rectangle::set_width(int width) {
  rect.w = width;
}

/**
 * @brief Sets the height of this rectangle.
 * @param height the new height
 */
inline void Rectangle::set_height(int height) {
  rect.h = height;
}


/**
 * @brief Sets the coordinates of the top-left corner of this rectangle
 * @param x the new x coordinate
 * @param y the new y coordinate
 */
inline void Rectangle::set_xy(int x, int y) {
  set_x(x);
  set_y(y);
}

/**
 * @brief Sets the coordinates of the top-left corner of this rectangle.
 *
 * The coordinates are represented as a Rectangle whose size is ignored.
 *
 * @param xy a rectangle whose coordinates should be copied from
 */
inline void Rectangle::set_xy(const Rectangle &xy) {
  set_xy(xy.get_x(), xy.get_y());
}

/**
 * @brief Sets the size of this rectangle.
 * @param width the new width
 * @param height the new height.
 */
inline void Rectangle::set_size(int width, int height) {
  set_width(width);
  set_height(height);
}

/**
 * @brief Sets the size of this rectangle.
 *
 * The size is represented as another rectangle whose coordinates are ignored.
 *
 * @param other a rectangle whose size should be copied from
 */
inline void Rectangle::set_size(const Rectangle &other) {
  set_size(other.get_width(), other.get_height());
}

/**
 * @brief Adds a value to the x coordinate of this rectangle.
 * @param dx the value to add
 */
inline void Rectangle::add_x(int dx) {
  set_x(get_x() + dx);
}

/**
 * @brief Adds a value to the y coordinate of this rectangle.
 * @param dy the value to add
 */
inline void Rectangle::add_y(int dy) {
  set_y(get_y() + dy);
}

/**
 * @brief Adds a value to the width of this rectangle.
 * @param dw the value to add
 */
inline void Rectangle::add_width(int dw) {
  set_width(get_width() + dw);
}

/**
 * @brief Adds a value to the height of this rectangle.
 * @param dh the value to add
 */
inline void Rectangle::add_height(int dh) {
  set_height(get_height() + dh);
}

/**
 * @brief Adds something to both coordinates of this rectangle.
 * @param dx change to the x coordinate
 * @param dy change to the y coordinate
 */
inline void Rectangle::add_xy(int dx, int dy) {
  add_x(dx);
  add_y(dy);
}

/**
 * @brief Adds something to both coordinates of this rectangle.
 *
 * The change of coordinates is represented as a rectangle whose size is ignored.
 *
 * @param dxy a rectangle whose coordinates should be added to the current's
 */
inline void Rectangle::add_xy(const Rectangle &dxy) {
  add_xy(dxy.get_x(), dxy.get_y());
}

#endif

