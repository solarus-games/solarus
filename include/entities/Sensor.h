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
#ifndef ZSDX_SENSOR_H
#define ZSDX_SENSOR_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * A sensor is an invisible entity that triggers something
 * when the hero or another entity walks on it. Unlike the switches,
 * the hero cannot avoid a sensor by jumping.
 */
class Sensor: public Detector {

 public:

  /**
   * Subtypes of sensors.
   */
  enum Subtype {
    CUSTOM                 = 0, /**< the map script is called when the hero walks on this sensor */
    CHANGE_LAYER           = 1, /**< the hero's layer is changed when he comes on this sensor
	                         * (the hero's layer becomes the sensor's layer) */
    RETURN_FROM_BAD_GROUND = 2, /**< when falling in a hole or in other bad grounds, the hero
	                         * comes back at the location of this sensor */
  };

 private:

  Subtype subtype;            /**< subtype of teletransporter */
  bool hero_already_overlaps; /**< true if the hero is currently on this sensor */

 public:

  Sensor(std::string name, Layer layer, int x, int y, int width, int height, Subtype subtype);
  ~Sensor(void);

  EntityType get_type(void);

  bool is_obstacle_for(MapEntity *other);
  bool check_collision_custom(MapEntity *entity);
  void collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
};

#endif
