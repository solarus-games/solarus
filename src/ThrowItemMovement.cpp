#include "ThrowItemMovement.h"

/**
 * Creates a throw item movement.
 * @param map the map where the item is thrown
 * @param direction direction where the item is thrown (0 to 3)
 */
ThrowItemMovement::ThrowItemMovement(Map *map, int direction):
  MovementWithCollision() {

  set_map(map);
  set_speed(20);
  set_direction(direction * 90);

  y_increment = -2;
  next_down_date = SDL_GetTicks() + 40;
}

/**
 * Destructor.
 */
ThrowItemMovement::~ThrowItemMovement(void) {

}

/**
 * Updates the x position.
 */
void ThrowItemMovement::update_x(void) {
  MovementWithCollision::update_x();
}

/**
 * Updates the y position.
 */
void ThrowItemMovement::update_y(void) {
  MovementWithCollision::update_y();

  if (y_increment < 7 && !is_stopped()) {
    Uint16 now = SDL_GetTicks();
    while (now >= next_down_date) {
      translate_y(y_increment);
      next_down_date += 40;
      y_increment++;
    }
  }
  else {
    stop();
  }
}

/**
 * Suspends or resumes the movement.
 * This function is called by the entity when the game is suspended or resumed.
 * @param suspended true to suspend the movement, false to resume it
 */
void ThrowItemMovement::set_suspended(bool suspended) {
  MovementWithCollision::set_suspended(suspended);

  if (!suspended) {
    // recalculate next_down_date
    if (when_suspended != 0) {
      Uint16 now = SDL_GetTicks();
      next_down_date = now + (next_down_date - when_suspended);
    }
  }
}

/**
 * Returns the current height of the entity.
 * @return the entity's height
 */
int ThrowItemMovement::get_item_height(void) {
  return 18 - y_increment;
}
