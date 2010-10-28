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
#include "movements/BoundedRandomMovement.h"
#include "entities/MapEntity.h"
#include "lowlevel/System.h"
#include "lowlevel/Random.h"
#include "lowlevel/Geometry.h"

/**
 * @brief Constructor.
 */
BoundedRandomMovement::BoundedRandomMovement():
  RectilinearMovement(true) {

  set_speed(3);
  set_next_direction();
}

/**
 * @brief Destructor.
 */
BoundedRandomMovement::~BoundedRandomMovement() {

}

/**
 * @brief Sets the entity to be controlled by this movement object.
 * @param entity the entity to control
 */
void BoundedRandomMovement::set_entity(MapEntity *entity) {

  Movement::set_entity(entity);

  // restrict the movement in a 80*80 rectangle
  bounds.set_xy(entity->get_xy());
  bounds.add_xy(-40, -40);
  bounds.set_size(80, 80);
}

/**
 * @brief Chooses a new direction for the fairy.
 */
void BoundedRandomMovement::set_next_direction() {

  if (get_entity() == NULL || bounds.contains(get_x(), get_y())) {

    // we are inside the bounds: we pick a random direction
    int nb = Random::get_number(8);
    set_direction(nb * 45 + 22); // 8 possible directions but no "simple" direction
  }
  else {

    // we are outside the bounds: we get back into the rectangle to avoid going to far
    double angle = Geometry::get_angle(get_x(), get_y(), bounds.get_x() + bounds.get_width() / 2, bounds.get_y() + bounds.get_height() / 2);
    set_direction_radians(angle);
  }

  next_direction_change_date = System::now() + 500 + Random::get_number(3) * 500; // change again in 0.5 to 2 seconds
}

/**
 * @brief Updates the fairy movement.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of Movement::update()
 * to change the fairy's direction sometimes.
 */
void BoundedRandomMovement::update() {

  RectilinearMovement::update();

  if (!is_suspended()) {

    uint32_t now = System::now();
    if (now >= next_direction_change_date) {
      set_next_direction();
    }
  }
}

/**
 * @brief Suspends or resumes this movement.
 * @param suspended true to suspend the movement, false to resume it
 */
void BoundedRandomMovement::set_suspended(bool suspended) {

  RectilinearMovement::set_suspended(suspended);

  if (!suspended) {
    next_direction_change_date += System::now() - get_when_suspended();
  }
}

