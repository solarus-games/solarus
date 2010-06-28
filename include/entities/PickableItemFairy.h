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
#ifndef SOLARUS_PICKABLE_ITEM_FAIRY_H
#define SOLARUS_PICKABLE_ITEM_FAIRY_H

#include "Common.h"
#include "PickableItem.h"

/**
 * @brief Represents a fairy that the hero can take to increase his life.
 *
 * The fairy moves on the map with an instance of FairyMovement.
 */
class PickableItemFairy: public PickableItem {

  private:

    Movement *movement;

  protected:

    // initialization
    void initialize_movement(void);

  public:

    // creation and destruction
    PickableItemFairy(int x, int y);
    ~PickableItemFairy(void);

    bool has_layer_independent_collisions(void);

    // update
    void give_item_to_player(void);
    void update(void);
};

#endif

