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
#include "movements/PathFinding.h"
#include "entities/MapEntity.h"
#include "Map.h"

const SDL_Rect PathFinding::neighbours_locations[] = {
  {  8,  0, 16, 16 },
  {  8, -8, 16, 16 },
  {  0, -8, 16, 16 },
  { -8, -8, 16, 16 },
  { -8,  0, 16, 16 },
  { -8,  8, 16, 16 },
  {  0,  8, 16, 16 },
  {  8,  8, 16, 16 }
};

const SDL_Rect PathFinding::transition_collision_boxes[] = {
  { 16,  0,  8, 16 },
  {  0, -8, 24, 24 },
  {  0, -8, 16,  8 },
  { -8, -8, 24, 24 },
  { -8,  0,  8, 16 },
  { -8,  0, 24, 24 },
  {  0, 16, 16,  8 },
  {  0,  0, 24, 24 }
};

/**
 * Constructor.
 * @param map the map
 * @param source the entity that will move from the starting point to the target
 * (its position must be a 16*16 rectangle aligned on the map grid)
 * @param target the target entity (its size must be 16*16)
 */
PathFinding::PathFinding(Map *map, MapEntity *source_entity, MapEntity *target_entity):
  map(map), source_entity(source_entity), target_entity(target_entity) {

  SDL_Rect source = source_entity->get_position_in_map();
  SDL_Rect target = target_entity->get_position_in_map();

  if (source.x % 8 != 0 || source.y % 8 != 0 ||
      source.w % 16 != 0 || source.h % 16 != 0 ||
      target.w % 16 != 0 || target.h % 16 != 0) {
    DIE("The source and the target must be 16*16 rectangles and the source must be aligned on the map grid");
  }
}

/**
 * Destructor.
 */
PathFinding::~PathFinding(void) {

}

/**
 * Tries to find a path between the source point and the target point.
 * @return the path found, or an empty string if no path was found
 * (because there is no path or the target is too far)
 */
std::string PathFinding::compute_path(void) {

  // TODO faire un array de nodes correspondant à la grille 8*8 de la map

  // std::cout << "will compute a path from " << source.x << "," << source.y << " to " << target.x << "," << target.y << std::endl;

  SDL_Rect source = source_entity->get_position_in_map();
  SDL_Rect target = target_entity->get_position_in_map();

  target.x += 4;
  target.x -= target.x % 8;
  target.y += 4;
  target.y -= target.y % 8;

  if (target.x % 8 != 0 || target.y % 8 != 0) {
    DIE("Could not snap the target to the map grid");
  }

  int total_mdistance = get_manhattan_distance(source, target);
  if (total_mdistance > 15) {
    return ""; // too far to compute a path
  }

  std::string path = "";
  std::list<Node> closed_list;
  std::list<Node> open_list;

  Node starting_node;
  starting_node.location = source;
  starting_node.previous_cost = 0;
  starting_node.heuristic = total_mdistance;
  starting_node.total_cost = total_mdistance;
  starting_node.direction = ' ';
  open_list.push_front(starting_node);

  bool finished = false;
  while (!finished) {

    // pick the node with the lowest total cost in the open list
    Node current_node = open_list.front();
    open_list.pop_front();
    closed_list.push_back(current_node);
    // std::cout << SDL_GetTicks() << " picking the lowest cost node in the open list (" << (open_list.size() + 1) << " elements): "
    //  << current_node.location.x << "," << current_node.location.y << ", cost = " << current_node.previous_cost << " + " << current_node.heuristic << "\n";
    if (current_node.location.x == target.x && current_node.location.y == target.y) {
      // std::cout << "target node was added to the closed list\n";
      finished = true;
      path = rebuild_path(closed_list, current_node);
      // std::cout << "rebuild done\n";
    }
    else {
      // look at the accessible nodes from it
      // std::cout << SDL_GetTicks() << " looking for accessible states\n";
      for (int i = 0; i < 8; i++) {

	Node new_node;
	int immediate_cost = (i % 2 == 0) ? 10 : 14;
	new_node.previous_cost = current_node.previous_cost + immediate_cost;
	new_node.location = current_node.location;
	new_node.location.x += neighbours_locations[i].x;
	new_node.location.y += neighbours_locations[i].y;

	Node *existing_node = find_node(closed_list, new_node.location);
	if (existing_node == NULL && is_node_transition_valid(current_node, i)) {
	  // std::cout << "  node in direction " << i << " is not in the closed list\n";
	  // not in the closed list: look in the open list
	  existing_node = find_node(open_list, new_node.location);
	  if (existing_node == NULL) {
	    // not in the open list: add it
	    new_node.heuristic = get_manhattan_distance(new_node.location, target);
	    new_node.total_cost = new_node.previous_cost + new_node.heuristic;
	    new_node.parent_location = current_node.location;
	    new_node.direction = '0' + i;
	    add_sorted(open_list, new_node);
	    // std::cout << "  node in direction " << i << " is not in the open list, adding it with cost " << new_node.previous_cost << " + " << new_node.heuristic << "\n";
	  }
	  else {
	    // std::cout << "  node in direction " << i << " is already in the open list\n";
	    // already in the open list: see if the current path is better
	    if (new_node.previous_cost < existing_node->previous_cost) {
	      existing_node->previous_cost = new_node.previous_cost;
	      existing_node->total_cost = existing_node->previous_cost + existing_node->heuristic;
	      existing_node->parent_location = current_node.location;
	      open_list.sort();
	    }
	  }
	}
      }
      if (open_list.empty()) {
	finished = true;
      }
    }
  }

  // std::cout << "path found: " << path << std::endl;
  return path;
}

/**
 * Returns the Manhattan distance of two points, measured in number of 8*8 squares.
 * @param point1 a first point
 * @param point2 a second point
 * @return the Manhattan distance between these points
 */
int PathFinding::get_manhattan_distance(const SDL_Rect &point1, const SDL_Rect &point2) {

  int distance = abs(point2.x - point1.x) + abs(point2.y - point1.y);
  return distance / 8;
}

/**
 * Tries to find in a list a node with the specified location.
 * @param list the list to seek
 * @param location location of the node
 * @return the node if it was found, NULL otherwise
 */
PathFinding::Node * PathFinding::find_node(std::list<Node> &nodes, const SDL_Rect &location) {

  std::list<Node>::iterator i;
  std::list<Node>::iterator end = nodes.end();
  for (i = nodes.begin(); i != end; i++) {
    Node *node = &(*i);
    if (node->location.x == location.x && node->location.y == location.y) {
      return node;
    }
  }

  return NULL;
}

/**
 * Compares two nodes according to their total estimated cost.
 */
bool PathFinding::Node::operator<(const Node &other) {
  return total_cost < other.total_cost;
}

/**
 * Adds a node to the open list, making sure the list remains sorted.
 * @param open_list the open list of the A* algorithm
 * @param node the node to add
 */
void PathFinding::add_sorted(std::list<Node> &open_list, const Node &node) {

  bool inserted = false;
  std::list<Node>::iterator i;
  for (i = open_list.begin(); i != open_list.end() && !inserted; i++) {
    const Node &current_node = *i;
    if (current_node.total_cost >= node.total_cost) {
      open_list.insert(i, node);
      inserted = true;
    }
  }

  if (!inserted) {
    open_list.push_back(node);
  }
}

/**
 * Builds the string representation of the path found by the algorithm.
 * @param closed_list the closed list of A*
 * @return final_node the final node of the path
 */
std::string PathFinding::rebuild_path(std::list<Node> &closed_list, const Node &final_node) {

  const Node *current_node = &final_node;
  std::string path = "";
  while (current_node->direction != ' ') {
    path = current_node->direction + path;
    current_node = find_node(closed_list, current_node->parent_location);
    // std::cout << "path = " << path << std::endl;
  }
  return path;
}

/**
 * Returns whether a transition between two nodes is valid, i.e. whether there is no collision with the map.
 * @param initial_node the first node
 * @param direction the direction to take (0 to 7)
 * @return true if there is no collision for this transition
 */
bool PathFinding::is_node_transition_valid(const Node &initial_node, int direction) {

  SDL_Rect collision_box = transition_collision_boxes[direction];
  collision_box.x += initial_node.location.x;
  collision_box.y += initial_node.location.y;

  return !map->collision_with_obstacles(source_entity->get_layer(), collision_box, source_entity);
}

