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
#ifndef SOLARUS_FAIRY_MOVEMENT_H
#define SOLARUS_FAIRY_MOVEMENT_H

#include "Common.h"
#include "movements/Movement.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Defines the movement of a pickable fairy.
 *
 * This movement is typically used by the PickableItemFairy entity.
 * The entity makes successive small straight steps
 * and changes its direction randomly.
 * It stays inside a limited rectangle.
 */
class FairyMovement: public Movement {

  private:

    Rectangle bounds;                        /**< a rectangle the fairy cannot escape from */
    uint32_t next_direction_change_date;     /**< date of the next direction change */

    void set_next_direction();

  public:

    FairyMovement();
    ~FairyMovement();

    void set_entity(MapEntity *entity);
    void update();
};

#endif

