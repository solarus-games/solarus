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
#include "entities/Explosion.h"
#include "entities/MapEntities.h"
#include "Map.h"
#include "ResourceManager.h"
#include "Sound.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"

/**
 * Creates an explosion.
 * @param xy coordinates of the center of the explosion
 */
Explosion::Explosion(Layer layer, const SDL_Rect &xy):
  Detector(COLLISION_SPRITE, "", layer, xy.x, xy.y, 48, 48) { 

  // initialize the entity
  create_sprite("entities/explosion");
  get_sprite()->get_animation_set()->enable_pixel_collisions();
  set_rectangle_from_sprite();
  ResourceManager::get_sound("explosion")->play();
}

/**
 * Destructor.
 */
Explosion::~Explosion(void) {

}

/**
 * Returns the type of entity.
 * @return the type of entity
 */
EntityType Explosion::get_type() {
  return BOOMERANG;
}

/**
 * Returns whether entities of this type can be obstacles for other entities.
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 * @return true if this type of entity can be obstacle for other entities
 */
bool Explosion::can_be_obstacle(void) {
  return false; 
}

/**
 * Returns whether entities of this type can detect the presence 
 * of the hero or other entities (this is possible only for
 * suclasses of Detector). If yes, the function 
 * notify_collision() will be called when a collision is detected.
 * @return true if this type of entity can detect other entities
 */
bool Explosion::can_detect_entities(void) {
  return true;
}

/**
 * Returns whether entities of this type can be displayed.
 * If enabled, the sprites added by the add_sprite() calls will be 
 * displayed (if any).
 * @return true if this type of entity can be displayed
 */
bool Explosion::can_be_displayed(void) {
  return true; 
}

/**
 * Returns whether an entity of this type should be displayed above
 * the hero and other entities having this property when it is in front of them.
 * This means that the displaying order of entities having this
 * feature depends on their y position. The entities without this feature
 * are displayed in the normal order (i.e. as specified by the map file), 
 * and before the entities with the feature.
 * @return true if this type of entity is displayed at the same level as the hero
 */
bool Explosion::is_displayed_in_y_order(void) {
  return false;
}

/**
 * Updates this entity.
 */
void Explosion::update(void) {
  Detector::update();
  if (get_sprite()->is_animation_finished()) {
    map->get_entities()->remove_entity(this);
  }
}

