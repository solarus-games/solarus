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
#ifndef SOLARUS_GRID_H
#define SOLARUS_GRID_H

#include "solarus/core/Common.h"
#include "solarus/core/Debug.h"
#include "solarus/core/Rectangle.h"
#include "solarus/core/Size.h"
#include <set>
#include <vector>

namespace Solarus {

/**
 * \brief A collection of objects spatially located in a grid.
 *
 * In the current implementation, objects cannot move or be resized.
 */
template <typename T>
class Grid {

  public:

    Grid(const Size& grid_size, const Size& cell_size);

    const Size& get_grid_size() const;
    const Size& get_cell_size() const;
    size_t get_num_rows() const;
    size_t get_num_columns() const;
    size_t get_num_cells() const;

    void clear();
    void add(const T& element, const Rectangle& bounding_box);

    const std::vector<T>& get_elements(size_t cell_index) const;
    void get_elements(const Rectangle& where,
        std::vector<T>& elements) const;

  private:

    const Size grid_size;
    const Size cell_size;
    size_t num_rows;
    size_t num_columns;
    std::vector<std::vector<T>> elements;     /**< Two-dimensional array of cells. */

};

/**
 * \brief Constructor.
 * \param grid_size Size of the grid to create.
 * \param cell_size Size of a cell of the grid. It is allowed that the size of
 * the grid is not a multiple of the cell size.
 */
template <typename T>
Grid<T>::Grid(const Size& grid_size, const Size& cell_size):
    grid_size(grid_size),
    cell_size(cell_size),
    num_rows(0),
    num_columns(0) {

  Debug::check_assertion(grid_size.width > 0 && grid_size.height > 0,
      "Invalid grid size");
  Debug::check_assertion(cell_size.width > 0 && cell_size.height > 0,
      "Invalid cell size");

  num_rows = grid_size.height / cell_size.height;
  if (grid_size.height % cell_size.height != 0) {
    ++num_rows;
  }

  num_columns = grid_size.width / cell_size.width;
  if (grid_size.width % cell_size.width != 0) {
    ++num_columns;
  }
  elements.resize(num_rows * num_columns);
}

/**
 * \brief Returns the size of the grid.
 * \return The size of the grid.
 */
template <typename T>
const Size& Grid<T>::get_grid_size() const {
  return grid_size;
}

/**
 * \brief Returns the size of a cell of the grid.
 * \return The size of a cell.
 */
template <typename T>
const Size& Grid<T>::get_cell_size() const {
  return cell_size;
}

/**
 * \brief Returns the number of rows in the grid.
 * \return The number of rows.
 */
template <typename T>
size_t Grid<T>::get_num_rows() const {
  return num_rows;
}

/**
 * \brief Returns the number of columns in the grid.
 * \return The number of columns.
 */
template <typename T>
size_t Grid<T>::get_num_columns() const {
  return num_columns;
}

/**
 * \brief Returns the total number of cells in the grid.
 * \return The number of cells.
 */
template <typename T>
size_t Grid<T>::get_num_cells() const {
  return elements.size();
}

/**
 * \brief Returns all elements in the specified cell.
 * \param cell_index Index of a cell in the grid.
 * \return The elements in this cell.
 */
template <typename T>
const std::vector<T>& Grid<T>::get_elements(size_t cell_index) const {

  Debug::check_assertion(cell_index < get_num_cells(),
      "Invalid index");

  return elements[cell_index];
}

/**
 * \brief Returns all elements in the specified rectangle.
 *
 * Elements that are in several cells are returned only once.
 *
 * \param where The area to get.
 * \param[out] elements The vector to fill.
 */
template <typename T>
void Grid<T>::get_elements(
    const Rectangle& where,
    std::vector<T>& elements) const {

  const size_t row1 = where.get_y() / num_rows;
  const size_t row2 = where.get_y() + where.get_height() / num_rows;
  const size_t column1 = where.get_x() / num_columns;
  const size_t column2 = where.get_x() + where.get_width() / num_columns;

  if (row1 > row2 || column1 > column2) {
    // No cell.
    return;
  }

  std::set<T> elements_added;
  for (size_t i = row1; i <= row2; ++i) {
    if (i >= num_rows) {
      continue;
    }

    for (size_t j = column1; j <= column2; ++j) {
      if (j >= num_columns) {
        continue;
      }

      const std::vector<T>& in_cell = this->elements[i * num_columns + j];
      for (const T& element: in_cell) {
        if (elements_added.find(element) == elements_added.end()) {
          elements_added.insert(element);
          elements.push_back(element);
        }
      }
    }
  }
}

/**
 * \brief Removes all elements in the grid.
 */
template <typename T>
void Grid<T>::clear() {

  elements.clear();
  elements.resize(num_rows * num_columns);
}

/**
 * \brief Adds an element in the grid.
 *
 * The element will be added to all cells it overlaps.
 *
 * \param element The element to add.
 * \param bounding_box Bounding box of the element.
 */
template <typename T>
void Grid<T>::add(const T& element, const Rectangle& bounding_box) {

  const int row1 = bounding_box.get_y() / cell_size.height;
  const int row2 = (bounding_box.get_y() + bounding_box.get_height()) / cell_size.height;
  const int column1 = bounding_box.get_x() / cell_size.width;
  const int column2 = (bounding_box.get_x() + bounding_box.get_width()) / cell_size.width;

  if (row1 > row2 || column1 > column2) {
    // No cell.
    return;
  }

  for (int i = row1; i <= row2; ++i) {
    if (i < 0 || i >= (int) num_rows) {
      continue;
    }

    for (int j = column1; j <= column2; ++j) {
      if (j < 0 || j >= (int) num_columns) {
        continue;
      }

      elements[i * num_columns + j].push_back(element);
    }
  }
}

}

#endif

