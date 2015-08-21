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
#include <map>
#include <vector>

namespace Solarus {

/**
 * \brief A collection of objects spatially located in an adaptable 2D grid.
 *
 * The main goal of this container is to get objects in a given rectangle as
 * quickly as possible.
 *
 * \param T Type of objects. There must be a get_bounding_box() method.
 */
template <typename T>
class Quadtree {

  public:

    Quadtree(const Size& space_size);

    Size get_space_size() const;

    bool add(const T& element);
    bool remove(const T& element);
    bool move(const T& element);

    void get_elements(
        const Rectangle& where,
        std::vector<T>& elements
    ) const;

    int get_num_elements() const;

    static constexpr int
        max_in_cell = 8;     /**< A cell is split if it exceeds this number
                              * when adding an element. */
    static constexpr int
        min_in_4_cells = 4;  /**< 4 sibling cells are merged if their total
                              * is below this number when removing an element. */
    static constexpr int
        min_cell_size = 16;  /**< Don't split more if a cell is smaller than
                              * this size. */

  private:

    class Node {

      public:

        Node(const Rectangle& cell);

        Rectangle get_cell() const;
        Size get_cell_size() const;

        int add(
            const T& element
        );

        void get_elements(
            const Rectangle& where,
            std::vector<T>& elements
        ) const;

        int get_num_elements() const;

      private:

        std::vector<T> elements;
        Node* children[4];
        Node* parent;
        Rectangle cell;
        Point center;
        int num_elements;

    };

    struct ElementInfo {
        Rectangle bounding_box;
        int num_nodes;
    };

    std::map<T, ElementInfo> elements;
    Node root;

};

template<typename T>
using QuadtreeNode = typename Quadtree<T>::Node;

}

#include "solarus/containers/Quadtree.inl"

#endif

