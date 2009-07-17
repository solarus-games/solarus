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
#ifndef ZSDX_CONVEYOR_BELT_H
#define ZSDX_CONVEYOR_BELT_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * A special terrain where the hero is moved towards a specific direction.
 */
class ConveyorBelt: public Detector {

 private:

   bool enabled;

 public:

  ConveyorBelt(Layer layer, int x, int y, int width, int height, int direction);
  ~ConveyorBelt(void);

  EntityType get_type(void);

  bool is_enabled(void);
  void set_enabled(bool enabled);

  bool is_obstacle_for(MapEntity *other);
  bool check_collision_custom(MapEntity *entity);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);

  void update(void);
  void display_on_map(void);
};

#endif
