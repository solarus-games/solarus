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
#ifndef ZSDX_RANDOM_WALK_MOVEMENT_H
#define ZSDX_RANDOM_WALK_MOVEMENT_H

#include "Common.h"
#include "movements/PathMovement.h"

/**
 * Movement for an entity that walks randomly.
 */
class RandomWalkMovement: public PathMovement {

 private:

  static const std::string random_directions[];

 public:

  RandomWalkMovement(Map *map, int speed);
  ~RandomWalkMovement(void);

  void update(void);
  void start_next_move(void);
  bool is_finished(void);

 protected:

  static const std::string get_random_direction(void);

};

#endif
