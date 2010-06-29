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
#include "movements/StraightMovement.h"
#include "lowlevel/Geometry.h"
#include "lowlevel/System.h"

/**
 * @brief Creates a straight movement.
 * @param speed the speed
 * @param direction angle of the movement (0 to 359)
 * @param time duration of the movement in milliseconds
 */
StraightMovement::StraightMovement(int speed, int direction, uint32_t time) {
  start(speed, direction, time);
}

/**
 * @brief Creates a straight movement.
 * @param speed the speed
 * @param direction angle of the movement in radians
 * @param time duration of the movement in milliseconds
 */
StraightMovement::StraightMovement(int speed, double direction, uint32_t time) {
  start(speed, direction, time);
}

/**
 * @brief Creates a straight movement.
 * @param speed the speed
 * @param source_xy the movement will start from this point
 * @param target_xy the movement will go into this point's direction
 * @param time duration of the movement in milliseconds
 */
StraightMovement::StraightMovement(int speed, const Rectangle &source_xy, const Rectangle &target_xy, uint32_t time) {

  double angle = Geometry::get_angle(source_xy.get_x(), source_xy.get_y(), target_xy.get_x(), target_xy.get_y());
  start(speed, angle, time);
}

/**
 * @brief Destructor.
 */
StraightMovement::~StraightMovement(void) {

}

/**
 * @brief Starts the straight movement into a direction.
 * @param speed the speed
 * @param direction angle of the movement (0 to 359)
 * @param time duration of the movement in milliseconds
 */
void StraightMovement::start(int speed, int direction, uint32_t time) {

  start(speed, direction * Geometry::TWO_PI / 360.0, time);
}

/**
 * @brief Starts the straight movement into a direction.
 * @param speed the speed
 * @param direction angle of the movement in radians
 * @param time duration of the movement in milliseconds
 */
void StraightMovement::start(int speed, double direction, uint32_t time) {

  finished = false;
  end_movement_date = System::now() + time;
  set_speed(speed);
  if (speed != 0) {
    set_direction(direction);
  }
}

/**
 * @brief Updates the movement.
 */
void StraightMovement::update(void) {
  CollisionMovement::update();

  uint32_t now = System::now();
  if (now >= end_movement_date) {
    stop();
    finished = true;
  }
}

/**
 * @brief Suspends or resumes the movement
 * @param suspended true to suspend the movement, false to resume it
 */
void StraightMovement::set_suspended(bool suspended) {

  CollisionMovement::set_suspended(suspended);

  if (!suspended) {
    end_movement_date += System::now() - when_suspended;
  }
}

/**
 * @brief Returns whether the movement is finished.
 *
 * This functions returns false while the delay is not finished,
 * even if the movement was stopped by an obstacle.
 *
 * @return true if the movement is finished
 */
bool StraightMovement::is_finished(void) {
  return finished;
}

/**
 * @brief Stops the movement finished even if the delay planned is not finished yet.
 */
void StraightMovement::set_finished(void) {
  stop();
  this->finished = true;
}

