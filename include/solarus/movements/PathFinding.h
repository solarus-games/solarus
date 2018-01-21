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
#ifndef SOLARUS_PATH_FINDING_H
#define SOLARUS_PATH_FINDING_H

#include "solarus/core/Common.h"
#include "solarus/core/Point.h"
#include <list>
#include <map>
#include <string>

namespace Solarus {

class Map;
class Entity;
class Rectangle;

/**
 * \brief Implementation of the A* algorithm to compute a path.
 *
 * In the current implementation, the computed path always corresponds to a
 * shape of 16*16. If the entity to move is bigger, some obstacles may prevent
 * it from following the computed path.
 */
class SOLARUS_API PathFinding {

  public:

    PathFinding(
        Map& map,
        Entity& source_entity,
        Entity& target_entity);

    std::string compute_path();
    std::string compute_path(const Point& offset);

  private:

    /**
     * \brief Represents a node in the path to compute.
     *
     * A node is the location of a 16*16 square of the map.
     * The algorithm tries to find the best sequence of nodes leading to the target.
     */
    class Node {

     public:

      Point location;     /**< location of this node on the map */
      int index;          /**< index of this node's square on the map (depends on the location) */

      // total_cost = previous_cost + heuristic
      int previous_cost;  /**< cost of the best path that leads to this node */
      int heuristic;      /**< estimation of the remaining cost to the target */
      int total_cost;     /**< total cost of this node */

      int parent_index;   /**< index of the square containing the best node leading to this node */
      char direction;     /**< direction from the parent node to this node (0 to 7) */

      bool operator<(const Node& other) const;
    };

    int get_square_index(const Point& location) const;
    bool is_node_transition_valid(const Node& node, int direction) const;
    void add_index_sorted(Node* node);
    std::string rebuild_path(const Node* final_node);

    static const Point neighbours_locations[];
    static const Rectangle transition_collision_boxes[];

    Map& map;                          /**< the map */
    Entity& source_entity;             /**< the entity to move */
    Entity& target_entity;             /**< the target point */

    std::map<int, Node> closed_list;   /**< the closed list, indexed by the node locations on the map */
    std::map<int, Node> open_list;     /**< the open list, indexed by the node locations on the map */
    std::list<int> open_list_indices;  /**< indices of the open list elements, sorted by priority */

};

}

#endif

