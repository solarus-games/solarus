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
#include "entities/Sensor.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "ZSDX.h"
#include "Game.h"
#include "Map.h"
#include "MapScript.h"

/**
 * Constructor.
 * @param name name of the entity
 * @param layer layer of the entity
 * @param x x position of the entity's rectangle
 * @param y y position of the entity's rectangle
 * @param subtype the subtype of switch
 * @param needs_block true if a block is required to enabled this switch
 * @param disable_when_leaving true to disable the switch when the hero or 
 * the block leaves it
 */
Sensor::Sensor(std::string name, Layer layer, int x, int y,
	       int width, int height, Subtype subtype):
  Detector(COLLISION_CUSTOM, name, layer, x, y, width, height),
  subtype(subtype), hero_already_overlaps(false) {

  if (subtype == RETURN_FROM_BAD_GROUND && (width != 16 || height != 16)) {
    DIE("This place to return from bad grounds has an incorrect size: " << width << "x" << height);
  }

  set_origin(8, 13);

  if (subtype == CHANGE_LAYER) {
    // check the collisions with the hero even if he is not on the same layer yet
    set_layer_ignored(true);
  }
}

/**
 * Destructor.
 */
Sensor::~Sensor(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Sensor::get_type() {
  return SENSOR;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Sensor::is_obstacle_for(MapEntity *other) {
  return other->is_sensor_obstacle(this);
}

/**
 * Checks whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool Sensor::check_collision_custom(MapEntity *entity) {

  const SDL_Rect &entity_position = entity->get_position_in_map();
  int x1 = entity_position.x + 4;
  int x2 = x1 + entity_position.w - 5;
  int y1 = entity_position.y + 4;
  int y2 = y1 + entity_position.h - 5;

  const SDL_Rect &sensor_position = get_position_in_map();

  bool collision = is_point_in(sensor_position, x1, y1) &&
    is_point_in(sensor_position, x2, y1) &&
    is_point_in(sensor_position, x1, y2) &&
    is_point_in(sensor_position, x2, y2);

  if (entity->is_hero() && !collision) {
    this->hero_already_overlaps = false;
  }

  return collision;
}

/**
 * This function is called by the engine when an entity overlaps this sensor.
 * This is a redefinition of Detector::collision().
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Sensor::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  // for now the sensors only apply to the hero
  if (entity_overlapping->is_hero() && !hero_already_overlaps) {

    Hero *hero = (Hero*) entity_overlapping;
    hero_already_overlaps = true;

    switch (subtype) {

      case CUSTOM:
	// we call the map script
	map->get_script()->event_hero_on_sensor(get_name());
	break;

      case CHANGE_LAYER:
	// we change the hero's layer
	map->get_entities()->set_hero_layer(this->get_layer());
	break;

      case RETURN_FROM_BAD_GROUND:
	// we indicate to the hero a location to return
	// after falling into a hole or some other ground
	hero->set_target_solid_ground_coords(get_coordinates());
	break;

    }
  }
}
