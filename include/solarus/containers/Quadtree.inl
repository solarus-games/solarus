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
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

/**
 * \brief Creates a quadtree.
 * \param size Size of the space to create partitions of.
 */
template<typename T>
Quadtree<T>::Quadtree(const Size& space_size) :
    root(Rectangle(Point(0, 0), space_size)) {

}

/**
 * \brief Returns the size of the space partitioned by this quadtree.
 * \return The space size.
 */
template<typename T>
Size Quadtree<T>::get_space_size() const {
    return root.get_cell_size();
}

/**
 * \brief Adds an element to the quadtree.
 * \param element The element to add.
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::add(const T& element) {

  if (elements.find(element) != elements.end()) {
    return false;
  }

  return root.add(element);
}

/**
 * \brief Removes an element from the quadtree.
 * \param element The element to remove.
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::remove(const T& /* element */) {

  // TODO
  return false;
}

/**
 * \brief Moves the element in the quadtree.
 *
 * This function should be called when the position or size or the element
 * is changed.
 *
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::move(const T& /* element */) {

  // TODO store a map of bounding boxes to know the previous position
  return false;
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
 * \param[in] where The rectangle to check.
 * \param[in/out] elements A list that will be filled with elements.
 */
template<typename T>
void Quadtree<T>::get_elements(
    const Rectangle& where,
    std::vector<T>& elements
) const {
  root.get_elements(where, elements);
}

/**
 * \brief Creates a node with the given coordinates.
 * \param cell Cell coordinates of the node.
 */
template<typename T>
Quadtree<T>::Node::Node(const Rectangle& cell) :
    elements(),
    children{ nullptr, nullptr, nullptr, nullptr },
    cell(cell),
    center(cell.get_center()),
    num_elements(0) {

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
 * \return \c true in case of success.
 */
template<typename T>
bool Quadtree<T>::Node::add(
    const T& element
) {
  const Rectangle& box = element->get_bounding_box();
  if (!get_cell().overlaps(box)) {
    return false;
  }

  if (!is_split()) {

    // See if it is time to split.
    if (get_cell().contains(box.get_center())) {
      // We are the main cell of this element: it counts in the total.
      if (get_num_elements() >= max_in_cell &&
          get_cell_size().width < min_cell_size &&
          get_cell_size().height < min_cell_size) {
        split();
      }
    }
  }

  if (!is_split()) {
    // Add it to the current node.
    elements.push_back(element);
    return true;
  }

  // Add it to children cells.
  children[0]->add(element);
  children[1]->add(element);
  children[2]->add(element);
  children[3]->add(element);
  return true;
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

  // Create children cells.
  const Rectangle& cell = get_cell();
  const Point& center = cell.get_center();
  children[0] = std::unique_ptr<Node>(
      new Node(Rectangle(cell.get_top_left(), center))
  );
  children[1] = std::unique_ptr<Node>(
      new Node(Rectangle(center.x, cell.get_top(), cell.get_right(), center.y))
  );
  children[2] = std::unique_ptr<Node>(
      new Node(Rectangle(cell.get_left(), center.y, center.x, cell.get_bottom()))
  );
  children[3] = std::unique_ptr<Node>(
      new Node(Rectangle(center, cell.get_bottom_right()))
  );

  // Move existing elements into them.
  for (const T& element: elements) {
    children[0]->add(element);
    children[1]->add(element);
    children[2]->add(element);
    children[3]->add(element);
  }
  elements.clear();
}

/**
 * \brief Returns the number of elements whose center is under this node.
 * \return The number of elements under this node.
 */
template<typename T>
int Quadtree<T>::Node::get_num_elements() const {

  // Some elements can overlap several cells.
  // To avoid duplicates, we count an element if its center is in this cell.
  // TODO This information could be stored for better performance.
  int num_elements = 0;
  for (const T& element: elements) {
    const Rectangle& box = element->get_bounding_box();
    if (get_cell().contains(box.get_center())) {
      ++num_elements;
    }
  }
  return num_elements;
}

/**
 * \brief Gets the elements intersecting the given rectangle under this node.
 * \param[in] where The rectangle to check.
 * \param[in/out] elements A list that will be filled with elements.
 */
template<typename T>
void Quadtree<T>::Node::get_elements(
    const Rectangle& /* where */,
    std::vector<T>& /* elements */
) const {

  // TODO avoid duplicates
}

}  // namespace Solarus
