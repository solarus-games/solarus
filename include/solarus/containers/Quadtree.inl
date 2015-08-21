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

namespace Solarus {

/**
 * \brief Creates a quadtree.
 * \param size Size of the space to create partitions of.
 */
template<typename T>
Quadtree<T>::Quadtree(const Size& space_size) :
    space_size(space_size),
    root() {

}

/**
 * \brief Returns the size of the space partitioned by this quadtree.
 * \return The space size.
 */
template<typename T>
const Size& Quadtree<T>::get_space_size() const {
    return space_size;
}

/**
 * \brief Adds an element to the quadtree.
 * \param element The element to add.
 */
template<typename T>
void Quadtree<T>::add(const T& element) {

  // TODO
}

/**
 * \brief Removes an element from the quadtree.
 * \param element The element to remove.
 */
template<typename T>
void Quadtree<T>::remove(const T& element) {

  // TODO
}

/**
 * \brief Moves the element in the quadtree.
 *
 * This function should be called when the position or size or the element
 * is changed.
 */
template<typename T>
void Quadtree<T>::move(const T& element) {

  // TODO store a map of bounding boxes to know the previous position
}

/**
 * \brief Returns the total number of elements in the quadtree.
 * \return The number of elements.
 */
template<typename T>
int Quadtree<T>::get_num_elements() const {
  return get_num_elements(Rectangle(Point(0, 0), get_space_size()));
}

/**
 * \brief Returns the number of elements intersecting the given rectangle.
 * \param where The rectangle to check.
 * \return The number of elements whose bounding box intersect the rectangle.
 */
template<typename T>
int Quadtree<T>::get_num_elements(
    const Rectangle& where
) const {
  return get_num_elements(&root, where);
}

/**
 * \brief Returns the number of elements intersecting the given rectangle
 * under a given node.
 * \param node The node to inspect or nullptr.
 * \param where The rectangle to check.
 * \return The number of elements under this node.
 */
template<typename T>
int Quadtree<T>::get_num_elements(
    const Node* /* node */,
    const Rectangle& /* where */
) const {

  // TODO avoid duplicates
  return 0;
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
  get_elements(&root, where, elements);
}

/**
 * \brief Gets the elements intersecting the given rectangle under a node.
 * \param[in] node The node to inspect or nullptr.
 * \param[in] where The rectangle to check.
 * \param[in/out] elements A list that will be filled with elements.
 */
template<typename T>
void Quadtree<T>::get_elements(
    const Node* /* node */,
    const Rectangle& /* where */,
    std::vector<T>& /* elements */
) const {

  // TODO avoid duplicates
}

}  // namespace Solarus
