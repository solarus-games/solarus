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
#include "entities/Bomb.h"
#include "entities/Explosion.h"
#include "entities/MapEntities.h"
#include "entities/Hero.h"
#include "entities/CarriedItem.h"
#include "entities/ConveyorBelt.h"
#include "movements/PathMovement.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "Sprite.h"
#include "Map.h"
#include "KeysEffect.h"

/**
 * @brief Constructor.
 *
 * Creates a bomb.
 *
 * @param layer layer of the entity to create
 * @param x x coordinate of the entity to create
 * @param y y coordinate of the entity to create
 */
Bomb::Bomb(Layer layer, int x, int y):
  Detector(COLLISION_FACING_POINT, "", layer, x, y, 16, 16),
  explosion_date(System::now() + 6000) {

  create_sprite("entities/bomb");
  get_sprite().enable_pixel_collisions();
  set_size(16, 16);
  set_origin(8, 13);
}

/**
 * @brief Destructor.
 */
Bomb::~Bomb() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Bomb::get_type() {
  return BOMB;
}

/**
 * @brief Returns whether entities of this type can be obstacles for other entities.
 *
 * If yes, the function is_obstacle_for() will be called
 * to determine whether this particular entity is an obstacle or not.
 *
 * @return true if this type of entity can be obstacle for other entities
 */
bool Bomb::can_be_obstacle() {
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
bool Bomb::can_detect_entities() {
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
bool Bomb::can_be_displayed() {
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
bool Bomb::is_displayed_in_y_order() {
  return true;
}

/**
 * @brief Returns whether a conveyor belt is currently considered as an obstacle by this entity.
 * @param conveyor_belt a conveyor belt
 * @return true if the conveyor belt is currently an obstacle for this entity
 */
bool Bomb::is_conveyor_belt_obstacle(ConveyorBelt &conveyor_belt) {
  return false;
}

/**
 * @brief This function is called by the engine when an entity overlaps this detector.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Bomb::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_bomb(*this, collision_mode);
}

/**
 * @brief This function is called when an explosion's sprite
 * detects a pixel-perfect collision with a sprite of this entity.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of the current entity that collides with the explosion
 */
void Bomb::notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping) {

  if (!is_being_removed()) {
    explode();
  }
}

/**
 * @brief This function is called when a conveyor belt detects a collision with this entity.
 * @param conveyor_belt a conveyor belt
 * @param dx direction of the x move in pixels (0, 1 or -1)
 * @param dy direction of the y move in pixels (0, 1 or -1)
 */
void Bomb::notify_collision_with_conveyor_belt(ConveyorBelt &conveyor_belt, int dx, int dy) {

  if (get_movement() == NULL) {

    // check that a significant part of the bomb is on the conveyor belt
    Rectangle center = get_center_point();
    center.add_xy(-1, -1);
    center.set_size(2, 2);

    if (conveyor_belt.overlaps(center)) {
      set_xy(conveyor_belt.get_xy());

      std::string path = "  ";
      path[0] = path[1] = '0' + conveyor_belt.get_direction();
      clear_movement();
      set_movement(new PathMovement(path, 64, false, false, false));
    }
  }
}

/**
 * @brief This function is called when the entity has just moved.
 */
void Bomb::notify_position_changed() {

  if (get_hero().get_facing_entity() == this
      && get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_LIFT
      && !get_hero().is_facing_point_in(get_bounding_box())) {

    get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
  }
}

/**
 * @brief Notifies this entity that the player is interacting with it.
 *
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * The hero lifts the bomb if possible.
 */
void Bomb::action_key_pressed() {

  KeysEffect::ActionKeyEffect effect = get_keys_effect().get_action_key_effect();

  if (effect == KeysEffect::ACTION_KEY_LIFT
      && get_hero().get_facing_entity() == this
      && get_hero().is_facing_point_in(get_bounding_box())) {

    get_hero().start_lifting(new CarriedItem(get_hero(), *this,
	"entities/bomb", "", 0, explosion_date));
    Sound::play("lift");
    remove_from_map();
  }
}

/**
 * @brief This function is called by the map when the game is suspended or resumed.
 * @param suspended true to suspend the entity, false to resume it
 */
void Bomb::set_suspended(bool suspended) {

  Detector::set_suspended(suspended); // suspend the animation and the movement

  if (!suspended && when_suspended != 0) {
    // recalculate the timer
    uint32_t diff = System::now() - when_suspended;
    explosion_date += diff;
  }
}

/**
 * @brief This function is called repeatedly.
 */
void Bomb::update() {

  Detector::update();

  if (suspended) {
    return;
  }

  // check the explosion date
  uint32_t now = System::now();
  if (now >= explosion_date) {
    explode();
  }
  else if (now >= explosion_date - 1500
      && get_sprite().get_current_animation() != "stopped_explosion_soon") {
    get_sprite().set_current_animation("stopped_explosion_soon");
  }

  // destroy the movement once finished
  if (get_movement() != NULL && get_movement()->is_finished()) {
    clear_movement();
  }

  // check collision with explosions, conveyor belts, etc.
  get_map().check_collision_with_detectors(*this);
  get_map().check_collision_with_detectors(*this, get_sprite());
}

/**
 * @brief Makes the bomb explode.
 */
void Bomb::explode() {

  get_entities().add_entity(new Explosion(get_layer(), get_center_point(), true));
  Sound::play("explosion");
  remove_from_map();
}

