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
#include "entities/Detector.h"
#include "entities/Hero.h"
#include "ResourceManager.h"
#include "Map.h"
#include "MapScript.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "SpriteAnimationSet.h"

/**
 * Constructor.
 * @param collision_modes the detector's collision mode(s)
 * @param name a name identifying the detector (or an empty string if you
 * don't need to identify the detector)
 * @param layer layer of the detector
 * @param x x position of the detector's rectangle
 * @param y y position of the detector's rectangle
 * @param width width of the detector's rectangle
 * @param height height of the detector's rectangle
 */
Detector::Detector(int collision_modes,
		   const std::string &name, Layer layer,
		   int x, int y, int width, int height):
  MapEntity(name, 0, layer, x, y, width, height),
  collision_modes(collision_modes), layer_ignored(false) {

}

/**
 * Destructor.
 */
Detector::~Detector(void) {

}

/**
 * Sets the collision modes of this detector.
 * @param collision_modes the detector's collision modes
 * (can be an OR combination of collision modes)
 */
void Detector::set_collision_modes(int collision_modes) {

  if (collision_modes & COLLISION_SPRITE) {

    for (unsigned int i = 0; i < sprites.size(); i++) {
      get_sprite(i)->get_animation_set()->enable_pixel_collisions();
    }
  }
  this->collision_modes = collision_modes;
}

/**
 * Adds a collision mode to this detector.
 * @param collision_mode the collision mode to add
 */
void Detector::add_collision_mode(CollisionMode collision_mode) {

  set_collision_modes(this->collision_modes | collision_mode);
}

/**
 * Returns whether the detector's collision modes includes
 * the specified collision mode.
 * @param collision_mode a collision mode
 */
bool Detector::has_collision_mode(CollisionMode collision_mode) {
  return (this->collision_modes & collision_mode) != 0;
}

/**
 * Sets whether the detector can detect entities even if
 * they are not on the same layer.
 * @param layer_ignored true to detect all entities, false
 * to detect only those on the same layer.
 */
void Detector::set_layer_ignored(bool layer_ignored) {
  this->layer_ignored = layer_ignored;
}

/**
 * This function is called by the map when an entity has just moved.
 * It checks whether the entity collides with this detector.
 * Depending on the detector collision mode(s), the appropriate check_collision_*
 * functions are called.
 * If there is a collision, the collision(MapEntity*) method is called.
 * @param entity the entity to check
 */
void Detector::check_collision(MapEntity *entity) {

  if (entity != this
      && (layer_ignored || get_layer() == entity->get_layer())) { // the entity is in the same layer as the detector

    // detect the collision depending on the collision mode

    if (has_collision_mode(COLLISION_RECTANGLE) && check_collision_rectangle(entity)) {
      collision(entity, COLLISION_RECTANGLE);
    }

    if (has_collision_mode(COLLISION_ORIGIN_POINT) && check_collision_origin_point(entity)) {
      collision(entity, COLLISION_ORIGIN_POINT);
    }

    if (has_collision_mode(COLLISION_FACING_POINT) && check_collision_facing_point(entity)) {
      entity->set_facing_entity(this);
      collision(entity, COLLISION_FACING_POINT);
    }

    if (has_collision_mode(COLLISION_CENTER) && check_collision_center(entity)) {
      collision(entity, COLLISION_CENTER);
    }

    if (has_collision_mode(COLLISION_CUSTOM) && check_collision_custom(entity)) {
      collision(entity, COLLISION_CUSTOM);
    }
  }
}

/**
 * This function whether a sprite collides with this detector.
 * If there is a collision, the collision(MapEntity*, Sprite*) method is called.
 * @param entity the entity to check
 * @param sprite the sprite of that entity
 */
void Detector::check_collision(MapEntity *entity, Sprite *sprite) {

  if (has_collision_mode(COLLISION_SPRITE)
      && entity != this
      && (layer_ignored || get_layer() == entity->get_layer())
      && sprite->get_animation_set()->are_pixel_collisions_enabled()
      && get_sprite(0)->check_collision(sprite, get_x(), get_y(), entity->get_x(), entity->get_y())) {

    // TODO allow a detector to have several pixel collision capable sprites (e.g. green soldier)

    collision(entity, sprite);
  }
}

/**
 * Checks whether an entity's rectangle is overlapping the detector's rectangle.
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_RECTANGLE.
 * @param entity the entity
 * @return true if the entity's rectangle is overlapping the detector's rectangle
 */
bool Detector::check_collision_rectangle(MapEntity *entity) {

  return entity->overlaps(get_position_in_map());
}

/**
 * Checks whether an entity's origin point is overlapping the detector's rectangle.
 * This method is called by check_entity_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_ORIGIN_POINT.
 * @param entity the entity
 * @return true if the entity's origin point is overlapping the detector's rectangle
 */
bool Detector::check_collision_origin_point(MapEntity *entity) {

  return entity->is_origin_point_in(get_position_in_map());
}

/**
 * Checks whether an entity's facing point is overlapping the detector's rectangle.
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_FACING_POINT.
 * @param entity the entity
 * @return true if the entity's facing point is overlapping the detector's rectangle
 */
bool Detector::check_collision_facing_point(MapEntity *entity) {

  return entity->is_facing_point_in(get_position_in_map());
}

/**
 * Checks whether an entity's center is overlapping the detector's rectangle.
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_CENTER.
 * @param entity the entity
 * @return true if the entity's center is overlapping the detector's rectangle
 */
bool Detector::check_collision_center(MapEntity *entity) {

  return entity->is_center_in(get_position_in_map());
}

/**
 * Checks whether an entity's collides with this detector with respect to a custom rule.
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_CUSTOM.
 * Redefine it if you want to use this collision mode.
 * @param entity the entity
 * @return true if the entity's collides with this detector with respect to the custom rule
 */
bool Detector::check_collision_custom(MapEntity *entity) {

  DIE("Custom collision mode invoked but not defined");
}

/**
 * This function is called by check_collision(MapEntity*)
 * when an entity overlaps the detector.
 * By default, nothing is done.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision (useful if
 * the detector has several collision modes)
 */
void Detector::collision(MapEntity *entity_overlapping, CollisionMode collision_mode) {

}

/**
 * This function is called by check_collision(MapEntity*, Sprite*)
 * when a sprite overlaps the detector.
 * By default, nothing happens.
 * Redefine this method in the subclasses to do the appropriate action.
 * @param entity the entity
 * @param sprite_overlapping the sprite of this entity overlapping the detector
 */
void Detector::collision(MapEntity *entity, Sprite *sprite_overlapping) {

}

/**
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * By default, nothing happens.
 * Redefine your function in the subclasses to allow the hero to interact with this entity.
 */
void Detector::action_key_pressed(void) {

}

/**
 * This function is called when the player tries to push or pull this detector.
 * By default, nothing happens.
 * Redefine your function in the subclasses to make something happen with the entity.
 * @return true if the detector was pushed or pulled successfully
 */
bool Detector::moved_by_hero(void) {
  return false;
}

/**
 * When the sword sprite collides with this detector,
 * this function can be called to determine whether the sword is really hitting the detector.
 * This depends on the hero state and its distance to the detector.
 * This function assumes that there is already a collision between the sword sprite and the detector's sprite.
 * @param hero the hero
 * @return true if the sword is hitting this detector
 */
bool Detector::is_hit_by_sword(Hero *hero) {

  bool hit = false;
  Hero::State state = hero->get_state();
  int animation_direction = hero->get_animation_direction();

  if (state == Hero::SPIN_ATTACK) {
    hit = true;
  }
  else if (state == Hero::SWORD_SWINGING
	   || (is_obstacle_for(hero) && hero->is_moving_towards(animation_direction))) {

    SDL_Rect facing_point = hero->get_facing_point();

    int distance = is_obstacle_for(hero) ? 14 : 4;

    switch (animation_direction) {

    case 0:
      hit = facing_point.y >= position_in_map.y
	&& facing_point.y < position_in_map.y + position_in_map.h
	&& facing_point.x >= position_in_map.x - distance
	&& facing_point.x < position_in_map.x + position_in_map.w - distance;
      break;

    case 1:
      hit = facing_point.x >= position_in_map.x
	&& facing_point.x < position_in_map.x + position_in_map.w
	&& facing_point.y >= position_in_map.y + distance
	&& facing_point.y < position_in_map.y + position_in_map.h + distance;
      break;

    case 2:
      hit = facing_point.y >= position_in_map.y
	&& facing_point.y < position_in_map.y + position_in_map.h
	&& facing_point.x >= position_in_map.x + distance
	&& facing_point.x < position_in_map.x + position_in_map.w + distance;
      break;

    case 3:
      hit = facing_point.x >= position_in_map.x
	&& facing_point.x < position_in_map.x + position_in_map.w
	&& facing_point.y >= position_in_map.y - distance
	&& facing_point.y < position_in_map.y + position_in_map.h - distance;
      break;

    default:
      DIE("Invalid animation direction of the hero: " << hero->get_animation_direction());
      break;
    }
  }
  return hit;
}
