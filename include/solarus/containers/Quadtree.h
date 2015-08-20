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
#ifndef SOLARUS_QUADTREE_H
#define SOLARUS_QUADTREE_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lowlevel/Size.h"
#include <array>
#include <vector>

namespace Solarus {

/**
 * \brief A collection of objects spatially located in an adaptable 2D grid.
 *
 * The main goal of this container is to get objects in a given rectangle as
 * quickly as possible.
 */
template <typename T>
class Quadtree {

  public:

    Quadtree(const Size& space_size);

    const Size& get_space_size() const;

    void add(const T& element);
    void remove(const T& element);
    void move(const T& element);

    void get_elements(
        const Rectangle& where,
        std::vector<T>& elements
    ) const;

    static constexpr int
        max_in_cell = 8;     /**< A cell is split if it exceeds this number
                              * when adding an element. */
    static constexpr int
        min_in_4_cells = 4;  /**< 4 sibling cells are merged if their total
                              * is below this number when removing an element. */

  private:

    struct Node {

      T elements[max_in_cell];
      Node* children[4];
      Node* parent;
      Point center;
      int num_elements;

      Node() :
        elements(),
        children({ nullptr }),
        parent(nullptr),
        num_elements(0) {

      }
    };

    void get_elements(
        const Node* node,
        const Rectangle& where,
        std::vector<T>& elements
    ) const;
    int get_num_elements(
        const Node* node
    );

    const Size space_size;
    Node root;

};

}

#include "solarus/containers/Quadtree.inl"

#endif

