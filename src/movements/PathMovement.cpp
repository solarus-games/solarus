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
#include "movements/PathMovement.h"
#include "entities/MapEntity.h"
#include "Geometry.h"
#include "Random.h"

/**
 * Creates a path movement object.
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7')
 * @param speed speed of the movement
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param with_collisions true to make the movement sensitive to obstacles
 */
PathMovement::PathMovement(const std::string &path, int speed, bool loop, bool with_collisions):
  initial_path(path), remaining_path(path), initial_speed(speed), current_direction(0), distance_covered(0),
  loop(loop), with_collisions(with_collisions), finished(false), snapping(false) {

  set_speed(speed);
  start_next_move();
}

/**
 * Destructor.
 */
PathMovement::~PathMovement(void) {

}

/**
 * Sets the position of the entity.
 * This is a redefinition of CollisionMovement::set_position() because we need
 * to update the number of pixels covered.
 * @param x the new x position
 * @param y the new y position
 */
void PathMovement::set_position(int x, int y) {

  if (x != get_x()) {
    distance_covered++;
  }
  if (y != get_y()) {
    distance_covered++;
  }
  CollisionMovement::set_position(x, y);
}

/**
 * Updates the movements: detects the collisions
 * in order to restart the movement.
 */
void PathMovement::update(void) {

  if (!suspended && is_current_move_finished()) {
    start_next_move();
  }

  CollisionMovement::update();
}

/**
 * Returns whether the movement is finished.
 * @return true if the end of the path was reached or the entity 
 * reached an obstacle
 */
bool PathMovement::is_finished(void) {
  return finished || is_stopped();
}

/**
 * Returns whether the current move of the path is finished.
 * @return true if the current move is finished
 */
bool PathMovement::is_current_move_finished(void) {

  if (remaining_path.size() == 0) {
    return true;
  }

  int distance_to_cover = (current_direction % 2 == 0) ? 8 : 16;

  return distance_covered >= distance_to_cover;
}

/**
 * Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 * If the collisions are not enabled for this movement, false is always returned.
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map obstacles in this position
 */
bool PathMovement::collision_with_map(int dx, int dy) {
  return with_collisions && CollisionMovement::collision_with_map(dx, dy);
}

/**
 * Starts the next step of the movement.
 */
void PathMovement::start_next_move(void) {

  if (entity == NULL) {
    return;
  }

  if (!entity->is_aligned_to_grid()) {
    // snap the entity to the grid before making the next move

    int x = entity->get_top_left_x();
    int y = entity->get_top_left_y();
    int snapped_x = x + 4;
    int snapped_y = y + 4;
    snapped_x -= snapped_x % 8;
    snapped_y -= snapped_y % 8;

    if (Geometry::get_distance(x, y, snapped_x, snapped_y) < 2) {
      set_position(entity->get_x() + (snapped_x - x), entity->get_y() + (snapped_y - y));
    }
    else if (!snapping) {
      set_direction(Geometry::get_angle(entity->get_top_left_x(), entity->get_top_left_y(), snapped_x, snapped_y));
      snapping = true;
    }
  }
  
  if (entity->is_aligned_to_grid()) {
    if (remaining_path.size() == 0) {
      if (loop) {
	remaining_path = initial_path;
      }
      else {
	finished = true;
	stop();
      }
    }

    if (!finished) {
      current_direction = remaining_path[0] - '0';
      set_direction(current_direction * 45);
      distance_covered = 0;
      snapping = false;
      remaining_path = remaining_path.substr(1);
    }
  }
}

/**
 * Returns the direction of the current move, between 0 and 7.
 * @return the direction of the current move
 */
int PathMovement::get_current_direction(void) {
  return current_direction;
}

/**
 * Returns a string describing a random length path in one of the four main directions.
 * @return a random direction
 */
const std::string PathMovement::get_random_path() {

  char c = '0' + (Random::get_number(4) * 2);
  int length = Random::get_number(5) + 3;
  std::string path = "";
  for (int i = 0; i < length; i++) {
    path += c;
  }

  return path;
}

