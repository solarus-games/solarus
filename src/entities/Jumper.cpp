/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Jumper.h"
#include "entities/Hero.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"

namespace solarus {

/**
 * \brief Creates a jumper.
 * \param Name identifying this jumper
 * \param layer layer of the sensor on the map
 * \param x x coordinate of the top-left corner of the sensor's rectangle
 * \param y y coordinate of the top-left corner of the sensor's rectangle
 * \param width width of the sensor's rectangle
 * \param height height of the sensor's rectangle
 * \param direction direction of the jump (0 to 7 as the jump may be diagonal)
 * \param jump_length length of the jump in pixels (usually a multiple of 8)
 */
Jumper::Jumper(const std::string& name,
    Layer layer,
    int x,
    int y,
    int width,
    int height,
    int direction,
    int jump_length):
  Detector(COLLISION_CUSTOM | COLLISION_FACING, // Facing point detection is necessary to avoid sword tapping.
      name, layer, x, y, width, height),
  jump_length(jump_length) {

  set_direction(direction);

  // check the size
  if (direction % 2 != 0) {
    Debug::check_assertion(width == height, "This jumper has a diagonal direction but is not square");
  }
  else {
    if (direction % 4 == 0) {
      Debug::check_assertion(width == 8, "This jumper is horizontal but its height is not 8");
    }
    else {
      Debug::check_assertion(height == 8, "This jumper is vertical but its width is not 8");
    }
  }
  // check the jump length
  Debug::check_assertion(jump_length >= 16, "The jump length of this jumper is lower than 16");
}

/**
 * \brief Destructor.
 */
Jumper::~Jumper() {

}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Jumper::get_type() const {
  return ENTITY_JUMPER;
}

/**
 * \brief Returns whether entities of this type can be drawn.
 * \return true if this type of entity can be drawn
 */
bool Jumper::can_be_drawn() const {
  return false;
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other another entity
 * \return true if this entity is an obstacle for the other one
 */
bool Jumper::is_obstacle_for(MapEntity& other) {

  if (get_direction() % 2 != 0) {
    return false; // diagonal jumper: never obstacle (the tiles below the jumper should block entities)
  }

  // non-diagonal jumper: it depends on the entities (the tiles below the jumper should NOT block entities)
  return other.is_jumper_obstacle(*this);
}

/**
 * \brief Returns whether an entity is correctly placed to start jumping
 * with this jumper.
 * \param entity A map entity.
 * \return \c true if the entity is correctly placed to start the jump.
 */
bool Jumper::is_in_jump_position(const MapEntity& entity) const {

  const int direction8 = get_direction();

  if (is_jump_diagonal()) {
    // The sensor's shape is a diagonal bar.

    return is_point_in_diagonal(entity.get_facing_point((direction8 - 1) / 2))
      || is_point_in_diagonal(entity.get_facing_point((direction8 + 1) % 8 / 2));
  }

  // The sensor has one of the four main directions.
  // Its shape is exactly its rectangle.

  const int expected_direction4 = direction8 / 2;
  const Rectangle& facing_point = entity.get_facing_point(expected_direction4);
  const bool horizontal_jump = is_jump_horizontal();

  return overlaps(facing_point.get_x() + (horizontal_jump ? 0 : -8),
      facing_point.get_y() + (horizontal_jump ? -8 : 0))
    && overlaps(facing_point.get_x() + (horizontal_jump ? 0 : 7),
        facing_point.get_y() + (horizontal_jump ? 7 : 0));
}

/**
 * \brief Returns whether an entity's collides with this jumper.
 *
 * The result depends on the sensor's shape.
 *
 * \param entity the entity
 * \return true if the entity's collides with this jumper
 */
bool Jumper::test_collision_custom(MapEntity& entity) {

  if (!entity.is_hero()) {
    return false;
  }

  Hero& hero = static_cast<Hero&>(entity);
  const int direction8 = get_direction();

  if (!is_jump_diagonal()) {

    int expected_hero_direction4 = direction8 / 2;
    if (!hero.is_moving_towards(expected_hero_direction4)) {
      return false;
    }
  }

  return is_in_jump_position(hero);
}

/**
 * \brief Returns the length of the jump to make with this jumper.
 * \return Length of the jump in pixels (usually a multiple of 8).
 */
int Jumper::get_jump_length() const {
  return jump_length;
}

/**
 * \brief Returns whether this jumper makes horizontal jumps.
 * \return \c true if this jumper makes horizontal jumps.
 */
bool Jumper::is_jump_horizontal() const {

  return get_direction() == 0 || get_direction() == 4;
}

/**
 * \brief Returns whether this jumper makes vertical jumps.
 * \return \c true if this jumper makes vertical jumps.
 */
bool Jumper::is_jump_vertical() const {

  return get_direction() == 2 || get_direction() == 6;
}


/**
 * \brief Returns whether the direction of this jumper is diagonal.
 * \return \c true if this jumper is a diagonal one.
 */
bool Jumper::is_jump_diagonal() const {

  return get_direction() % 2 != 0;
}

/**
 * \brief Returns whether the specified point is in the jumper's shape.
 *
 * This function is used only for a jumper with diagonal direction.
 *
 * \param point the point to check
 * \return true if this point is overlapping the jumper
 */
bool Jumper::is_point_in_diagonal(const Rectangle& point) const {

  if (!overlaps(point.get_x(), point.get_y())) {
    return false;
  }

  bool collision = false;
  int x = point.get_x() - this->get_x();
  int y = point.get_y() - this->get_y();
  int width = get_width();

  switch (get_direction()) {

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
    Debug::die("Invalid direction of jumper");
  }

  return collision;
}

/**
 * \brief This function is called when an entity overlaps the jumper.
 * \param entity_overlapping The entity that overlaps the jumper.
 * \param collision_mode The collision mode that triggered the event.
 */
void Jumper::notify_collision(MapEntity& entity_overlapping,
    CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_jumper(*this, collision_mode);
}

/**
 * \brief Returns true if this entity does not react to the sword.
 *
 * If true is returned, nothing will happen when the hero hits this entity
 * with the sword.
 *
 * \return \c true if the sword is ignored
 */
bool Jumper::is_sword_ignored() const {

  // We don't want a sword tapping animation.
  return true;
}

}

