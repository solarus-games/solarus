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
#include "entities/Detector.h"
#include "entities/Hero.h"
#include "Map.h"
#include "KeysEffect.h"
#include "Sprite.h"
#include "lowlevel/Debug.h"

/**
 * @brief Constructor.
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
  collision_modes(collision_modes),
  layer_independent_collisions(false) {

}

/**
 * @brief Destructor.
 */
Detector::~Detector() {

}

/**
 * @brief Sets the collision modes of this detector.
 * @param collision_modes the detector's collision modes
 * (can be an OR combination of collision modes)
 */
void Detector::set_collision_modes(int collision_modes) {

  if (collision_modes & COLLISION_SPRITE) {
    enable_pixel_collisions();
  }
  this->collision_modes = collision_modes;
}

/**
 * @brief Adds a collision mode to this detector.
 * @param collision_mode the collision mode to add
 */
void Detector::add_collision_mode(CollisionMode collision_mode) {

  set_collision_modes(this->collision_modes | collision_mode);
}

/**
 * @brief Returns whether the detector's collision modes includes
 * the specified collision mode.
 * @param collision_mode a collision mode
 * @return true if this collision mode is detected by this detector
 */
bool Detector::has_collision_mode(CollisionMode collision_mode) {
  return (this->collision_modes & collision_mode) != 0;
}

/**
 * @brief Enables the pixel-perfect collision checks for all sprites
 * of this detector.
 */
void Detector::enable_pixel_collisions() {

  std::list<Sprite*>::iterator it;
  for (it = get_sprites().begin(); it != get_sprites().end(); it++) {
    (*it)->enable_pixel_collisions();
  }
}

/**
 * @brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @return true if this entity can collide with entities that are on another layer
 */
bool Detector::has_layer_independent_collisions() {
  return layer_independent_collisions;
}

/**
 * @brief Sets whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * @param independent true if this entity can collide with entities that are on another layer
 */
void Detector::set_layer_independent_collisions(bool independent) {
  this->layer_independent_collisions = independent;
}

/**
 * @brief Checks whether an entity collides with this detector.
 *
 * This function is called by the map when an entity has just moved.
 * It checks whether the entity collides with this detector.
 * Depending on the detector collision mode(s), the appropriate has_collision_*
 * functions are called.
 * If there is a collision, the notify_collision() method is called.
 *
 * @param entity the entity to check
 */
void Detector::check_collision(MapEntity &entity) {

  if (&entity != this
      && (has_layer_independent_collisions() || get_layer() == entity.get_layer())) { // the entity is in the same layer as the detector

    // detect the collision depending on the collision mode

    if (has_collision_mode(COLLISION_RECTANGLE) && test_collision_rectangle(entity)) {
      notify_collision(entity, COLLISION_RECTANGLE);
    }

    if (has_collision_mode(COLLISION_INSIDE) && test_collision_inside(entity)) {
      notify_collision(entity, COLLISION_INSIDE);
    }

    if (has_collision_mode(COLLISION_ORIGIN_POINT) && test_collision_origin_point(entity)) {
      notify_collision(entity, COLLISION_ORIGIN_POINT);
    }

    if (has_collision_mode(COLLISION_FACING_POINT) && test_collision_facing_point(entity)) {

      if (entity.get_facing_entity() == NULL) { // make sure only one entity can think "I am the facing entity"
        entity.set_facing_entity(this);
      }
      notify_collision(entity, COLLISION_FACING_POINT);
    }

    if (has_collision_mode(COLLISION_FACING_POINT_ANY) && test_collision_facing_point_any(entity)) {
      notify_collision(entity, COLLISION_FACING_POINT_ANY);
    }

    if (has_collision_mode(COLLISION_CENTER) && test_collision_center(entity)) {
      notify_collision(entity, COLLISION_CENTER);
    }

    if (has_collision_mode(COLLISION_CUSTOM) && test_collision_custom(entity)) {
      notify_collision(entity, COLLISION_CUSTOM);
    }
  }
}

/**
 * @brief Checks whether a sprite collides with this detector.
 *
 * If there is a collision, the notify_collision(MapEntity&, Sprite&, Sprite&) method is called.
 *
 * @param entity the entity to check
 * @param sprite the sprite of that entity
 */
void Detector::check_collision(MapEntity &entity, Sprite &sprite) {

  if (has_collision_mode(COLLISION_SPRITE)
      && &entity != this
      && (has_layer_independent_collisions() || get_layer() == entity.get_layer())) {

    // we check the collision between the specified entity's sprite and all sprites of the current entity
    std::list<Sprite*>::iterator it;
    for (it = get_sprites().begin(); it != get_sprites().end(); it++) {
      Sprite &this_sprite = *(*it);

      if (this_sprite.test_collision(sprite, get_x(), get_y(), entity.get_x(), entity.get_y())) {
        notify_collision(entity, sprite, this_sprite);
      }
    }
  }
}

/**
 * @brief Returns whether an entity's rectangle is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_RECTANGLE.
 *
 * @param entity the entity
 * @return true if the entity's rectangle is overlapping the detector's rectangle
 */
bool Detector::test_collision_rectangle(MapEntity &entity) {

  return entity.overlaps(*this);
}

/**
 * @brief Returns whether an entity's rectangle is entirely inside the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_INSIDE.
 *
 * @param entity the entity
 * @return true if the entity's rectangle is entirely inside the detector's rectangle
 */
bool Detector::test_collision_inside(MapEntity &entity) {

  return get_bounding_box().contains(entity.get_bounding_box());
}


/**
 * @brief Returns whether the origin point of an entity is overlapping the detector's rectangle.
 *
 * This method is called by check_entity_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_ORIGIN_POINT.
 *
 * @param entity the entity
 * @return true if the entity's origin point is overlapping the detector's rectangle
 */
bool Detector::test_collision_origin_point(MapEntity &entity) {

  return entity.is_origin_point_in(get_bounding_box());
}

/**
 * @brief Returns whether the facing point of an entity is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_FACING_POINT.
 *
 * @param entity the entity
 * @return true if the entity's facing point is overlapping the detector's rectangle
 */
bool Detector::test_collision_facing_point(MapEntity &entity) {

  return entity.is_facing_point_in(get_bounding_box());
}

/**
 * @brief Returns whether the facing point of an entity (in any of the four main directions)
 * is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_FACING_POINT_ANY.
 *
 * @param entity the entity
 * @return true if a facing point of the entity is overlapping the detector's rectangle
 */
bool Detector::test_collision_facing_point_any(MapEntity& entity) {

  const Rectangle& bounding_box = get_bounding_box();
  return entity.is_facing_point_in(bounding_box, 0)
      || entity.is_facing_point_in(bounding_box, 1)
      || entity.is_facing_point_in(bounding_box, 2)
      || entity.is_facing_point_in(bounding_box, 3);
}

/**
 * @brief Returns whether the center point of an entity is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_CENTER.
 *
 * @param entity the entity
 * @return true if the entity's center is overlapping the detector's rectangle
 */
bool Detector::test_collision_center(MapEntity &entity) {

  return entity.is_center_in(get_bounding_box());
}

/**
 * @brief Returns whether an entity collides with this detector with respect to a custom rule.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_CUSTOM.
 * Redefine it if you want to use this collision mode.
 *
 * @param entity the entity
 * @return true if the entity's collides with this detector with respect to the custom rule
 */
bool Detector::test_collision_custom(MapEntity &entity) {

  Debug::die("Custom collision mode invoked but not defined");
  return false;
}

/**
 * @brief Notifies this detector that a collision was just detected with an entity.
 *
 * This function is called by check_collision(MapEntity*)
 * when an entity overlaps the detector.
 * By default, nothing is done.
 *
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision (useful if
 * the detector has several collision modes)
 */
void Detector::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

}

/**
 * @brief Notifies this detector that a pixel-perfect collision was just detected with another sprite.
 *
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 * By default, nothing happens. Redefine this method in the subclasses to do the appropriate action.
 *
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Detector::notify_collision(MapEntity &other_entity, Sprite &other_sprite, Sprite &this_sprite) {

}

/**
 * @brief Notifies this detector that the player is interacting with it by pressing the action key.
 *
 * This function is called when the player presses the action key
 * while the hero is facing this detector, and the action icon lets him do this.
 * By default, nothing happens.
 * Redefine your function in the subclasses to allow the hero to interact with this entity.
 */
void Detector::action_key_pressed() {

}

/**
 * @brief Notifies this detector that the player is interacting with it by using an inventory item.
 * 
 * This function is called when the player uses an inventory item
 * while the hero is facing this detector.
 * The exact conditions where this function is called depend on the type of inventory item.
 * For some items, the function is called as soon as the player uses the item.
 * For others, it is called after the player confirms the action in a dialog box.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make this entity react to an inventory item.
 *
 * @param item the inventory item used
 * @return true if an interaction occured
 */
bool Detector::interaction_with_inventory_item(InventoryItem &item) {
  return false;
}

/**
 * @brief This function is called when the player tries to push or pull this detector.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make something happen with the entity.
 *
 * @return true if the detector was pushed or pulled successfully
 */
bool Detector::moved_by_hero() {
  return false;
}

/**
 * @brief This function is called when the player is tapping his sword against this detector.
 * @return the sound to play when tapping this detector with the sword
 */
SoundId Detector::get_sword_tapping_sound() {
  return "sword_tapping";
}

/**
 * @brief Notifies this entity that it is about to be removed.
 *
 * This function is called when the entity has just been added
 * to the list of entities that will be removed from the map
 * and deleted from the memory as soon as possible.
 */
void Detector::notify_being_removed() {

  MapEntity::notify_being_removed();

  if (get_hero().get_facing_entity() == this) {
    get_hero().set_facing_entity(NULL);
  }
}

