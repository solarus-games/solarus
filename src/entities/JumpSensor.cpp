#include "entities/JumpSensor.h"
#include "entities/Hero.h"
#include "movements/PlayerMovement.h"

/**
 * Creates a jump sensor.
 * @param name a name identifying this jump sensor
 * @param layer layer of the sensor on the map
 * @param x x coordinate of the top-left corner of the sensor's rectangle
 * @param y y coordinate of the top-left corner of the sensor's rectangle
 * @param width width of the sensor's rectangle
 * @param height height of the sensor's rectangle
 * @param direction direction of the jump (0 to 7 as the jump may be diagonal)
 * @param jump_length length of the jump in pixels (usually a multiple of 8)
 */
JumpSensor::JumpSensor(string name, Layer layer, int x, int y, int width, int height,
		       int direction, int jump_length):
  Detector(COLLISION_CUSTOM, name, layer, x, y, width, height),
  jump_length(jump_length) {

  set_direction(direction);

  // check the size
  if (direction % 2 != 0) {
    if (width != height) {
      DIE("This jump sensor has a diagonal direction but is not square");
    }
  }
  else {
    if (direction % 4 == 0) {
      if (width != 8) {
	DIE("This jump sensor is horizontal but its height is not 8");
      }
    }
    else {
      if (height != 8) {
	DIE("This jump sensor is vertical but its width is not 8");
      }
    }
  }
  // check the jump length
  if (jump_length < 16) {
    DIE("The jump length of this jump sensor is lower than 16");
  }
}

/**
 * Destructor.
 */
JumpSensor::~JumpSensor(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
MapEntity::EntityType JumpSensor::get_type() {
  return JUMP_SENSOR;
}

/**
 * Checks whether an entity's collides with this jump sensor.
 * The test depends on the sensor's shape.
 * @param entity the entity
 * @return true if the entity's collides with this jump sensor
 */
bool JumpSensor::check_collision_custom(MapEntity *entity) {

  // if the sensor's has one of the four main directions, then
  // its shape is exactly its rectangle
  if (direction % 2 == 0) {
    SDL_Rect facing_point = entity->get_facing_point(direction / 2);
    return is_point_in(get_position_in_map(), facing_point.x, facing_point.y);
  }

  // otherwise, the sensor's shape is a diagonal bar

  return is_point_in_diagonal(entity->get_facing_point((direction - 1) / 2))
    || is_point_in_diagonal(entity->get_facing_point((direction + 1) % 8 / 2));
}

/**
 * Returns whether the specified point is in the jump sensor's shape
 * (only for a jump sensor with diagonal direction).
 * @param point the point to check
 * @return true if this point is overlapping the jump sensor
 */
bool JumpSensor::is_point_in_diagonal(const SDL_Rect &point) {

  if (!is_point_in(get_position_in_map(), point.x, point.y)) {
    return false;
  }

  bool collision = false;
  int x = point.x - this->get_x();
  int y = point.y - this->get_y();
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
    DIE("Invalid direction of jump sensor");

  }

  return collision;
}

/**
 * This function is called when an entity overlaps the sensor.
 * If this entity is the hero, then we make him jump.
 * @param entity_overlapping the entity that overalps the sensor
 * @param collision_mode the collision mode that triggered the event
 * (not used here since a jump sensor has only one collision mode)
 */
void JumpSensor::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (entity_overlapping->is_hero()) {
    Hero* hero = (Hero*) entity_overlapping;
    if (hero->get_normal_movement()->is_moving_enabled()) {
      hero->start_jumping(direction, jump_length, false);
    }
  }
}
