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
 * Represents a fairy placed on the map, that the hero can take.
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

  // update
  virtual void give_item_to_player(void);
  void update(void);
};

#endif
