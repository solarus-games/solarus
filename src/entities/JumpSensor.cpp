#include "entities/JumpSensor.h"
#include "entities/Link.h"

/**
 * Creates a jump sensor.
 * @param name a name identifying this jump sensor
 * @param layer layer of the sensor on the map
 * @param x x coordinate of the top-left corner of the sensor's rectangle
 * @param y y coordinate of the top-left corner of the sensor's rectangle
 * @param width width of the sensor's rectangle
 * @param height height of the sensor's rectangle
 * @param direction direction of the jump (0 to 7 as the jump may be diagonal)
 * @param length length of the jump in pixels (usually a multiple of 8)
 */
JumpSensor::JumpSensor(string name, Layer layer, int x, int y, int width, int height,
		       int direction, int jump_length):
  Detector(COLLISION_FACING_POINT, name, layer, x, y, width, height),
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

}

/**
 * Destructor.
 */
JumpSensor::~JumpSensor(void) {

}

/**
 * Checks whether an entity's collides with this jump sensor.
 * The test depends on the sensor's shape.
 * @param entity the entity
 * @return true if the entity's collides with this jump sensor
 */
bool JumpSensor::check_collision_custom(MapEntity *entity) {

  // first, the facing point has to be inside the sensor's rectangle
  if (!check_collision_facing_point(entity)) {
    return false;
  }

  // if the sensor's has one of the four main directions, then
  // its shape is exactly its rectangle so we have nothing else to do
  if (direction % 2 == 0) {
    return true;
  }

  // otherwise, the sensor's shape is a diagonal bar
  bool collision = false;
  int x = entity->get_facing_point().x - this->get_x();
  int y = entity->get_facing_point().y - this->get_y();
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
    Link* link = (Link*) entity_overlapping;
    link->start_jumping(direction, jump_length);
  }
}
