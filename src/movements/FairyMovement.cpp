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
#include "movements/FairyMovement.h"
#include "lowlevel/Random.h"

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

  if (!is_suspended()) {

    Uint32 now = SDL_GetTicks();
    if (now >= next_direction_change) {
      set_random_direction();
    }
  }
}
