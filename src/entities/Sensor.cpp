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
#include "entities/Sensor.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "Game.h"
#include "Map.h"
#include "lua/MapScript.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"

/**
 * @brief Constructor.
 * @param name name of the entity
 * @param layer layer of the entity
 * @param x x position of the entity's rectangle
 * @param y y position of the entity's rectangle
 * @param width width of the entity's rectangle 
 * @param height height of the entity's rectangle 
 * @param subtype the subtype of sensor
 */
Sensor::Sensor(const std::string &name, Layer layer, int x, int y,
	       int width, int height, Subtype subtype):
  Detector(COLLISION_CUSTOM, name, layer, x, y, width, height),
  subtype(subtype), hero_already_overlaps(false) {

  if (subtype == RETURN_FROM_BAD_GROUND) {
    
    Debug::check_assertion(width == 16 && height == 16,
	StringConcat() << "This place to return from bad grounds has an incorrect size: " << width << "x" << height);
  }

  set_origin(8, 13);
}

/**
 * @brief Destructor.
 */
Sensor::~Sensor() {

}

/**
 * @brief Creates an instance from an input stream.
 *
 * The input stream must respect the syntax of this entity type.
 *
 * @param game the game that will contain the entity created
 * @param is an input stream
 * @param layer the layer
 * @param x x coordinate of the entity
 * @param y y coordinate of the entity
 * @return the instance created
 */
MapEntity* Sensor::parse(Game &game, std::istream &is, Layer layer, int x, int y) {

  std::string name;
  int width, height, subtype;

  FileTools::read(is, width);
  FileTools::read(is, height);
  FileTools::read(is, name);
  FileTools::read(is, subtype);
 
  return new Sensor(name, Layer(layer), x, y, width, height, Subtype(subtype));
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Sensor::get_type() {
  return SENSOR;
}
    
/**
 * @brief Returns the subtype of this sensor.
 * @return the subtype
 */
Sensor::Subtype Sensor::get_subtype() {
  return subtype;
}

/**
 * @brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @return true if this entity can collide with entities that are on another layer
 */
bool Sensor::has_layer_independent_collisions() {

  // check the collisions with the hero even if he is not on the same layer yet
  return subtype == CHANGE_LAYER;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool Sensor::is_obstacle_for(MapEntity &other) {

  return other.is_sensor_obstacle(*this);
}

/**
 * @brief Checks whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool Sensor::test_collision_custom(MapEntity &entity) {

  const Rectangle &entity_rectangle = entity.get_bounding_box();
  int x1 = entity_rectangle.get_x() + 4;
  int x2 = x1 + entity_rectangle.get_width() - 9;
  int y1 = entity_rectangle.get_y() + 4;
  int y2 = y1 + entity_rectangle.get_height() - 9;

  bool collision = overlaps(x1, y1) && overlaps(x2, y1) &&
    overlaps(x1, y2) && overlaps(x2, y2);

  if (entity.is_hero() && !collision) {
    this->hero_already_overlaps = false;
  }

  return collision;
}

/**
 * @brief This function is called by the engine when an entity overlaps this sensor.
 *
 * This is a redefinition of Detector::collision().
 *
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Sensor::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_sensor(*this);
}

/**
 * @brief Activates this sensor.
 *
 * This function is called when the hero overlaps the sensor.
 *
 * @param hero the hero
 */
void Sensor::activate(Hero &hero) {

  if (!hero_already_overlaps) {
 
    hero_already_overlaps = true;

    switch (subtype) {

      case CUSTOM:
	// we notify the scripts
	get_map_script().event_hero_on_sensor(get_name());
	get_hero().reset_movement();
	break;

      case CHANGE_LAYER:
	// we change the hero's layer
	get_entities().set_entity_layer(&hero, get_layer());
	break;

      case RETURN_FROM_BAD_GROUND:
	// we indicate to the hero a location to return
	// after falling into a hole or some other ground
	get_hero().set_target_solid_ground_coords(get_xy(), get_layer());
	break;
    }
  }
}

