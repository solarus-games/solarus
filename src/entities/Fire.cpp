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
#include "entities/Fire.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"

/**
 * @brief Creates some fire.
 * @param layer layer of the fire entity to create
 * @param xy coordinates of the fire
 */
Fire::Fire(Layer layer, const Rectangle& xy):
  Detector(COLLISION_RECTANGLE, "", layer, xy.get_x(), xy.get_y(), 16, 16) {

  // initialize the entity
  create_sprite("entities/fire");

  get_sprite().enable_pixel_collisions();
  set_origin(8, 13);
}

/**
 * @brief Destructor.
 */
Fire::~Fire() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Fire::get_type() {
  return FIRE;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * @return true if this type of entity can be obstacle for other entities
 */
bool Fire::can_be_obstacle() {
  return false; 
}

/**
 * @brief Returns whether entities of this type have detection capabilities.
 *
 * This function returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 *
 * @return true if this type of entity can detect other entities
 */
bool Fire::can_detect_entities() {
  return true;
}

/**
 * @brief Returns whether entities of this type can be displayed.
 *
 * If yes, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 *
 * @return true if this type of entity can be displayed
 */
bool Fire::can_be_displayed() {
  return true; 
}

/**
 * @brief Returns whether this entity has to be displayed in y order.
 *
 * This function returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 *
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Fire::is_displayed_in_y_order() {
  return false;
}

/**
 * @brief Updates this entity.
 */
void Fire::update() {

  Detector::update();
  if (get_sprite().is_animation_finished()) {
    remove_from_map();
  }

  check_collision_with_detectors();
}

/**
 * @brief Notifies this entity that a pixel-perfect collision was just detected with another sprite.
 *
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 *
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Fire::notify_collision(MapEntity& other_entity, Sprite& other_sprite, Sprite& this_sprite) {

  other_entity.notify_collision_with_fire(*this, other_sprite);
}

