/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "movements/PathMovement.h"
#include "entities/MapEntity.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/Random.h"
#include "lowlevel/System.h"

/**
 * Creates a path movement object.
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7')
 * @param speed speed of the movement
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param with_collisions true to make the movement sensitive to obstacles
 * @param must_be_aligned true to snap the entity to the map grid before moving it
 */
PathMovement::PathMovement(const std::string &path, int speed,
    bool loop, bool with_collisions, bool must_be_aligned):
  CollisionMovement(with_collisions),
  initial_path(path), remaining_path(path), normal_speed(speed), current_direction(0),
  distance_covered(0), total_distance_covered(0),
  loop(loop), finished(false), must_be_aligned(must_be_aligned), snapping(false) {

}

/**
 * Destructor.
 */
PathMovement::~PathMovement(void) {

}

/**
 * Sets the entity to be controlled by this movement object.
 * @param entity the entity to control
 */
void PathMovement::set_entity(MapEntity *entity) {
  CollisionMovement::set_entity(entity);
  start_next_move();
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
    total_distance_covered++;
  }
  if (y != get_y()) {
    distance_covered++;
    total_distance_covered++;
  }
  CollisionMovement::set_position(x, y);
}

/**
 * Changes the speed of the movement.
 * @param speed the new speed
 */
void PathMovement::set_speed(int speed) {
  this->normal_speed = speed; // memorize the speed to allow restarting later
  CollisionMovement::set_speed(speed);
}

/**
 * Updates the movements: detects the collisions
 * in order to restart the movement.
 */
void PathMovement::update(void) {

  if (!is_suspended() && is_current_move_finished()) {
    start_next_move();
  }

//  if (snapping) std::cout << "snapping\n";

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

  if (is_stopped()) {
    return true;
  }

  int distance_to_cover = (current_direction % 2 == 0) ? 8 : 16;

  return distance_covered >= distance_to_cover;
}

/**
 * Starts the next step of the movement.
 * This function is called when a step of the movement is finished,
 * or when the movement is restarted.
 * Before starting the step, if the must_be_aligned property is true,
 * the entity's top-left corner tries to get aligned with the 8*8 squares of the grid.
 */
void PathMovement::start_next_move(void) {

//  std::cout << "PathMovement::start_next_move()\n";

  // don't move while the entity is unknown
  if (entity == NULL) {
//    std::cout << "NULL\n";
    return;
  }

  // before starting the move, check that the entity is aligned with the 8*8 squares grid if necessary
  if (must_be_aligned && !entity->is_aligned_to_grid()) {
 
    // the entity has to be aligned but is not

    // calculate the coordinates of the closest grid intersection from the top-left corner of the entity
    int x = entity->get_top_left_x();
    int y = entity->get_top_left_y();
    int snapped_x = x + 4;
    int snapped_y = y + 4;
    snapped_x -= snapped_x % 8;
    snapped_y -= snapped_y % 8;

    if (!snapping) {
      // if we haven't started to move the entity into the direction of the closest grid intersection, we do it
      snapping_angle = Geometry::get_angle(entity->get_top_left_x(), entity->get_top_left_y(), snapped_x, snapped_y);
//      std::cout << "not aligned yet, going to direction " << snapping_angle << "\n";
      set_speed(normal_speed);
      set_direction(snapping_angle);
      snapping = true;
      stop_snapping_date = System::now() + 500;
    }
    else {
      // the entity is currently trying to move towards the closest grid intersection
      
      if (Geometry::get_distance(x, y, snapped_x, snapped_y) <= 2) {
	// if the entity has become close to the intersection, put it there directly,
	// unless an unlikely (but possible) collision occurs (see below)

//	std::cout << "almost aligned, distance = " << Geometry::get_distance(x, y, snapped_x, snapped_y) << "\n";

	x = entity->get_x() + (snapped_x - x);
	y = entity->get_y() + (snapped_y - y);

	if (!test_collision_with_map(x, y)) {
	  // no problem, we can align the entity right now
//	  std::cout << "aligning directly\n";
	  set_position(x, y);
	  stop();
	  snapping = false;
	}
      }

      uint32_t now = System::now();
      if (now >= stop_snapping_date) {
        // we could not snap the entity after the timeout, so we just go back to the opposite direction:
	// this is possible when there is collisions with 8*8 squares where only a part is an obstacle
	// (typically a statue that is being moved)
	snapping_angle += Geometry::PI;
//	std::cout << "could not align: going back to direction " << (snapping_angle * 360 / Geometry::TWO_PI) << "\n";
	set_speed(normal_speed);
	set_direction(snapping_angle);
	stop_snapping_date = now + 500;
      }
    }
  }

  // start the next step if we are ready
  if (!must_be_aligned || entity->is_aligned_to_grid()) {
//    std::cout << "aligned, considering next move (remaining_path = '" << remaining_path << "')\n";
    
    if (remaining_path.size() == 0) {
      // well, there is no next step (the remaining path is empty)
//      std::cout << "path empty:\n";
      if (loop) {
	// if the property 'loop' is true, repeat the same path again
//	std::cout << "  loop, doing path '" << remaining_path << "' again.\n";
	remaining_path = initial_path;
      }
      else if (!is_stopped()) {
	// the movement is finished: stop the entity
//	std::cout << "  path finished.\n";
	finished = true;
	stop();
      }
    }

    if (!finished && remaining_path.size() != 0) {
      // normal case: there is a next step to do

//      std::cout << "path: " << remaining_path << "\n";
      current_direction = remaining_path[0] - '0';
      set_speed(normal_speed);
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

/**
 * Returns the distance covered by this movement since its beginning.
 * @return the total distance covered in pixels
 */
int PathMovement::get_total_distance_covered(void) {
  return total_distance_covered;
}

