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
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Random.h"
#include "solarus/lowlevel/Surface.h"
#include <algorithm>
#include <set>

namespace Solarus {

/**
 * \brief Creates a quadtree with a default space size.
 *
 * Call initialize() later to specify the size.
 */
template<typename T>
Quadtree<T>::Quadtree() :
    Quadtree(Rectangle(0, 0, 256, 256)) {

}

/**
 * \brief Creates a quadtree and initializes with the given size.
 * \param space Rectangle representing the space to create partitions of.
 */
template<typename T>
Quadtree<T>::Quadtree(const Rectangle& space) :
    elements(),
    root() {

    initialize(space);
}

/**
 * \brief Removes all elements of the quadtree.
 */
template<typename T>
void Quadtree<T>::clear() {

  elements.clear();
  root.clear();
}

/**
 * \brief Clears the quadtree and initializes with a new size.
 * \param space Rectangle representing the space to create partitions of.
 */
template<typename T>
void Quadtree<T>::initialize(const Rectangle& space) {

  clear();

  // Expand the space so that it is square.
  Rectangle square = space;
  if (space.get_width() > space.get_height()) {
    square.set_y(square.get_center().y - square.get_width() / 2);
    square.set_height(square.get_width());
  }
  else {
    square.set_x(square.get_center().x - square.get_height() / 2);
    square.set_width(square.get_height());
  }

  root.initialize(square);
}

/**
 * \brief Returns the space partitioned by this quadtree.
 * \return The partitioned space.
 */
template<typename T>
Rectangle Quadtree<T>::get_space() const {
    return root.get_cell();
}

/**
 * \brief Adds an element to the quadtree.
 * \param element The element to add.
 * \param bounding_box Bounding box of the element.
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::add(const T& element, const Rectangle& bounding_box) {

  if (elements.find(element) != elements.end()) {
    // Element already in the quadtree.
    return false;
  }

  ElementInfo info;
  info.bounding_box = bounding_box;
  elements.insert(std::make_pair(element, info));

  return root.add(element, bounding_box);
}

/**
 * \brief Removes an element from the quadtree.
 * \param element The element to remove.
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::remove(const T& element) {

  const auto& it = elements.find(element);
  if (it == elements.end()) {
    // Unknown element.
    return false;
  }

  Rectangle box = it->second.bounding_box;
  elements.erase(element);
  return root.remove(element, box);
}

/**
 * \brief Moves the element in the quadtree.
 *
 * This function should be called when the position or size or the element
 * is changed.
 *
 * \param element The element to move.
 * \param bounding_box New bounding box of the element.
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::move(const T& element, const Rectangle& bounding_box) {

  const auto& it = elements.find(element);
  if (it == elements.end()) {
    // Entity not in the quadtree.
    return false;
  }

  if (it->second.bounding_box == bounding_box) {
    // No change.
    return true;
  }

  if (!remove(element)) {
    return false;
  }
  return add(element, bounding_box);
}

/**
 * \brief Returns the total number of elements in the quadtree.
 * \return The number of elements.
 */
template<typename T>
int Quadtree<T>::get_num_elements() const {
  return root.get_num_elements();
}

/**
 * \brief Gets the elements intersecting the given rectangle.
 * \param[in] region The rectangle to check.
 * \param[in/out] elements A list that will be filled with elements.
 */
template<typename T>
void Quadtree<T>::get_elements(
    const Rectangle& region,
    std::vector<T>& elements
) const {
  root.get_elements(region, elements);
}

/**
 * \brief Draws the quadtree on a surface for debugging purposes.
 * \param dst_surface The destination surface.
 * \param dst_position Where to draw on that surface.
 */
template<typename T>
void Quadtree<T>::draw(const SurfacePtr& dst_surface, const Point& dst_position) {

  root.draw(dst_surface, dst_position);
}

/**
 * \brief Creates a node with default coordinates.
 *
 * Call initialize later to set coordinates.
 */
template<typename T>
Quadtree<T>::Node::Node() :
    Node(Rectangle(0, 0, 256, 256)) {

}

/**
 * \brief Creates a node with the given coordinates.
 * \param cell Cell coordinates of the node.
 */
template<typename T>
Quadtree<T>::Node::Node(const Rectangle& cell) :
    elements(),
    children(),
    cell(cell),
    center(cell.get_center()),
    color() {

  initialize(cell);

  if (debug_quadtrees) {
    color = Color(Random::get_number(256), Random::get_number(256), Random::get_number(256));
  }
}

/**
 * \brief Clears this node.
 *
 * Children nodes and their content are destroyed.
 */
template<typename T>
void Quadtree<T>::Node::clear() {

  elements.clear();
  children[0] = nullptr;
  children[1] = nullptr;
  children[2] = nullptr;
  children[3] = nullptr;
}

/**
 * \brief Clears this node and initializes it with a new cell rectangle.
 * \param cell The new cell.
 */
template<typename T>
void Quadtree<T>::Node::initialize(const Rectangle& cell) {

  clear();
  this->cell = cell;
  elements.reserve(max_in_cell);
}

/**
 * \brief Returns the cell represented by this node.
 * \return The cell's rectangle.
 */
template<typename T>
Rectangle Quadtree<T>::Node::get_cell() const {
  return cell;
}

/**
 * \brief Returns the size of the cell represented by this node.
 * \return The cell size.
 */
template<typename T>
Size Quadtree<T>::Node::get_cell_size() const {
    return get_cell().get_size();
}

/**
 * \brief Adds an element to this node if its bounding box intersects it.
 *
 * Splits the node if necessary when the threshold is exceeded.
 *
 * \param element The element to add.
 * \param bounding_box Bounding box of the element.
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::Node::add(
    const T& element,
    const Rectangle& bounding_box
) {
  if (!get_cell().overlaps(bounding_box)) {
    // Nothing to do.
    return false;
  }

  if (!is_split()) {

    // See if it is time to split.
    if (get_cell().contains(bounding_box.get_center())) {
      // We are the main cell of this element: it counts in the total.
      if (get_num_elements() >= max_in_cell &&
          get_cell_size().width > min_cell_size &&
          get_cell_size().height > min_cell_size) {
        split();
      }
    }
  }

  if (!is_split()) {
    // Add it to the current node.
    elements.push_back(std::make_pair(element, bounding_box));
    return true;
  }

  // Add it to children cells.
  for (const std::unique_ptr<Node>& child : children) {
    child->add(element, bounding_box);
  }
  return true;
}

/**
 * \brief Removes an element from this node if its bounding box intersects it.
 *
 * Merges nodes when necessary.
 *
 * \param element The element to remove.
 * \param bounding_box Bounding box of the element.
 * \return \c true in the element was found and removed.
 */
template<typename T>
bool Quadtree<T>::Node::remove(
    const T& element,
    const Rectangle& bounding_box
) {
  if (!get_cell().overlaps(bounding_box)) {
    // Nothing to do.
    return false;
  }

  if (!is_split()) {
    // Remove from this cell.
    const auto& it = std::find(elements.begin(), elements.end(), std::make_pair(element, bounding_box));
    if (it == elements.end()) {
      // The element was not here.
      return false;
    }
    elements.erase(it);
    return true;
  }

  // Remove from children cells.
  bool removed = false;
  for (const std::unique_ptr<Node>& child : children) {
    removed |= child->remove(element, bounding_box);
  }

  if (removed &&
      !children[0]->is_split()  // We are the parent node of where the element was removed.
  ) {
    // See if it is time to merge.
    int num_elements_in_children = get_num_elements();
    if (num_elements_in_children < min_in_4_cells) {
      merge();
    }
  }
  return removed;
}

/**
 * \brief Returns whether this node is split or is a leaf cell.
 * \return \c true if the node is split.
 */
template<typename T>
bool Quadtree<T>::Node::is_split() const {

  return children[0] != nullptr;
}

/**
 * \brief Splits this cell in four parts and moves its elements to them.
 */
template<typename T>
void Quadtree<T>::Node::split() {

  Debug::check_assertion(!is_split(), "Quadtree node already split");

  // Create 4 children cells.
  const Rectangle& cell = get_cell();
  const Point& center = cell.get_center();
  children[0] = std::unique_ptr<Node>(
      new Node(Rectangle(cell.get_top_left(), center))
  );
  children[1] = std::unique_ptr<Node>(
      new Node(Rectangle(Point(center.x, cell.get_top()), Point(cell.get_right(), center.y)))
  );
  children[2] = std::unique_ptr<Node>(
      new Node(Rectangle(Point(cell.get_left(), center.y), Point(center.x, cell.get_bottom())))
  );
  children[3] = std::unique_ptr<Node>(
      new Node(Rectangle(center, cell.get_bottom_right()))
  );

  // Move existing elements into them.
  for (const std::pair<T, Rectangle>& pair: elements) {
    for (const std::unique_ptr<Node>& child : children) {
      child->add(pair.first, pair.second);
    }
  }
  elements.clear();

  Debug::check_assertion(is_split(), "Quadtree node split failed");
}

/**
 * \brief Merges the four children cell into this one and destroys them.
 *
 * The children must already be leaves.
 */
template<typename T>
void Quadtree<T>::Node::merge() {

  Debug::check_assertion(is_split(), "Quadtree node already merged");

  // We want to avoid duplicates while preserving a deterministic order.
  std::set<T> merged_elements;
  for (const std::unique_ptr<Node>& child : children) {
    Debug::check_assertion(!child->is_split(), "Quadtree node child is not a leaf");
    for (const std::pair<T, Rectangle>& pair: child->elements) {
      const T& element = pair.first;
      if (merged_elements.insert(element).second) {
        elements.push_back(pair);
      }
    }
  }

  for (std::unique_ptr<Node>& child : children) {
    child = nullptr;
  }

  Debug::check_assertion(!is_split(), "Quadtree node merge failed");
}

/**
 * \brief Returns the number of elements whose center is under this node.
 * \return The number of elements under this node.
 */
template<typename T>
int Quadtree<T>::Node::get_num_elements() const {

  int num_elements = 0;
  if (!is_split()) {
    // Some elements can overlap several cells.
    // To avoid duplicates, we count an element if its center is in this cell.
    // TODO This information could be stored for better performance.
    for (const std::pair<T, Rectangle>& pair: elements) {
      const Rectangle& box = pair.second;
      if (get_cell().contains(box.get_center())) {
        ++num_elements;
      }
    }
  }
  else {
    // Ask children.
    for (const std::unique_ptr<Node>& child : children) {
      num_elements += child->get_num_elements();
    }
  }
  return num_elements;
}

/**
 * \brief Gets the elements intersecting the given rectangle under this node.
 * \param[in] region The rectangle to check.
 * \param[in/out] result A list that will be filled with elements.
 */
template<typename T>
void Quadtree<T>::Node::get_elements(
    const Rectangle& region,
    std::vector<T>& result
) const {

  if (!get_cell().overlaps(region)) {
    // Nothing here.
    return;
  }

  if (!is_split()) {
    for (const std::pair<T, Rectangle>& pair : elements) {
      if (pair.second.overlaps(region)) {
        result.push_back(pair.first);
      }
    }
  }
  else {
    // Get from from children cells.
    for (const std::unique_ptr<Node>& child : children) {
      child->get_elements(region, result);
    }
  }
}

/**
 * \brief Draws the node on a surface for debugging purposes.
 * \param dst_surface The destination surface.
 * \param dst_position Where to draw on that surface.
 */
template<typename T>
void Quadtree<T>::Node::draw(const SurfacePtr& dst_surface, const Point& dst_position) {

  if (!is_split()) {
    // Draw the rectangle of the node.
    draw_rectangle(get_cell(), color, dst_surface, dst_position);

    // Draw bounding boxes of elements.
    for (const std::pair<T, Rectangle>& pair: elements) {
      const Rectangle& bounding_box = pair.second;
      if (get_cell().contains(bounding_box.get_center())) {
        draw_rectangle(bounding_box, color, dst_surface, dst_position);
      }
    }
  }
  else {
    // Draw children nodes.
    for (const std::unique_ptr<Node>& child : children) {
      child->draw(dst_surface, dst_position);
    }
  }
}

/**
 * \brief Draws the border of a rectangle on a surface for debugging purposes.
 * \param rectangle The rectangle to draw.
 * \param line_color The color to use.
 * \param dst_surface The destination surface.
 * \param dst_position Where to draw on that surface.
 */
template<typename T>
void Quadtree<T>::Node::draw_rectangle(
    const Rectangle& rectangle,
    const Color& line_color,
    const SurfacePtr& dst_surface,
    const Point& dst_position
) {
  // TODO remove this function when the draw line API is available

  Rectangle where = rectangle;
  where.set_xy(where.get_xy() + dst_position);
  dst_surface->fill_with_color(line_color, Rectangle(
      where.get_top_left(), Size(where.get_width(), 1)
  ));
  dst_surface->fill_with_color(line_color, Rectangle(
      where.get_bottom_left() + Point(0, -1), Size(where.get_width(), 1)
  ));
  dst_surface->fill_with_color(line_color, Rectangle(
      where.get_top_left(), Size(1, where.get_height())
  ));
  dst_surface->fill_with_color(line_color, Rectangle(
      where.get_top_right() + Point(-1, 0), Size(1, where.get_height())
  ));
}

}  // namespace Solarus
