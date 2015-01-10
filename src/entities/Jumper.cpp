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
#include "solarus/entities/Jumper.h"
#include "solarus/entities/Hero.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

/**
 * \brief Creates a jumper.
 * \param name Name identifying this entity.
 * \param layer layer of the entity on the map
 * \param x x coordinate of the top-left corner of the entity's rectangle
 * \param y y coordinate of the top-left corner of the entity's rectangle
 * \param width width of the entity's rectangle
 * \param height height of the entity's rectangle
 * \param direction direction of the jump (0 to 7 as the jump may be diagonal)
 * \param jump_length length of the jump in pixels (usually a multiple of 8)
 */
Jumper::Jumper(const std::string& name,
    Layer layer,
    const Point& xy,
    const Size& size,
    int direction,
    int jump_length):
  Detector(COLLISION_CUSTOM | COLLISION_FACING, // Facing point detection is necessary to avoid sword tapping.
      name, layer, xy, size),
  jump_length(jump_length) {

  set_direction(direction);

  // check the size
  if (direction % 2 != 0) {
    Debug::check_assertion(size.is_square(), "This jumper has a diagonal direction but is not square");
  }
  else {
    if (direction % 4 == 0) {
      Debug::check_assertion(size.width == 8, "This jumper is horizontal but its height is not 8");
    }
    else {
      Debug::check_assertion(size.height == 8, "This jumper is vertical but its width is not 8");
    }
  }
  // check the jump length
  Debug::check_assertion(jump_length >= 16, "The jump length of this jumper is lower than 16");
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Jumper::get_type() const {
  return EntityType::JUMPER;
}

/**
 * \brief Returns whether entities of this type can be drawn.
 * \return true if this type of entity can be drawn
 */
bool Jumper::can_be_drawn() const {
  return false;
}

/**
 * \copydoc MapEntity::is_obstacle_for(MapEntity&, const Rectangle&)
 */
bool Jumper::is_obstacle_for(
    MapEntity& other, const Rectangle& candidate_position) {

  return other.is_jumper_obstacle(*this, candidate_position);
}

/**
 * \brief Returns whether the hero is correctly placed to
 * start jumping with this jumper.
 * \param hero The hero.
 * \param candidate_position The candidate bounding box.
 * \param extended_region Whether you want to consider the extended jumper
 * beyond its bounding box, or to keep it restricted to its bounding box.
 * \return \c true if the hero is correctly placed to start a jump.
 */
bool Jumper::is_in_jump_position(
    const Hero& hero,
    const Rectangle& candidate_position,
    bool extended_region) const {

  if (overlaps_jumping_region(candidate_position, extended_region)) {
    // Overlapping the active region: cannot start a jump from there.
    return false;
  }

  const int direction8 = get_direction();
  const int expected_hero_direction4 = direction8 / 2;

  if (is_jump_diagonal()) {
    // Diagonal case: The sensor's shape is a diagonal bar.

    // The player should move toward one of both directions.
    if (!hero.is_moving_towards(expected_hero_direction4) &&
        !hero.is_moving_towards((expected_hero_direction4 + 1) % 4)) {
      return false;
    }

    // Test if the appropriate corner of the hero crosses the diagonal.
    Point corner = {
        candidate_position.get_x() - 1,
        candidate_position.get_y() - 1
    };
    if (direction8 == 1 || direction8 == 7) {
      // Right-up or right-down.
      corner.x += candidate_position.get_width() + 1;
    }
    if (direction8 == 5 || direction8 == 7) {
      // Left-down or right-down.
      corner.y += candidate_position.get_height() + 1;
    }

    return extended_region ?
        is_point_in_extended_diagonal(corner) :
        is_point_in_diagonal(corner);
  }

  // Non-diagonal case: the sensor has one of the four main directions.
  // Its shape is exactly its rectangle.

  // The player should move toward the jumper's direction.
  if (!hero.is_moving_towards(expected_hero_direction4)) {
    return false;
  }

  Point facing_point;

  switch (expected_hero_direction4) {

    // right
    case 0:
      facing_point = {
          candidate_position.get_x() + 16,
          candidate_position.get_y() + 8
      };
      break;

      // up
    case 1:
      facing_point = {
          candidate_position.get_x() + 8,
          candidate_position.get_y() - 1
      };
      break;

      // left
    case 2:
      facing_point = {
          candidate_position.get_x() - 1,
          candidate_position.get_y() + 8
      };
      break;

      // down
    case 3:
      facing_point = {
          candidate_position.get_x() + 8,
          candidate_position.get_y() + 16
      };
      break;

    default:
      Debug::die("Invalid direction");
  }

  if (is_jump_horizontal()) {
    if (extended_region) {
      // Are we inside the extended strip?
      return facing_point.x >= get_top_left_x() &&
          facing_point.x < get_top_left_x() + get_width();
    }
    else {
      // Are we inside the strip and the bounding box?
      return overlaps(facing_point.x, facing_point.y - 8)
          && overlaps(facing_point.x, facing_point.y + 7);
    }
  }
  else {
    // Same thing for a vertical jump.
    if (extended_region) {
      // Are we inside the extended strip?
      return facing_point.y >= get_top_left_y() &&
          facing_point.y < get_top_left_y() + get_height();
    }
    else {
      return overlaps(facing_point.x - 8, facing_point.y) &&
          overlaps(facing_point.x + 7, facing_point.y);
    }
  }
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

  return is_in_jump_position(
      static_cast<Hero&>(entity), entity.get_bounding_box(), false
  );
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
 * \brief Returns whether a point is in the 8-pixel active strip
 * of this diagonal jumper.
 *
 * This function only make sense for a diagonal jumper.
 *
 * \param point The point to check.
 * \return \c true if this point is overlapping the jumper
 */
bool Jumper::is_point_in_diagonal(const Point& point) const {

  return overlaps(point) &&
    is_point_in_extended_diagonal(point);
}

/**
 * \brief Returns whether a point is in the 8-pixel active strip
 * of this diagonal jumper or in the extension of that strip.
 *
 * This function only make sense for a diagonal jumper.
 *
 * \param point The point to check.
 * \return \c true if this point is overlapping the jumper
 */
bool Jumper::is_point_in_extended_diagonal(const Point& point) const {

  const int x = point.x - this->get_x();
  const int y = point.y - this->get_y();
  const int width = get_width();

  switch (get_direction()) {

  case 1:
    return (x >= y) && (x < y + 8);

  case 3:
    return (x + y <= width) && (x + y > width - 8);

  case 5:
    return (y >= x) && (y < x + 8);

  case 7:
    return (x + y >= width) && (x + y < width + 8);

  default:
    Debug::die("Invalid direction of jumper");
  }

  return false;
}

/**
 * \brief Returns whether a rectangle overlaps the active region of the jumper.
 *
 * For a horizontal or vertical jumper, the active region is the whole
 * bounding box, that is, an 8-pixel thick horizontal or vertical strip.
 * For a diagonal jumper, the active region is an 8-pixel thick diagonal strip.
 *
 * \param rectangle A rectangle.
 * \param extended_region Whether you want to consider the extended jumper
 * beyond its bounding box, or to keep it restricted to its bounding box.
 * \return \c true if the rectangle overlaps the active region of the jumper.
 */
bool Jumper::overlaps_jumping_region(const Rectangle& rectangle, bool /* extended_region */) const {

  if (!is_jump_diagonal()) {
    return overlaps(rectangle);
  }

  // Check the 4 corners of the rectangle.
  Point xy = rectangle.get_xy();
  if (is_point_in_diagonal(xy)) {
    return true;
  }

  xy.x += rectangle.get_width() - 1;
  if (is_point_in_diagonal(xy)) {
    return true;
  }

  xy.y += rectangle.get_height() - 1;
  if (is_point_in_diagonal(xy)) {
    return true;
  }

  xy.x = rectangle.get_x();
  if (is_point_in_diagonal(xy)) {
    return true;
  }

  // Check the two ends of the diagonal.
  if (get_direction() == 1 || get_direction() == 5) {
    if (rectangle.contains(get_top_left_x(), get_top_left_y())) {
      return true;
    }
    if (rectangle.contains(
        get_top_left_x() + get_width() - 1,
        get_top_left_y() + get_height() - 1)) {
      return true;
    }
  }
  else {
    if (rectangle.contains(
        get_top_left_x() + get_width() - 1, get_top_left_y())) {
      return true;
    }
    if (rectangle.contains(
        get_top_left_x(), get_top_left_y() + get_height() - 1)) {
      return true;
    }
  }

  return false;
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

