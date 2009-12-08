/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_PATH_FINDING_H
#define ZSDX_PATH_FINDING_H

#include "Common.h"
#include "lowlevel/Rectangle.h"
#include <list>
#include <map>

/**
 * This class computes a path between two entities on the map with the A* algorithm.
 */
class PathFinding {

  private:

    /**
     * A node is a 16*16 square of the map.
     * The algorithm tries to find the best sequence of nodes leading to the target.
     */
    class Node {

     public:

      Rectangle location; /**< location of this node on the map */
      int index;          /**< index of this node's square on the map (depends on the location) */

      // total_cost = previous_cost + heuristic
      int previous_cost;  /**< cost of the best path that leads to this node */
      int heuristic;      /**< estimation of the remaining cost to the target */
      int total_cost;     /**< total cost of this node */

      int parent_index;   /**< index of the square containing the best node leading to this node */
      char direction;     /**< direction from the parent node to this node (0 to 7) */

      bool operator<(const Node &other);
    };

    static const Rectangle neighbours_locations[];
    static const Rectangle transition_collision_boxes[];

    Map *map;                       /**< the map */
    MapEntity *source_entity;       /**< the entity to move */
    MapEntity *target_entity;       /**< the target point */

    std::map<int,Node> closed_list;   /**< the closed list, indexed by the node locations on the map */
    std::map<int,Node> open_list;     /**< the open list, indexed by the node locations on the map */
    std::list<int> open_list_indices; /**< indices of the open list elements, sorted by priority */

    int get_square_index(const Rectangle &location);
    int get_manhattan_distance(const Rectangle &point1, const Rectangle &point2);
    bool is_node_transition_valid(const Node &node, int direction);
    void add_index_sorted(Node *node);
    std::string rebuild_path(const Node *final_node);

  public:

    PathFinding(Map *map, MapEntity *source_entity, MapEntity *target_entity);
    ~PathFinding(void);

    std::string compute_path(void);
};

#endif

