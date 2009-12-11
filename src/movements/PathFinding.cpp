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

const Rectangle PathFinding::neighbours_locations[] = {
  Rectangle( 8,  0, 16, 16 ),
  Rectangle( 8, -8, 16, 16 ),
  Rectangle( 0, -8, 16, 16 ),
  Rectangle(-8, -8, 16, 16 ),
  Rectangle(-8,  0, 16, 16 ),
  Rectangle(-8,  8, 16, 16 ),
  Rectangle( 0,  8, 16, 16 ),
  Rectangle( 8,  8, 16, 16 )
};

const Rectangle PathFinding::transition_collision_boxes[] = {
  Rectangle(16,  0,  8, 16 ),
  Rectangle( 0, -8, 24, 24 ),
  Rectangle( 0, -8, 16,  8 ),
  Rectangle(-8, -8, 24, 24 ),
  Rectangle(-8,  0,  8, 16 ),
  Rectangle(-8,  0, 24, 24 ),
  Rectangle( 0, 16, 16,  8 ),
  Rectangle( 0,  0, 24, 24 )
};

/**
 * Constructor.
 * @param map the map
 * @param source_entity the entity that will move from the starting point to the target
 * (its position must be a 16*16 rectangle aligned on the map grid)
 * @param target_entity the target entity (its size must be 16*16)
 */
PathFinding::PathFinding(Map *map, MapEntity *source_entity, MapEntity *target_entity):
  map(map), source_entity(source_entity), target_entity(target_entity) {

  const Rectangle &source = source_entity->get_bounding_box();
  const Rectangle &target = target_entity->get_bounding_box();

  if (!source_entity->is_aligned_to_grid() ||
      source.get_width() % 16 != 0 || source.get_height() % 16 != 0 ||
      target.get_width() % 16 != 0 || target.get_height() % 16 != 0) {
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

//  std::cout << "will compute a path from " << source_entity->get_top_left_x() << "," << source_entity->get_top_left_y()
//    << " to " << target_entity->get_top_left_x() << "," << target_entity->get_top_left_y() << std::endl;

  Rectangle source = source_entity->get_bounding_box();
  Rectangle target = target_entity->get_bounding_box();

  target.add_x(4);
  target.add_x(-target.get_x() % 8);
  target.add_y(4);
  target.add_y(-target.get_y() % 8);
  int target_index = get_square_index(target);

  if (target.get_x() % 8 != 0 || target.get_y() % 8 != 0) {
    DIE("Could not snap the target to the map grid");
  }

  int total_mdistance = get_manhattan_distance(source, target) * 10;
  if (total_mdistance > 250 || target_entity->get_layer() != source_entity->get_layer()) {
//    std::cout << "too far, not computing a path\n";
    return ""; // too far to compute a path
  }

  std::string path = "";

  Node starting_node;
  int index = get_square_index(source);
  starting_node.location = source;
  starting_node.index = index;
  starting_node.previous_cost = 0;
  starting_node.heuristic = total_mdistance;
  starting_node.total_cost = total_mdistance;
  starting_node.direction = ' ';
  starting_node.parent_index = -1;

  open_list[index] = starting_node;
  open_list_indices.push_front(index);

  bool finished = false;
  while (!finished) {

    // pick the node with the lowest total cost in the open list
    int index = open_list_indices.front();
    Node *current_node = &open_list[index];
    open_list_indices.pop_front();
    closed_list[index] = *current_node;
    open_list.erase(index);
    current_node = &closed_list[index];

    // std::cout << System::now() << " picking the lowest cost node in the open list (" << (open_list_indices.size() + 1) << " elements): "
    //  << current_node->location.x << "," << current_node->location.y << ", cost = " << current_node->previous_cost << " + " << current_node->heuristic << "\n";

    if (index == target_index) {
      // std::cout << "target node was added to the closed list\n";
      finished = true;
      path = rebuild_path(current_node);
      // std::cout << "rebuild done\n";
    }
    else {
      // look at the accessible nodes from it
      // std::cout << System::now() << " looking for accessible states\n";
      for (int i = 0; i < 8; i++) {

	Node new_node;
	int immediate_cost = (i % 2 == 0) ? 10 : 14;
	new_node.previous_cost = current_node->previous_cost + immediate_cost;
	new_node.location = current_node->location;
	new_node.location.add_xy(neighbours_locations[i].get_x(), neighbours_locations[i].get_y());
	new_node.index = get_square_index(new_node.location);
	// std::cout << "  node in direction " << i << ": index = " << new_node.index << std::endl;

	bool in_closed_list = (closed_list.find(new_node.index) != closed_list.end());
	if (!in_closed_list && get_manhattan_distance(new_node.location, target) < 25 && is_node_transition_valid(*current_node, i)) {
	  // std::cout << "  node in direction " << i << " is not in the closed list\n";
	  // not in the closed list: look in the open list
	  
	  bool in_open_list = open_list.find(new_node.index) != open_list.end();

	  if (!in_open_list) {
	    // not in the open list: add it
	    new_node.heuristic = get_manhattan_distance(new_node.location, target) * 10;
	    new_node.total_cost = new_node.previous_cost + new_node.heuristic;
	    new_node.parent_index = index;
	    new_node.direction = '0' + i;
            open_list[new_node.index] = new_node; 
	    add_index_sorted(&open_list[new_node.index]);
	    // std::cout << "  node in direction " << i << " is not in the open list, adding it with cost " << new_node.previous_cost << " (" 
	    //  << current_node->previous_cost << " + " << immediate_cost << ") + " << new_node.heuristic << " and parent " << new_node.parent_index << "\n";
	  }
	  else {
	    // std::cout << "  node in direction " << i << " is already in the open list\n";
	    Node *existing_node = &open_list[new_node.index];
	    // already in the open list: see if the current path is better
	    if (new_node.previous_cost < existing_node->previous_cost) {
	      existing_node->previous_cost = new_node.previous_cost;
	      existing_node->total_cost = existing_node->previous_cost + existing_node->heuristic;
	      existing_node->parent_index = index;
	      open_list_indices.sort();
	    }
	  }
	}
      }
      if (open_list_indices.empty()) {
	finished = true;
      }
    }
  }

//  std::cout << "path found: " << path << ", open nodes: " << open_list.size() << ", closed nodes: " << closed_list.size() << std::endl;
  return path;
}

/**
 * Returns the index of the 8*8 square in the map
 * corresponding to the specified location.
 * @param location location of a node on the map
 * @return index of the square corresponding to the top-left part of the location
 */
int PathFinding::get_square_index(const Rectangle &location) {

  int x8 = location.get_x() / 8;
  int y8 = location.get_y() / 8;
  return y8 * map->get_width8() + x8;
}

/**
 * Compares two nodes according to their total estimated cost.
 * @param other the other node
 */
bool PathFinding::Node::operator<(const Node &other) {
  return total_cost < other.total_cost;
}

/**
 * Adds the index of a node to the sorted list of indices of the open list, making sure the list remains sorted.
 * @param node the node
 */
void PathFinding::add_index_sorted(Node *node) {

  bool inserted = false;
  std::list<int>::iterator i;
  for (i = open_list_indices.begin(); i != open_list_indices.end() && !inserted; i++) {
    int index = *i;
    Node *current_node = &open_list[index];
    if (current_node->total_cost >= node->total_cost) {
      open_list_indices.insert(i, node->index);
      inserted = true;
    }
  }

  if (!inserted) {
    open_list_indices.push_back(node->index);
  }
}

/**
 * Builds the string representation of the path found by the algorithm.
 * @param closed_list the closed list of A*
 * @return final_node the final node of the path
 */
std::string PathFinding::rebuild_path(const Node *final_node) {

  const Node *current_node = final_node;
  std::string path = "";
  while (current_node->direction != ' ') {
    path = current_node->direction + path;
    current_node = &closed_list[current_node->parent_index];
    // std::cout << "current_node: " << current_node->index << ", path = " << path << std::endl;
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

  Rectangle collision_box = transition_collision_boxes[direction];
  collision_box.set_x(collision_box.get_x() + initial_node.location.get_x());
  collision_box.set_y(collision_box.get_y() + initial_node.location.get_y());

  return !map->test_collision_with_obstacles(source_entity->get_layer(), collision_box, source_entity);
}

