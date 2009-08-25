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
#ifndef ZSDX_DETECTOR_H
#define ZSDX_DETECTOR_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * A detector is an object placed somewhere on the map
 * to detect the presence of the hero or other moving entities.
 * Examples of detectors are teletransporters, switches, bushes, enemies, etc.
 */
class Detector: public MapEntity {

 public:

  /**
   * Collisions modes of a detector.
   * The collision mode indicates what kind of collision the detector will detect.
   */
  enum CollisionMode {
    COLLISION_NONE         = 0x0000, /**< no collision will be detected
				      * (the detector doesn't detect any entity) */
    COLLISION_RECTANGLE    = 0x0001, /**< collision if an entity's rectangle
				      * overlaps the detector's rectangle */
    COLLISION_ORIGIN_POINT = 0x0002, /**< collision if an entity's origin point
				      * is inside the detector's rectangle */
    COLLISION_FACING_POINT = 0x0004, /**< collision if an entity's facing point
				      * is inside the detector's rectangle */
    COLLISION_CENTER       = 0x0008, /**< collision if the entity's center
				      * is inside the detector's rectangle */
    COLLISION_SPRITE       = 0x0010, /**< collision if an entity's sprite has pixels
				      * overlapping pixels of the detector's sprite */
    COLLISION_CUSTOM       = 0x0020, /**< custom collision function, defined by the subclass */
  };

 private:

  int collision_modes; /**< collision modes of the detector
		        * (can be an OR combination of CollisionMode values) */

  bool layer_ignored;  /**< indicates that collisions are detected even
		        * with entities that are not on the same layer than the detector */

 protected:

  // construction
  Detector(int collision_mode, const std::string &name, Layer layer,
	   int x, int y, int width, int height);

  // properties of the detector
  void set_collision_modes(int collision_modes);
  void add_collision_mode(CollisionMode collision_mode);
  bool has_collision_mode(CollisionMode collision_mode);
  void set_layer_ignored(bool layer_ignored);

  // specialized collision checking functions
  bool test_collision_rectangle(MapEntity *entity);
  bool test_collision_origin_point(MapEntity *entity);
  bool test_collision_facing_point(MapEntity *entity);
  bool test_collision_center(MapEntity *entity);
  virtual bool test_collision_custom(MapEntity *entity);

  // functions called when a collision is detected
  virtual void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
  virtual void notify_collision(MapEntity *entity, Sprite *sprite_overlapping);

 public:

  // destruction
  virtual ~Detector(void);

  // general collision checking functions
  void check_collision(MapEntity *entity);
  void check_collision(MapEntity *entity, Sprite *sprite);

  virtual void action_key_pressed(void);
  virtual bool interaction_with_inventory_item(InventoryItem *item);
  virtual bool moved_by_hero(void);
};

#endif

