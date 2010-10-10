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
#ifndef SOLARUS_BOUNDED_RANDOM_MOVEMENT_H
#define SOLARUS_BOUNDED_RANDOM_MOVEMENT_H

#include "Common.h"
#include "movements/RectilinearMovement.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief A movement that makes successive steps of random direction 
 * and stays inside a limited rectangle.
 */
class BoundedRandomMovement: public RectilinearMovement {

  private:

    Rectangle bounds;					/**< a rectangle the fairy cannot escape from */
    uint32_t next_direction_change_date;		/**< date of the next direction change */

    void set_next_direction();

  public:

    BoundedRandomMovement();
    ~BoundedRandomMovement();

    void set_entity(MapEntity *entity);
    void update();
    void set_suspended(bool suspended);
};

#endif

