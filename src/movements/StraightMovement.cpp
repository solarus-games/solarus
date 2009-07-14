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
#include "movements/StraightMovement.h"
#include "Geometry.h"

/**
 * Creates a straight movement.
 * @param speed the speed
 * @param direction angle of the movement (0 to 359)
 * @param time duration of the movement in milliseconds
 */
StraightMovement::StraightMovement(int speed, int direction, Uint32 time) {
  start(speed, direction, time);
}

/**
 * Creates a straight movement.
 * @param speed the speed
 * @param direction angle of the movement in radians
 * @param time duration of the movement in milliseconds
 */
StraightMovement::StraightMovement(int speed, double direction, Uint32 time) {

  start(speed, direction, time);
}

/**
 * Destructor.
 */
StraightMovement::~StraightMovement(void) {

}

/**
 * Starts the straight movement into a direction.
 * @param speed the speed
 * @param direction angle of the movement (0 to 359)
 * @param time duration of the movement in milliseconds
 */
void StraightMovement::start(int speed, int direction, Uint32 time) {

  start(speed, direction * Geometry::TWO_PI / 360.0, time);
}

/**
 * Starts the straight movement into a direction.
 * @param speed the speed
 * @param direction angle of the movement in radians
 * @param time duration of the movement in milliseconds
 */
void StraightMovement::start(int speed, double direction, Uint32 time) {

  finished = false;
  end_movement_date = SDL_GetTicks() + time;
  set_speed(speed);
  if (speed != 0) {
    set_direction(direction);
  }
}

/**
 * Updates the movement.
 */
void StraightMovement::update(void) {
  Movement::update();

  Uint32 now = SDL_GetTicks();
  if (now >= end_movement_date) {
    stop();
    finished = true;
  }
}

/**
 * Suspends or resumes the movement
 */
void StraightMovement::set_suspended(bool suspended) {

  Movement::set_suspended(suspended);

  if (!suspended) {
    end_movement_date += SDL_GetTicks() - when_suspended;
  }
}

/**
 * Returns whether the movement is finished.
 * This functions returns false while the delay is not finished,
 * even if the movement was stopped by an obstacle.
 * @return true if the movement is finished
 */
bool StraightMovement::is_finished(void) {
  return finished;
}
