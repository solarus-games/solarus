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
#include "solarus/entities/Detector.h"
#include "solarus/entities/Hero.h"
#include "solarus/Map.h"
#include "solarus/KeysEffect.h"
#include "solarus/Sprite.h"
#include "solarus/lowlevel/Debug.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param collision_modes the detector's collision mode(s)
 * \param name Name identifying the entity on the map or an empty string.
 * \param layer layer of the detector
 * \param xy Coordinates where to create the detector.
 * \param size Size of the detector's rectangle.
 */
Detector::Detector(
    int collision_modes,
    const std::string& name,
    Layer layer,
    const Point& xy,
    const Size& size
):
  MapEntity(name, 0, layer, xy, size),
  collision_modes(collision_modes),
  layer_independent_collisions(false) {

}

/**
 * \brief Destructor.
 */
Detector::~Detector() {

}

/**
 * \brief Returns whether entities of this type have detection capabilities.
 *
 * This function returns whether entities of this type can detect the presence
 * of the hero or other entities. If yes, the function
 * notify_collision() will be called when a collision is detected.
 *
 * \return \c true if this type of entity can detect other entities.
 */
bool Detector::is_detector() const {
  return true;
}

/**
 * \brief Sets the collision modes of this detector.
 * \param collision_modes the detector's collision modes
 * (can be an OR combination of collision modes)
 */
void Detector::set_collision_modes(int collision_modes) {

  if (collision_modes & COLLISION_SPRITE) {
    enable_pixel_collisions();
  }
  this->collision_modes = collision_modes;
}

/**
 * \brief Adds a collision mode to this detector.
 * \param collision_mode the collision mode to add
 */
void Detector::add_collision_mode(CollisionMode collision_mode) {

  set_collision_modes(this->collision_modes | collision_mode);
}

/**
 * \brief Returns whether the detector's collision modes includes
 * the specified collision mode.
 * \param collision_mode a collision mode
 * \return true if this collision mode is detected by this detector
 */
bool Detector::has_collision_mode(CollisionMode collision_mode) {
  return (this->collision_modes & collision_mode) != 0;
}

/**
 * \brief Enables the pixel-perfect collision checks for all sprites
 * of this detector.
 */
void Detector::enable_pixel_collisions() {

  for (const SpritePtr& sprite: get_sprites()) {
    sprite->enable_pixel_collisions();
  }
}

/**
 * \brief Returns whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * \return true if this entity can collide with entities that are on another layer
 */
bool Detector::has_layer_independent_collisions() const {
  return layer_independent_collisions;
}

/**
 * \brief Sets whether this entity can have collisions with entities even if
 * they are not on the same layer.
 * \param independent true if this entity can collide with entities that are on another layer
 */
void Detector::set_layer_independent_collisions(bool independent) {
  this->layer_independent_collisions = independent;
}

/**
 * \brief Checks whether an entity collides with this detector.
 *
 * This function is called by the map when an entity has just moved.
 * It checks whether the entity collides with this detector.
 * Depending on the detector collision mode(s), the appropriate
 * test_collision_* functions are called.
 * If there is a collision, the notify_collision() method is called.
 *
 * \param entity the entity to check
 */
void Detector::check_collision(MapEntity& entity) {

  if (&entity != this
      && (has_layer_independent_collisions() || get_layer() == entity.get_layer())) { // the entity is in the same layer as the detector

    // detect the collision depending on the collision mode

    if (has_collision_mode(COLLISION_OVERLAPPING) && test_collision_rectangle(entity)) {
      notify_collision(entity, COLLISION_OVERLAPPING);
    }

    if (has_collision_mode(COLLISION_CONTAINING) && test_collision_inside(entity)) {
      notify_collision(entity, COLLISION_CONTAINING);
    }

    if (has_collision_mode(COLLISION_ORIGIN) && test_collision_origin_point(entity)) {
      notify_collision(entity, COLLISION_ORIGIN);
    }

    if (has_collision_mode(COLLISION_FACING) && test_collision_facing_point(entity)) {

      if (entity.get_facing_entity() == nullptr) { // make sure only one entity can think "I am the facing entity"
        entity.set_facing_entity(this);
      }
      notify_collision(entity, COLLISION_FACING);
    }

    if (has_collision_mode(COLLISION_TOUCHING) && test_collision_touching(entity)) {
      notify_collision(entity, COLLISION_TOUCHING);
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
 * \brief Checks whether a sprite collides with this detector.
 *
 * If there is a collision, the notify_collision(MapEntity&, Sprite&, Sprite&) method is called.
 *
 * \param entity the entity to check
 * \param sprite the sprite of that entity
 */
void Detector::check_collision(MapEntity& entity, Sprite& sprite) {

  if (has_collision_mode(COLLISION_SPRITE)
      && &entity != this
      && (has_layer_independent_collisions() || get_layer() == entity.get_layer())) {

    // we check the collision between the specified entity's sprite and all sprites of the current entity
    for (const SpritePtr& this_sprite: get_sprites()) {

      if (this_sprite->test_collision(sprite, get_x(), get_y(), entity.get_x(), entity.get_y())) {
        notify_collision(entity, *this_sprite, sprite);
      }
    }
  }
}

/**
 * \brief Returns whether an entity's rectangle is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_RECTANGLE.
 *
 * \param entity the entity
 * \return true if the entity's rectangle is overlapping the detector's rectangle
 */
bool Detector::test_collision_rectangle(MapEntity& entity) {

  return entity.overlaps(*this);
}

/**
 * \brief Returns whether an entity's rectangle is entirely inside the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_INSIDE.
 *
 * \param entity the entity
 * \return true if the entity's rectangle is entirely inside the detector's rectangle
 */
bool Detector::test_collision_inside(MapEntity& entity) {

  return get_bounding_box().contains(entity.get_bounding_box());
}


/**
 * \brief Returns whether the origin point of an entity is overlapping the detector's rectangle.
 *
 * This method is called by check_entity_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_ORIGIN_POINT.
 *
 * \param entity the entity
 * \return true if the entity's origin point is overlapping the detector's rectangle
 */
bool Detector::test_collision_origin_point(MapEntity& entity) {

  return entity.is_origin_point_in(get_bounding_box());
}

/**
 * \brief Returns whether the facing point of an entity is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_FACING_POINT.
 *
 * \param entity the entity
 * \return true if the entity's facing point is overlapping the detector's rectangle
 */
bool Detector::test_collision_facing_point(MapEntity& entity) {

  return entity.is_facing_point_in(get_bounding_box());
}

/**
 * \brief Returns whether a touching point of an entity
 * (in any of the four main directions)
 * is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_TOUCHING_POINT.
 *
 * \param entity The entity.
 * \return \c true if a touching point of the entity is overlapping the
 * detector's rectangle.
 */
bool Detector::test_collision_touching(MapEntity& entity) {

  const Rectangle& bounding_box = get_bounding_box();
  return entity.is_touching_point_in(bounding_box, 0)
      || entity.is_touching_point_in(bounding_box, 1)
      || entity.is_touching_point_in(bounding_box, 2)
      || entity.is_touching_point_in(bounding_box, 3);
}

/**
 * \brief Returns whether the center point of an entity is overlapping the detector's rectangle.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_CENTER.
 *
 * \param entity the entity
 * \return true if the entity's center is overlapping the detector's rectangle
 */
bool Detector::test_collision_center(MapEntity& entity) {

  return entity.is_center_in(get_bounding_box());
}

/**
 * \brief Returns whether an entity collides with this detector with respect to a custom rule.
 *
 * This method is called by check_collision(MapEntity*) when the detector's collision
 * mode is COLLISION_CUSTOM.
 * Redefine it if you want to use this collision mode.
 *
 * \param entity the entity
 * \return true if the entity's collides with this detector with respect to the custom rule
 */
bool Detector::test_collision_custom(MapEntity& /* entity */) {

  Debug::die("Custom collision mode invoked but not defined");
  return false;
}

/**
 * \brief Notifies this detector that a collision was just detected with an entity.
 *
 * This function is called by check_collision(MapEntity*)
 * when an entity overlaps the detector.
 * By default, nothing is done.
 *
 * \param entity_overlapping the entity overlapping the detector
 * \param collision_mode the collision mode that detected the collision (useful if
 * the detector has several collision modes)
 */
void Detector::notify_collision(MapEntity& /* entity_overlapping */, CollisionMode /* collision_mode */) {

}

/**
 * \brief Notifies this detector that a pixel-perfect collision was just detected with another sprite.
 *
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 *
 * \param other_entity the entity overlapping this detector
 * \param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 * \param other_sprite the sprite of other_entity that is overlapping this detector
 */
void Detector::notify_collision(
    MapEntity& /* other_entity */,
    Sprite& /* this_sprite */,
    Sprite& /* other_sprite */
) {
  // By default, nothing happens.
  // Redefine this method in the subclasses to do the appropriate action.
}

/**
 * \brief Notifies this detector that the player is interacting with it by
 * pressing the action command.
 *
 * This function is called when the player presses the action command
 * while the hero is facing this detector, unless the action command effect
 * does not allow the hero to interact with the entity, like while he is
 * carrying an object.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make the hero to interact with
 * this entity.
 *
 * \return \c true if an interaction happened.
 */
bool Detector::notify_action_command_pressed() {
  return false;
}

/**
 * \brief Notifies this detector that the player is interacting with it by
 * using an equipment item.
 *
 * This function is called when the player uses an equipment item
 * while the hero is facing this detector.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make your entity react to an
 * equipment item.
 *
 * \param item The equipment item used.
 * \return true if an interaction occured.
 */
bool Detector::interaction_with_item(EquipmentItem& /* item */) {
  return false;
}

/**
 * \brief This function is called when the player tries to push or pull this
 * detector.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make something happen with the entity.
 *
 * \return true if the detector was pushed or pulled successfully
 */
bool Detector::start_movement_by_hero() {
  return false;
}

/**
 * \brief This function is called when the player finishes to push or pull
 * this detector.
 *
 * By default, nothing happens.
 * Redefine your function in the subclasses to make something happen with the entity.
 *
 * \return true if the detector was pushed or pulled successfully
 */
void Detector::stop_movement_by_hero() {
}

/**
 * \brief This function is called when the player is tapping his sword against this detector.
 * \return the sound to play when tapping this detector with the sword
 */
std::string Detector::get_sword_tapping_sound() {
  return "sword_tapping";
}

/**
 * \brief Notifies this entity that it is about to be removed.
 *
 * This function is called when the entity has just been added
 * to the list of entities that will be removed from the map
 * and deleted from the memory as soon as possible.
 */
void Detector::notify_being_removed() {

  MapEntity::notify_being_removed();

  if (get_hero().get_facing_entity() == this) {
    get_hero().set_facing_entity(nullptr);
  }
}

/**
 * \copydoc MapEntity::notify_position_changed
 */
void Detector::notify_position_changed() {

  // Since this entity is a detector, all entities need to check
  // their collisions with it.
  get_map().check_collision_from_detector(*this);

  MapEntity::notify_position_changed();
}

/**
 * \copydoc MapEntity::notify_layer_changed
 */
void Detector::notify_layer_changed() {

  // Since this entity is a detector, all entities need to check
  // their collisions with it.
  get_map().check_collision_from_detector(*this);

  MapEntity::notify_layer_changed();
}

}

