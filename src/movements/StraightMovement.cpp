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

  this->finished = false;
  this->end_movement_date = SDL_GetTicks() + time;
  set_speed(speed);
  set_direction(direction);
}

/**
 * Destructor.
 */
StraightMovement::~StraightMovement(void) {

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
