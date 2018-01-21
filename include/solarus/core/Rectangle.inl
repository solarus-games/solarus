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

namespace Solarus {

/**
 * \brief Creates a rectangle, without specifying its properties.
 */
constexpr Rectangle::Rectangle():
    rect({0, 0, 0, 0})
{}

/**
 * \brief Creates a rectangle, specifying its position.
 * \param x x coordinate of the top-left corner
 * \param y y coordinate of the top-left corner
 */
constexpr Rectangle::Rectangle(int x, int y):
    rect({x, y, 0, 0})
{}

/**
 * \brief Creates a rectangle, specifying its position.
 * \param xy coordinates of the top-left corner
 */
constexpr Rectangle::Rectangle(const Point& xy):
    Rectangle(xy.x, xy.y)
{}

/**
 * \brief Creates a rectangle, specifying its size.
 * \param size the rectangle's size
 */
constexpr Rectangle::Rectangle(const Size& size):
    rect({0, 0, size.width, size.height})
{}

/**
 * \brief Creates a rectangle, specifying its properties.
 * \param x x coordinate of the top-left corner
 * \param y y coordinate of the top-left corner
 * \param width the rectangle's width
 * \param height the rectangle's height
 */
constexpr Rectangle::Rectangle(int x, int y, int width, int height):
    rect({x, y, width, height})
{}

/**
 * \brief Creates a rectangle, specifying its properties.
 * \param xy coordinates of the top-left corner
 * \param size the rectangle's size
 */
constexpr Rectangle::Rectangle(const Point& xy, const Size& size):
    Rectangle(xy.x, xy.y, size.width, size.height)
{}

/**
 * \brief Creates a rectangle, specifying its properties.
 *
 * This is the equivalent of:
 * Rectangle(top_left.x, top_left.y,
 *           bottom_right.x - top_left.x , bottom_right.y - top_left.y)
 *
 * \param top_left coordinates of the top-left corner
 * \param bottom_right coordinates of the bottom-right corner
 */
constexpr Rectangle::Rectangle(const Point& top_left, const Point& bottom_right):
    Rectangle(top_left.x, top_left.y,
              bottom_right.x - top_left.x , bottom_right.y - top_left.y)
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
 * \brief Returns the bottom coordinate of this rectangle.
 *
 * This is the equivalent of: get_top() + get_height()
 *
 * \return The bottom coordinate.
 */
inline int Rectangle::get_bottom() const {
  return get_top() + get_height();
}

/**
 * \brief Returns the bottom-left point of this rectangle.
 *
 * This is the equivalent of: Point(get_left(), get_bottom())
 *
 * \return The bottom-left point.
 */
inline Point Rectangle::get_bottom_left() const {
  return { get_left(), get_bottom() };
}

/**
 * \brief Returns the bottom-right point of this rectangle.
 *
 * This is the equivalent of: Point(get_right(), get_bottom())
 *
 * \return The bottom-right point.
 */
inline Point Rectangle::get_bottom_right() const {
  return { get_right(), get_bottom() };
}

/**
 * \brief Returns the center point of this rectangle.
 * \return The center point.
 */
inline Point Rectangle::get_center() const {
  return {
      get_left() + get_width() / 2,
      get_top() + get_height() / 2,
  };
}

/**
 * \brief Returns the left coordinate of this rectangle.
 * \return The left coordinate.
 */
inline int Rectangle::get_left() const {
  return rect.x;
}

/**
 * \brief Returns the right coordinate of this rectangle.
 *
 * This is the equivalent of: get_left() + get_width()
 *
 * \return The right coordinate.
 */
inline int Rectangle::get_right() const {
  return get_left() + get_width();
}

/**
 * \brief Returns the top coordinate of this rectangle.
 * \return The top coordinate.
 */
inline int Rectangle::get_top() const {
  return rect.y;
}

/**
 * \brief Returns the top-left point of this rectangle.
 *
 * This is the equivalent of: Point(get_left(), get_top())
 *
 * \return The top-left point.
 */
inline Point Rectangle::get_top_left() const {
  return { get_left(), get_top() };
}

/**
 * \brief Returns the top-right point of this rectangle.
 *
 * This is the equivalent of: Point(get_right(), get_top())
 *
 * \return The top-right point.
 */
inline Point Rectangle::get_top_right() const {
  return { get_right(), get_top() };
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
 * \brief Creates a rectangle, specifying its center.
 * \param x x coordinate of the center
 * \param y y coordinate of the center
 */
inline void Rectangle::set_center(int x, int y) {
  set_xy(x - rect.w/2, y - rect.h/2);
}

/**
 * \brief Creates a rectangle, specifying its center and size.
 * \param center coordinate of the rectangle center.
 */
inline void Rectangle::set_center(const Point &xy) {
  return set_center(xy.x, xy.y);
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
  return *this & other;
}

/**
 * \brief Returns the intersection between this rectangle and another one.
 * \param other Another rectangle.
 * \return The intersection or an empty rectangle.
 */
inline Rectangle Rectangle::operator&(const Rectangle& other) const {
  Rectangle copy(*this);
  return copy &= other;
}

/**
 * \brief Intersects this rectangle with another one.
 * \param other Another rectangle.
 * \return This rectangle after modification.
 */
inline Rectangle& Rectangle::operator&=(const Rectangle& other) {

  Rectangle intersection;
  const bool intersects = SDL_IntersectRect(
      this->get_internal_rect(),
      other.get_internal_rect(),
      intersection.get_internal_rect()
  );
  if (intersects) {
    *this = intersection;
  }
  else {
    *this = Rectangle(0, 0, 0, 0);
  }
  return *this;
}

/**
 * \brief Returns the union between this rectangle and another one.
 * \param other Another rectangle.
 * \return The smallest rectangle that contains both.
 */
inline Rectangle Rectangle::operator|(const Rectangle& other) const {
  Rectangle copy(*this);
  return copy |= other;
}

/**
 * \brief Unites this rectangle with another one.
 * \param other Another rectangle.
 * \return This rectangle after modification.
 */
inline Rectangle& Rectangle::operator|=(const Rectangle& other) {

  Rectangle result;
  SDL_UnionRect(
      this->get_internal_rect(),
      other.get_internal_rect(),
      result.get_internal_rect()
  );
  *this = result;
  return *this;
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
