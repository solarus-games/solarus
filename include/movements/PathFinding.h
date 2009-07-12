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
#include <list>

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

      SDL_Rect location; /**< location of the 16*16 square on the map */

      // total_cost = previous_cost + heuristic
      int previous_cost; /**< cost of the best path that leads to this node */
      int heuristic;     /**< estimation of the remaining cost to the target */
      int total_cost;    /**< total cost of this node */

      SDL_Rect parent_location;      /**< location of the best node leading to this node */
      char direction;                /**< direction from the parent node to this node (0 to 7) */

      bool operator<(const Node &other);
    };

    static const SDL_Rect neighbours_locations[];
    static const SDL_Rect transition_collision_boxes[];

    Map *map;                     /**< the map */
    MapEntity *source_entity;     /**< the entity to move */
    MapEntity *target_entity;     /**< the target point */

    int get_manhattan_distance(const SDL_Rect &point1, const SDL_Rect &point2);
    bool is_node_transition_valid(const Node &node, int direction);
    Node * find_node(std::list<Node> &nodes, const SDL_Rect &location);
    void add_sorted(std::list<Node> &open_list, const Node &node);
    std::string rebuild_path(std::list<Node> &closed_list, const Node &final_node);

  public:

    PathFinding(Map *map, MapEntity *source_entity, MapEntity *target_entity);
    ~PathFinding(void);

    std::string compute_path(void);
};

#endif

