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

    Rectangle();
    Rectangle(int x, int y);
    explicit Rectangle(const Point& xy);
    explicit Rectangle(const Size& size);
    Rectangle(int x, int y, int width, int height);
    Rectangle(const Point& xy, const Size& size);

    int get_x() const;
    int get_y() const;
    Point get_xy() const;
    int get_width() const;
    int get_height() const;
    Size get_size() const;
    bool is_flat() const;

    void set_x(int x);
    void set_y(int y);
    void set_width(int width);
    void set_height(int height);

    void set_xy(int x, int y);
    void set_xy(const Point& xy);
    void set_size(int width, int height);
    void set_size(const Size& other);

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
    Point get_center() const;

    Rectangle get_intersection(const Rectangle& other) const;

  private:

    SDL_Rect* get_internal_rect();
    const SDL_Rect* get_internal_rect() const;

    SDL_Rect rect;      /**< the SDL_Rect encapsulated */

};

bool operator==(const Rectangle& lhs, const Rectangle& rhs);
bool operator!=(const Rectangle& lhs, const Rectangle& rhs);

std::ostream& operator<<(std::ostream& stream, const Rectangle& rectangle);

/**
 * \brief Creates a rectangle, without specifying its properties.
 */
inline Rectangle::Rectangle():
    Rectangle(0, 0, 0, 0)
{}

/**
 * \brief Creates a rectangle, specifying its position.
 * \param x x coordinate of the top-left corner
 * \param y y coordinate of the top-left corner
 */
inline Rectangle::Rectangle(int x, int y):
    Rectangle(x, y, 0, 0)
{}

/**
 * \brief Creates a rectangle, specifying its position.
 * \param xy coordinates of the top-left corner
 */
inline Rectangle::Rectangle(const Point& xy):
    Rectangle(xy.x, xy.y)
{}

/**
 * \brief Creates a rectangle, specifying its size.
 * \param size the rectangle's size
 */
inline Rectangle::Rectangle(const Size& size):
    Rectangle(0, 0, size.width, size.height)
{}

/**
 * \brief Creates a rectangle, specifying its properties.
 * \param x x coordinate of the top-left corner
 * \param y y coordinate of the top-left corner
 * \param width the rectangle's width
 * \param height the rectangle's height
 */
inline Rectangle::Rectangle(int x, int y, int width, int height) {

  set_xy(x, y);
  set_size(width, height);
}

/**
 * \brief Creates a rectangle, specifying its properties.
 * \param xy coordinates of the top-left corner
 * \param size the rectangle's size
 */
inline Rectangle::Rectangle(const Point& xy, const Size& size):
    Rectangle(xy.x, xy.y, size.width, size.height)
{}

/**
 * \brief Returns the x coordinate of the top-left corner of this rectangle.
 * \return the x coordinate of the top-left corner
 */
inline int Rectangle::get_x() const {
  return rect.x;
}

/**
 * \brief Returns the y coordinate of the top-left corner of this rectangle.
 * \return the y coordinate of the top-left corner
 */
inline int Rectangle::get_y() const {
  return rect.y;
}

/**
 * \brief Returns the coordinates of the top-left corner of this rectangle.
 * \return the coordinates of the top-left corner
 */
inline Point Rectangle::get_xy() const {
  return { get_x(), get_y() };
}

/**
 * \brief Returns the center point of this rectangle.
 * \return The center point.
 */
inline Point Rectangle::get_center() const {

  return {
      get_x() + get_width() / 2,
      get_y() + get_height() / 2,
  };
}

/**
 * \brief Returns the width of this rectangle.
 * \return the width
 */
inline int Rectangle::get_width()  const {
  return rect.w;
}

/**
 * \brief Returns the height of this rectangle.
 * \return the height
 */
inline int Rectangle::get_height() const {
  return rect.h;
}

/**
 * \brief Returns the size of this rectangle.
 * \return the size
 */
inline Size Rectangle::get_size() const {
  return { get_width(), get_height() };
}

/**
 * \brief Returns whether this rectangle is flat.
 * \return true if the width or the height is 0.
 */
inline bool Rectangle::is_flat() const {
  return get_width() == 0 || get_height() == 0;
}

/**
 * \brief Sets the x coordinate of the top-left corner of this rectangle.
 * \param x the new x coordinate
 */
inline void Rectangle::set_x(int x) {
  rect.x = x;
}

/**
 * \brief Sets the y coordinate of the top-left corner of this rectangle.
 * \param y the new y coordinate
 */
inline void Rectangle::set_y(int y) {
  rect.y = y;
}

/**
 * \brief Sets the width of this rectangle.
 * \param width the new width
 */
inline void Rectangle::set_width(int width) {
  rect.w = width;
}

/**
 * \brief Sets the height of this rectangle.
 * \param height the new height
 */
inline void Rectangle::set_height(int height) {
  rect.h = height;
}

/**
 * \brief Sets the coordinates of the top-left corner of this rectangle
 * \param x the new x coordinate
 * \param y the new y coordinate
 */
inline void Rectangle::set_xy(int x, int y) {
  set_x(x);
  set_y(y);
}

/**
 * \brief Sets the coordinates of the top-left corner of this rectangle.
 * \param xy the new coordinates
 */
inline void Rectangle::set_xy(const Point& xy) {
  set_xy(xy.x, xy.y);
}

/**
 * \brief Sets the size of this rectangle.
 * \param width the new width
 * \param height the new height
 */
inline void Rectangle::set_size(int width, int height) {
  set_width(width);
  set_height(height);
}

/**
 * \brief Sets the size of this rectangle.
 * \param size the new size
 */
inline void Rectangle::set_size(const Size& size) {
  set_size(size.width, size.height);
}

/**
 * \brief Adds a value to the x coordinate of this rectangle.
 * \param dx the value to add
 */
inline void Rectangle::add_x(int dx) {
  set_x(get_x() + dx);
}

/**
 * \brief Adds a value to the y coordinate of this rectangle.
 * \param dy the value to add
 */
inline void Rectangle::add_y(int dy) {
  set_y(get_y() + dy);
}

/**
 * \brief Adds a value to the width of this rectangle.
 * \param dw the value to add
 */
inline void Rectangle::add_width(int dw) {
  set_width(get_width() + dw);
}

/**
 * \brief Adds a value to the height of this rectangle.
 * \param dh the value to add
 */
inline void Rectangle::add_height(int dh) {
  set_height(get_height() + dh);
}

/**
 * \brief Adds something to both coordinates of this rectangle.
 * \param dx change to the x coordinate
 * \param dy change to the y coordinate
 */
inline void Rectangle::add_xy(int dx, int dy) {
  add_x(dx);
  add_y(dy);
}

/**
 * \brief Adds something to both coordinates of this rectangle.
 * \param dxy coordinates that should be added to the current's
 */
inline void Rectangle::add_xy(const Point& dxy) {
  add_xy(dxy.x, dxy.y);
}

/**
 * \brief Returns whether the specified point is inside this rectangle.
 * \param x x coordinate of the point
 * \param y y coordinate of the point
 * \return true if the point is in this rectangle
 */
inline bool Rectangle::contains(int x, int y) const {
  return x >= get_x() && x < get_x() + get_width()
      && y >= get_y() && y < get_y() + get_height();
}

/**
 * \brief Returns whether the specified point is inside this rectangle.
 * \param point point that may be in this rectangle
 * \return true if \a point is in this rectangle
 */
inline bool Rectangle::contains(const Point& point) const {
  return contains(point.x, point.y);
}

/**
 * \brief Returns whether the specified rectangle is inside this rectangle.
 * \param other another rectangle
 * \return true if the specified rectangle is inside this rectangle
 */
inline bool Rectangle::contains(const Rectangle& other) const {
  return contains(other.get_x(), other.get_y())
      && contains(other.get_x() + other.get_width() - 1, other.get_y() + other.get_height() - 1);
}

/**
 * \brief Returns whether or not another rectangle overlaps this one.
 * \param other another rectangle
 * \return true if the two rectangles overlap
 */
inline bool Rectangle::overlaps(const Rectangle& other) const {

  int x1 = get_x();
  int x2 = x1 + get_width();
  int x3 = other.get_x();
  int x4 = x3 + other.get_width();

  bool overlap_x = (x3 < x2 && x1 < x4);

  int y1 = get_y();
  int y2 = y1 + get_height();
  int y3 = other.get_y();
  int y4 = y3 + other.get_height();

  bool overlap_y = (y3 < y2 && y1 < y4);

  return overlap_x && overlap_y && !is_flat() && !other.is_flat();
}

/**
 * \brief returns the sdl rectangle encapsulated by this object.
 *
 * This function must be used only by other low-level classes
 * (typically surface) as it is library dependent.
 *
 * \return the internal rectangle encapsulated
 */
inline SDL_Rect* Rectangle::get_internal_rect() {
  return &rect;
}

/**
 * \brief returns the sdl rectangle encapsulated by this object.
 *
 * This function must be used only by other low-level classes
 * (typically surface) as it is library dependent.
 *
 * \return the internal rectangle encapsulated
 */
inline const SDL_Rect* Rectangle::get_internal_rect() const {
  return &rect;
}

/**
 * \brief Returns the intersection between this rectangle and another one.
 * \param other Another rectangle.
 * \return The intersection or an empty rectangle.
 */
inline Rectangle Rectangle::get_intersection(const Rectangle& other) const {

  Rectangle intersection;
  const bool intersects = SDL_IntersectRect(
      this->get_internal_rect(),
      other.get_internal_rect(),
      intersection.get_internal_rect()
  );
  if (intersects) {
    return intersection;
  }
  else {
    return Rectangle(0, 0, 0, 0);
  }
}

/**
 * \brief Compares two rectangles.
 * \param lhs first rectangle
 * \param rhs second rectangle
 * \return true if both rectangles have the same coordinates and size
 */
inline bool operator==(const Rectangle& lhs, const Rectangle& rhs) {
  return lhs.get_xy() == rhs.get_xy()
      && lhs.get_size() == rhs.get_size();
}

/**
 * \brief Compares two rectangles.
 * \param lhs first rectangle
 * \param rhs second rectangle
 * \return true if the rectangles are not equal
 */
inline bool operator!=(const Rectangle& lhs, const Rectangle& rhs) {
  return !(rhs == lhs);
}

}

#endif
