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
#include "lowlevel/System.h"
#include "lowlevel/Random.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

const std::string PathMovement::elementary_moves[] = {
    " 1  0   1  0   1  0   1  0   1  0   1  0   1  0   1  0", // 8 pixels right
    " 1 -1   1 -1   1 -1   1 -1   1 -1   1 -1   1 -1   1 -1", // 8 pixels right-up
    " 0 -1   0 -1   0 -1   0 -1   0 -1   0 -1   0 -1   0 -1", // 8 pixels up
    "-1 -1  -1 -1  -1 -1  -1 -1  -1 -1  -1 -1  -1 -1  -1 -1", // 8 pixels left-up
    "-1  0  -1  0  -1  0  -1  0  -1  0  -1  0  -1  0  -1  0", // 8 pixels left
    "-1  1  -1  1  -1  1  -1  1  -1  1  -1  1  -1  1  -1  1", // 8 pixels left-down
    " 0  1   0  1   0  1   0  1   0  1   0  1   0  1   0  1", // 8 pixels down
    " 1  1   1  1   1  1   1  1   1  1   1  1   1  1   1  1"  // 8 pixels right-down
};

/**
 * @brief Creates a path movement object.
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7')
 * @param speed speed of the movement
 * @param loop true to make the movement return to the beginning
 * once finished
 * @param ignore_obstacles true to make the movement ignore obstacles
 * @param must_be_aligned true to snap the entity to the map grid before moving it
 */
PathMovement::PathMovement(const std::string &path, int speed,
    bool loop, bool ignore_obstacles, bool must_be_aligned):

  PixelMovement("", 0, false, ignore_obstacles),
  current_direction(6),
  total_distance_covered(0),
  stopped_by_obstacle(false),
  speed(speed),
  loop(loop),
  snap_to_grid(must_be_aligned),
  snapping(false),
  stop_snapping_date(0) {

  set_path(path);
}

/**
 * @brief Destructor.
 */
PathMovement::~PathMovement() {

}

/**
 * @brief Sets the path of this movement.
 * @param path the succession of basic moves
 * composing this movement (each character represents
 * a direction between '0' and '7')
 */
void PathMovement::set_path(const std::string &path) {

  this->initial_path = path;
  restart();
}

/**
 * @brief Returns the speed of this movement.
 * @return the speed in pixels per second
 */
int PathMovement::get_speed() {
  return speed;
}

/**
 * @brief Sets the speed of this movement.
 * @param speed the speed in pixels per second
 */
void PathMovement::set_speed(int speed) {
  this->speed = speed;
}

/**
 * @brief Returns whether this movement loops when the end of the path is reached.
 * @return true if the movement loops
 */
bool PathMovement::get_loop() {

  return loop;
}

/**
 * @brief Sets whether this movement restarts when the path is finished.
 *
 * If the movement was finished and loop is set to true, the movement restarts.
 *
 * @param loop true to make the movement loop
 */
void PathMovement::set_loop(bool loop) {

  this->loop = loop;

  if (PixelMovement::is_finished() && remaining_path.size() == 0 && loop) {
    restart();
  }
}

/**
 * @brief Sets whether the entity is snapped to the map grid before the path starts.
 * @param snap_to_grid true to make the entity aligned to the grid
 */
void PathMovement::set_snap_to_grid(bool snap_to_grid) {

  this->snap_to_grid = snap_to_grid;
}

/**
 * @brief Sets the entity to be controlled by this movement object.
 * @param entity the entity to control
 */
void PathMovement::set_entity(MapEntity *entity) {

  Movement::set_entity(entity);
  restart();
}

/**
 * @brief Updates the movements: detects the collisions
 * in order to restart the movement.
 */
void PathMovement::update() {

  PixelMovement::update();

  if (!is_suspended() && is_current_elementary_move_finished()) {
    start_next_elementary_move();
  }
}

/**
 * @brief Suspends or resumes this movement.
 * @param suspended true to suspend the movement, false to resume it
 */
void PathMovement::set_suspended(bool suspended) {

  PixelMovement::set_suspended(suspended);

  if (!suspended) {
    stop_snapping_date += System::now() - get_when_suspended();
  }
}

/**
 * @brief Returns whether the movement is finished.
 * @return true if the end of the path was reached or the entity 
 * was stopped by an obstacle
 */
bool PathMovement::is_finished() {

  /*
  std::cout << "PathMovement::is_finished() : "
      << ((remaining_path.size() == 0 && !loop) || stopped_by_obstacle)
      << ", remaining path: " << remaining_path
      << ", stopped by obstacle: " << stopped_by_obstacle
      << "\n";
*/

  return (PixelMovement::is_finished() && remaining_path.size() == 0 && !loop)
      || stopped_by_obstacle;
}

/**
 * @brief Starts or restarts the movement.
 */
void PathMovement::restart() {

  this->remaining_path = initial_path;
  this->snapping = false;
  this->stop_snapping_date = 0;
  this->stopped_by_obstacle = false;

  start_next_elementary_move();
}

/**
 * @brief This function is called when a one-pixel step of the current elementary move just occured.
 * @param step_index index of the one-pixel step in the elementary move (the first one is 0)
 * @param success true if the one-pixel move was made, false if the movement was stopped by an obstacle
 */
void PathMovement::notify_step_done(int step_index, bool success) {

  if (success) {
    if (!snapping) {
      total_distance_covered++;
    }
  }
  else {
    stopped_by_obstacle = true;
  }
}

/**
 * @brief Returns whether the current move of the path is finished.
 * @return true if the current move is finished
 */
bool PathMovement::is_current_elementary_move_finished() {

  return PixelMovement::is_finished();
}

/**
 * @brief Starts the next 8-pixel trajectory of the path movement.
 *
 * This function is called when an 8-pixel trajectory of the movement is finished,
 * or when the movement is restarted.
 * Before starting the 8-pixel move, if the property must_be_aligned is true,
 * the entity's top-left corner tries to get aligned with the 8*8 squares of the grid.
 */
void PathMovement::start_next_elementary_move() {

  //std::cout << "start next elementary move, remaining path = '" << remaining_path << "'\n";

  MapEntity *entity = get_entity();

  // don't move while the entity is unknown
  if (entity == NULL) {
    return;
  }

  // before starting the move, check that the entity is aligned with the 8*8 squares grid if necessary
  if (snap_to_grid && !entity->is_aligned_to_grid()) {
 
    // the entity has to be aligned but is not
    snap();
  }

  // start the next step if we are ready
  if (!snap_to_grid || entity->is_aligned_to_grid()) {

    snapping = false;

    if (remaining_path.size() == 0) {
      // the path is finished
      if (loop) {
        // if the property 'loop' is true, repeat the same path again
        remaining_path = initial_path;
      }
      else if (!is_stopped()) {
        // the movement is finished: stop the entity
        stop();
      }
    }

    if (remaining_path.size() != 0) {
      // normal case: there is a next trajectory to do

      current_direction = remaining_path[0] - '0';
      Debug::check_assertion(current_direction >= 0 && current_direction < 8,
          StringConcat() << "Invalid path '" << initial_path << "' (bad direction '" << remaining_path[0] << "')");

      PixelMovement::set_delay(speed_to_delay(speed, current_direction));
      PixelMovement::set_trajectory(elementary_moves[current_direction]);
      remaining_path = remaining_path.substr(1);
    }
  }
}

/**
 * @brief Converts a speed into a delay in milliseconds between each pixel move.
 * @param speed the speed in pixels per seconds, as specified in the constructor of PathMovement
 * @param direction direction of the movement (0 to 7), used to adapt the computation if the movement is diagonal
 */
uint32_t PathMovement::speed_to_delay(int speed, int direction) {

  uint32_t delay = 1000 / speed; // speed in pixels per second, delay in milliseconds
  if (direction % 2 != 0) {
    delay = (uint32_t) (delay * Geometry::SQRT_2); // diagonal move
  }
  return delay;
}

/**
 * @brief Returns an xy value representing the total distance of this movement.
 * @return the total x and y distance of this movement
 */
Rectangle PathMovement::get_xy_change() {

  Rectangle xy;
  
  std::string::const_iterator it;
  for (it = initial_path.begin(); it != initial_path.end(); it++) {
    int direction = *it - '0';
    const Rectangle &xy_move = MapEntity::direction_to_xy_move(direction);
    xy.add_xy(xy_move.get_x() * 8, xy_move.get_y() * 8);
  }

  return xy;
}

/**
 * @brief Returns the current direction in the path.
 *
 * If the movement is finished, the last direction used is returned.
 *
 * @return the current direction (0 to 7)
 */
int PathMovement::get_current_direction() {

  return current_direction;
}

/**
 * @brief Returns the total distance covered by this movement.
 * @return the total distance in pixels (diagonal moves count for the same distance as non-diagonal moves),
 * not including the possible initial snapping phase
 */
int PathMovement::get_total_distance_covered() {
  return total_distance_covered;
}

/**
 * @brief Returns the direction a sprite controlled by this movement should take.
 * @return the direction to use to display the object controlled by this movement (0 to 3)
 */
int PathMovement::get_displayed_direction4() {

  static const int displayed_directions[] = {0, 0, 1, 2, 2, 2, 3, 0};
  return displayed_directions[current_direction];
}

/**
 * Tries to move the top-left corner of the entity towards an intersection of the 8*8 grid of the map.
 * This function is called only when the entity is not already aligned to the grid.
 */
void PathMovement::snap() {

  // calculate the coordinates of the closest grid intersection from the top-left corner of the entity
  int x = get_entity()->get_top_left_x();
  int y = get_entity()->get_top_left_y();
  int snapped_x = x + 4;
  int snapped_y = y + 4;
  snapped_x -= snapped_x % 8;
  snapped_y -= snapped_y % 8;

  uint32_t now = System::now();

  if (!snapping) {
    // if we haven't started to move the entity towards an intersection of the grid, do it now
    set_snapping_trajectory(Rectangle(x, y), Rectangle(snapped_x, snapped_y));
    snapping = true;
    stop_snapping_date = now + 500; // timeout
  }
  else {
    // the entity is currently trying to move towards the closest grid intersection

    uint32_t now = System::now();
    if (now >= stop_snapping_date) {
      // we could not snap the entity after the timeout:
      // this is possible when there is an (unlikely) collision with an obstacle that is not aligned to the grid
      // (typically a statue that is being moved);
      // let's try the opposite grid intersection instead
      snapped_x += (snapped_x < x) ? 8 : -8;
      snapped_y += (snapped_y < y) ? 8 : -8;
      set_snapping_trajectory(Rectangle(x, y), Rectangle(snapped_x, snapped_y));
      stop_snapping_date = now + 500;
    }
  }
}

/**
 * @brief Computes a pixel-by-pixel trajectory (in the sense of PixelMovement) between two points and starts it.
 *
 * This function is used to make a trajectory between the initial position of the entity and the closest grid point,
 * before the actual path is started.
 *
 * @param src current position of the entity
 * @param dst snapped position
 */
void PathMovement::set_snapping_trajectory(const Rectangle &src, const Rectangle &dst) {

  std::list<Rectangle> trajectory;
  Rectangle xy = src;
  while (!xy.equals_xy(dst)) {

    int dx = dst.get_x() - xy.get_x();
    int dy = dst.get_y() - xy.get_y();

    if (dx > 0) {
      dx = 1;
    }
    else if (dx < 0) {
      dx = -1;
    }

    if (dy > 0) {
      dy = 1;
    }
    else if (dy < 0) {
      dy = -1;
    }

    trajectory.push_back(Rectangle(dx, dy));
    xy.add_xy(dx, dy);
  }
  PixelMovement::set_trajectory(trajectory);
  PixelMovement::set_delay(speed_to_delay(speed, 0)); // don't bother adjusting the speed of diagonal moves
  PixelMovement::set_loop(false);
}


/**
 * @brief Returns a string describing a path with random length in one of the four main directions.
 * @return a random path
 */
const std::string PathMovement::create_random_path() {

  char c = '0' + (Random::get_number(4) * 2);
  int length = Random::get_number(5) + 3;
  std::string path = "";
  for (int i = 0; i < length; i++) {
    path += c;
  }

  return path;
}

/**
 * @brief Returns the value of a property of this movement.
 *
 * Accepted keys:
 * - path
 * - speed
 * - loop
 * - ignore_obstacles
 * - snap_to_grid
 *
 * @param key key of the property to get
 * @return the corresponding value as a string
 */
const std::string PathMovement::get_property(const std::string &key) {

  std::ostringstream oss;

  if (key == "path") {
    oss << initial_path;
  }
  else if (key == "speed") {
    oss << speed;
  }
  else if (key == "loop") {
    oss << loop;
  }
  else if (key == "ignore_obstacles") {
    oss << are_obstacles_ignored();
  }
  else if (key == "snap_to_grid") {
    oss << snap_to_grid;
  }
  else {
    Debug::die(StringConcat() << "Unknown property of PathMovement: '" << key << "'");
  }

  return oss.str();
}

/**
 * @brief Sets the value of a property of this movement.
 *
 * Accepted keys:
 * - path
 * - speed
 * - loop
 * - ignore_obstacles
 * - snap_to_grid
 *
 * @param key key of the property to set (the accepted keys depend on the movement type)
 * @param value the value to set
 */
void PathMovement::set_property(const std::string &key, const std::string &value) {

  std::istringstream iss(value);

  if (key == "path") {
    std::string path;
    iss >> path;
    set_path(path);
  }
  else if (key == "speed") {
    int speed;
    iss >> speed;
    set_speed(speed);
  }
  else if (key == "loop") {
    bool loop;
    iss >> loop;
    set_loop(loop);
  }
  else if (key == "ignore_obstacles") {
    bool ignore_obstacles;
    iss >> ignore_obstacles;
    set_default_ignore_obstacles(ignore_obstacles);
  }
  else if (key == "snap_to_grid") {
    bool snap_to_grid;
    iss >> snap_to_grid;
    set_snap_to_grid(snap_to_grid);
  }
  else {
    Debug::die(StringConcat() << "Unknown property of PathMovement: '" << key << "'");
  }
}

