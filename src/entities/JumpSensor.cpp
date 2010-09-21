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
#include "entities/JumpSensor.h"
#include "entities/Hero.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"

/**
 * @brief Creates a jump sensor.
 * @param name a name identifying this jump sensor
 * @param layer layer of the sensor on the map
 * @param x x coordinate of the top-left corner of the sensor's rectangle
 * @param y y coordinate of the top-left corner of the sensor's rectangle
 * @param width width of the sensor's rectangle
 * @param height height of the sensor's rectangle
 * @param direction direction of the jump (0 to 7 as the jump may be diagonal)
 * @param jump_length length of the jump in pixels (usually a multiple of 8)
 */
JumpSensor::JumpSensor(const std::string &name, Layer layer, int x, int y, int width, int height,
		       int direction, int jump_length):
  Detector(COLLISION_CUSTOM, name, layer, x, y, width, height),
  jump_length(jump_length) {

  set_direction(direction);

  // check the size
  if (direction % 2 != 0) {
    Debug::assert(width == height, "This jump sensor has a diagonal direction but is not square");
  }
  else {
    if (direction % 4 == 0) {
      Debug::assert(width == 8, "This jump sensor is horizontal but its height is not 8");
    }
    else {
      Debug::assert(height == 8, "This jump sensor is vertical but its width is not 8");
    }
  }
  // check the jump length
  Debug::assert(jump_length >= 16, "The jump length of this jump sensor is lower than 16");
}

/**
 * @brief Destructor.
 */
JumpSensor::~JumpSensor() {

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
MapEntity * JumpSensor::parse(Game *game, std::istream &is, Layer layer, int x, int y) {

  int jump_length, width, height, direction;
  std::string name;

  FileTools::read(is, width);
  FileTools::read(is, height);
  FileTools::read(is, name);
  FileTools::read(is, direction);
  FileTools::read(is, jump_length);
 
  return new JumpSensor(name, Layer(layer), x, y, width, height, direction, jump_length);
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType JumpSensor::get_type() {
  return JUMP_SENSOR;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this entity is an obstacle for the other one
 */
bool JumpSensor::is_obstacle_for(MapEntity *other) {

  if (direction % 2 != 0) {
    return false; // diagonal jump sensor: never obstacle (the tiles below the jump sensor should block entities)
  }

  // non-diagonal jump sensor: it depends on the entities (the tiles below the jump sensor should NOT block entities)
  return other->is_jump_sensor_obstacle(this);
}

/**
 * @brief Returns whether an entity's collides with this jump sensor.
 *
 * The result depends on the sensor's shape.
 *
 * @param entity the entity
 * @return true if the entity's collides with this jump sensor
 */
bool JumpSensor::test_collision_custom(MapEntity *entity) {

  if (!entity->is_hero()) {
    return false;
  }
  Hero *hero = (Hero*) entity;

  // if the sensor's has one of the four main directions, then
  // its shape is exactly its rectangle
  if (direction % 2 == 0) {

    if (!hero->is_moving_towards(direction / 2)) {
      return false;
    }

    bool horizontal = (direction % 4 == 0);
    const Rectangle &facing_point = hero->get_facing_point(direction / 2);
    return overlaps(facing_point.get_x() + (horizontal ? 0 : -8),
		    facing_point.get_y() + (horizontal ? -8 : 0))
      && overlaps(facing_point.get_x() + (horizontal ? 0 : 7),
		  facing_point.get_y() + (horizontal ? 7 : 0));
  }

  // otherwise, the sensor's shape is a diagonal bar

  return is_point_in_diagonal(hero->get_facing_point((direction - 1) / 2))
    || is_point_in_diagonal(hero->get_facing_point((direction + 1) % 8 / 2));
}

/**
 * @brief Returns whether the specified point is in the jump sensor's shape.
 *
 * This function is used only for a jump sensor with diagonal direction.
 *
 * @param point the point to check
 * @return true if this point is overlapping the jump sensor
 */
bool JumpSensor::is_point_in_diagonal(const Rectangle &point) {

  if (!overlaps(point.get_x(), point.get_y())) {
    return false;
  }

  bool collision = false;
  int x = point.get_x() - this->get_x();
  int y = point.get_y() - this->get_y();
  int width = get_width();

  switch (direction) {

  case 1:
    collision = (y >= x) && (y - 8 < x);
    break;

  case 3:
    collision = (x + y <= width) && (x + y > width - 8);
    break;

  case 5:
    collision = (x >= y) && (x - 8 < y);
    break;

  case 7:
    collision = (x + y >= width) && (x + y < width + 8);
    break;

  default:
    Debug::die("Invalid direction of jump sensor");
  }

  return collision;
}

/**
 * @brief This function is called when an entity overlaps the sensor.
 * @param entity_overlapping the entity that overalps the sensor
 * @param collision_mode the collision mode that triggered the event
 * (not used here since a jump sensor has only one collision mode)
 */
void JumpSensor::notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {
  entity_overlapping->notify_collision_with_jump_sensor(this);
}

/**
 * @brief Returns the length of the jump to make with this jump sensor.
 * @return length of the jump in pixels (usually a multiple of 8)
 */
int JumpSensor::get_jump_length() {
  return jump_length;
}

