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
#ifndef ZSDX_JUMP_SENSOR_H
#define ZSDX_JUMP_SENSOR_H

#include "Common.h"
#include "Detector.h"

/**
 * A jump sensor is a detector placed on a map.
 * When the hero touches this detetor, he makes a jump with a specified length.
 */
class JumpSensor: public Detector {

 private:

  int jump_length; /**< length of the jump in pixels (should be a multiple of 8) */

 public:

  JumpSensor(const std::string &name, Layer layer, int x, int y, int width, int height,
	     int direction, int jump_length);
  ~JumpSensor(void);

  EntityType get_type(void);

  bool check_collision_custom(MapEntity *entity);
  bool is_point_in_diagonal(const SDL_Rect &point);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);

};

#endif
