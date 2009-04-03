#include "movements/PathMovement.h"

/**
 * Creates a path movement object.
 * @param map the map
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7')
 * @param speed speed of the movement
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param with_collisions true to make the movement sensitive to obstacles
 */
PathMovement::PathMovement(Map *map, std::string path, int speed, bool loop, bool with_collisions):
  MovementWithCollision(map), initial_path(path), remaining_path(path),
  initial_speed(speed), loop(loop), with_collisions(with_collisions), finished(false) {

  set_speed(speed);
  start_next_move();
}

/**
 * Destructor.
 */
PathMovement::~PathMovement(void) {

}

/**
 * Sets the x position of the entity.
 * This is a redefinition of MovementWithCollision::set_x() because we need
 * the number of pixels covered.
 * @param x the new x position
 */
void PathMovement::set_x(int x) {

  if (x != get_x()) {
    distance_covered_x++;
    if (distance_covered_x == 8) {
      start_next_move();
    }
  }
  MovementWithCollision::set_x(x);
}

/**
 * Sets the y position of the entity.
 * This is a redefinition of MovementWithCollision::set_y() because we need
 * the number of pixels covered.
 * @param y the new y position
 */
void PathMovement::set_y(int y) {

  if (y != get_y()) {
    distance_covered_y++;
    if (distance_covered_y == 8) {
      start_next_move();
    }
  }
  MovementWithCollision::set_y(y);
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
 * Returns whether the entity would collide with the map
 * if it was moved a few pixels from its position.
 * If the collisions are not enabled for this movement, false is always returned.
 * @param dx x distance between the current position and the position to check
 * @param dy y distance between the current position and the position to check
 * @return true if the entity would overlap the map obstacles in this position
 */
bool PathMovement::collision_with_map(int dx, int dy) {
  return with_collisions && MovementWithCollision::collision_with_map(dx, dy);
}

/**
 * Starts the next step of the movement.
 */
void PathMovement::start_next_move(void) {

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
    distance_covered_x = 0;
    distance_covered_y = 0;
    remaining_path = remaining_path.substr(1);
  }
}

/**
 * Returns the direction of the current move, between 0 and 7.
 * @return the direction of the current move
 */
int PathMovement::get_current_direction(void) {
  return current_direction;
}
