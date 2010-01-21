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
#ifndef SOLARUS_PICKABLE_ITEM_HEART_H
#define SOLARUS_PICKABLE_ITEM_HEART_H

#include "Common.h"
#include "entities/PickableItem.h"

/**
 * Represents some life placed on the map, that the hero can pick.
 */
class PickableItemHeart: public PickableItem {

  protected:

    void initialize_sprites(void);
    void initialize_movement(void);

  public:

    PickableItemHeart(Layer layer, int x, int y);
    ~PickableItemHeart(void);

    void update(void);
};

#endif

