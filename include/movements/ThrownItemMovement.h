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
#ifndef ZSDX_THROWN_ITEM_MOVEMENT_H
#define ZSDX_THROWN_ITEM_MOVEMENT_H

#include "Common.h"
#include "MovementWithCollision.h"

/**
 * Movement used for an item thrown by the hero
 * (a pot, a bush, etc.).
 */
class ThrownItemMovement: public MovementWithCollision {

 private:

  int y_increment;
  Uint32 next_down_date;
  int item_height;

 protected:

  void update_x(void);
  void update_y(void);

  bool collision_with_map(int dx, int dy);

 public:

  ThrownItemMovement(Map *map, int direction);
  ~ThrownItemMovement(void);

  int get_item_height(void);

  void set_suspended(bool suspended);
};

#endif
