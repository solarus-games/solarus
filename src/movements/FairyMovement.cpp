#include "movements/FairyMovement.h"
#include "Random.h"

/**
 * Creates a fairy movement.
 */
FairyMovement::FairyMovement(void) {
  set_speed(4);
  set_random_direction();
}

/**
 * Destructor.
 */
FairyMovement::~FairyMovement(void) {

}

/**
 * Sets a random direction for the fairy.
 */
void FairyMovement::set_random_direction(void) {

  int nb = Random::get_number(8);
  set_direction(nb * 45 + 22); // 8 possible directions but no "simple" direction

  next_direction_change = SDL_GetTicks() + 2000; // change again in 2 seconds
}

/**
 * Updates the fairy movement.
 * This function is called repeatedly by the map.
 * This is a redefinition of Movement::update()
 * to change the fairy's direction sometimes.
 */
void FairyMovement::update(void) {

  Movement::update();

  if (!suspended) {

    Uint32 now = SDL_GetTicks();
    if (now >= next_direction_change) {
      set_random_direction();
    }
  }
}
