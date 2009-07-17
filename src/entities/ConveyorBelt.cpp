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
#include "entities/ConveyorBelt.h"
#include "Map.h"
#include "Sprite.h"

/**
 * Creates a new conveyor belt.
 * @param layer layer of the entity to create on the map
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 * @param width width of the conveyor belt (the pattern can be repeated)
 * @param height height of the conveyor (the pattern can be repeated)
 */
ConveyorBelt::ConveyorBelt(Layer layer, int x, int y, int width, int height, int direction):
  Detector(COLLISION_CUSTOM, "", layer, x, y, width, height), enabled(true) {

  create_sprite("entities/conveyor_belt");
  get_sprite()->set_current_direction(direction);
}

/**
 * Destructor.
 */
ConveyorBelt::~ConveyorBelt(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType ConveyorBelt::get_type() {
  return CONVEYOR_BELT;
}

/**
 * Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true if this block is raised
 */
bool ConveyorBelt::is_obstacle_for(MapEntity *other) {
  return other->is_conveyor_belt_obstacle(this);
}

/**
 * Returns whether this conveyor belt is enabled.
 * @return true if it is enabled
 */
bool ConveyorBelt::is_enabled(void) {
  return enabled;
}

/**
 * Sets whether this conveyor belt is enabled.
 * @param enable true to enable it, false to disable it
 */
void ConveyorBelt::set_enabled(bool enabled) {
  this->enabled = enabled;
}

/**
 * Checks whether an entity's collides with this entity.
 * @param entity an entity
 * @return true if the entity's collides with this entity
 */
bool ConveyorBelt::check_collision_custom(MapEntity *entity) {

  const SDL_Rect &entity_position = entity->get_position_in_map();
  int x1 = entity_position.x + 4;
  int x2 = x1 + entity_position.w - 8;
  int y1 = entity_position.y + 4;
  int y2 = y1 + entity_position.h - 8;

  return overlaps(x1, y1) && overlaps(x2, y1) &&
    overlaps(x1, y2) && overlaps(x2, y2);
}

/**
 * This function is called when another entity overlaps this entity.
 * @param entity_overlapping the other entity
 * @param collision_mode the collision mode that detected the collision
 */
void ConveyorBelt::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

  if (is_enabled()) {
    entity_overlapping->collision_with_conveyor_belt(this);
  }
}

/**
 * Updates this entity.
 */
void ConveyorBelt::update(void) {
  
}

/**
 * Displays the entity on the map.
 * This is a redefinition of MapEntity::display_on_map to repeat the pattern.
 */
void ConveyorBelt::display_on_map(void) {

  Sprite *sprite = get_sprite();

  int x1 = get_top_left_x();
  int y1 = get_top_left_y();
  int x2 = x1 + get_width();
  int y2 = y1 + get_height();

  for (int y = y1; y < y2; y += 16) {
    for (int x = x1; x < x2; x += 16) {
      map->display_sprite(sprite, x, y);
    }
  }
}
