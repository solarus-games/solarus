#include "movements/StraightMovement.h"

/**
 * Creates a straight movement.
 * @param map the map
 * @param speed the speed
 * @param direction angle of the movement (0 to 359)
 * @param time duration of the movement in milliseconds
 */
StraightMovement::StraightMovement(Map *map, int speed, int direction, Uint32 time):
  MovementWithCollision(map) {

  start(speed, direction, time);
}

/**
 * Creates a straight movement.
 * @param map the map
 * @param speed the speed
 * @param direction angle of the movement in radians
 * @param time duration of the movement in milliseconds
 */
StraightMovement::StraightMovement(Map *map, int speed, double direction, Uint32 time):
  MovementWithCollision(map) {

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

  start(speed, direction * TWO_PI / 360.0, time);
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
 * @return true if the movement is finished
 */
bool StraightMovement::is_finished(void) {
  return finished;
}
