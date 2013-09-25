/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/Destructible.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "entities/Pickable.h"
#include "entities/CarriedItem.h"
#include "movements/FallingHeight.h"
#include "lua/LuaContext.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Game.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "Treasure.h"
#include "Map.h"
#include "Sprite.h"
#include <lauxlib.h>

/**
 * \brief Features of each type of destructible item.
 */
const Destructible::Features Destructible::features[] = {
  // animation set, sound, can be lifted, can be cut, can_explode, can_regenerate, weight, damage, special ground
  {"pot",         "entities/pot",               "stone", true,  false, false, false, 0, 2, GROUND_EMPTY},
  {"",            "",                           "",      false, false, false, false, 0, 1, GROUND_EMPTY},
  {"bush",        "entities/bush",              "bush",  true,  true,  false, false, 1, 1, GROUND_EMPTY},
  {"white_stone", "entities/stone_small_white", "stone", true,  false, false, false, 1, 2, GROUND_EMPTY},
  {"black_stone", "entities/stone_small_black", "stone", true,  false, false, false, 2, 4, GROUND_EMPTY},
  {"grass",       "entities/grass",             "bush",  false, true,  false, false, 0, 0, GROUND_GRASS},
  {"bomb_flower", "entities/bomb_flower",       "bush",  true,  true,  true,  true,  1, 1, GROUND_EMPTY},
};

/**
 * \brief Creates a new destructible item with the specified subtype.
 * \param name Unique name identifying the entity on the map or an empty string.
 * \param layer layer of the destructible item to create on the map
 * \param x x coordinate of the destructible item to create
 * \param y y coordinate of the destructible item to create
 * \param subtype subtype of destructible item to create
 * \param treasure the pickable item that appears when the destructible
 * item is lifted or cut
 */
Destructible::Destructible(
    const std::string& name,
    Layer layer,
    int x,
    int y,
    Subtype subtype,
    const Treasure& treasure):

  Detector(COLLISION_NONE, name, layer, x, y, 16, 16),
  subtype(subtype),
  treasure(treasure),
  is_being_cut(false),
  regeneration_date(0),
  is_regenerating(false),
  modified_ground(features[subtype].special_ground),
  destruction_callback_ref(LUA_REFNIL) {

  set_origin(8, 13);
  create_sprite(get_animation_set_id());

  // set the collision mode
  if (features[subtype].can_be_lifted) {
    add_collision_mode(COLLISION_FACING_POINT);
  }

  if (features[subtype].can_be_cut
      || features[subtype].can_explode) {
    add_collision_mode(COLLISION_SPRITE);
  }
}

/**
 * \brief Destructor.
 */
Destructible::~Destructible() {

  get_lua_context().cancel_callback(this->destruction_callback_ref);
}

/**
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Destructible::get_type() const {
  return DESTRUCTIBLE;
}

/**
 * \brief Returns whether this entity has to be drawn in y order.
 *
 * Rhis function returns whether an entity of this type should be drawn above
 * the hero and other entities when it is in front of them.
 *
 * \return true if this entity is drawn at the same level as the hero
 */
bool Destructible::is_drawn_in_y_order() {
  return false;
}

/**
 * \brief Returns whether entities of this type can override the ground
 * of where they are placed.
 * \return \c true if this type of entity can change the ground.
 */
bool Destructible::is_ground_modifier() const {
  // Because some kinds of destructibles (like grass) set a special ground.
  return features[subtype].special_ground != GROUND_EMPTY;
}

/**
 * \brief When is_ground_modifier() is \c true, returns the ground defined
 * by this entity.
 * \return The ground defined by this entity.
 */
Ground Destructible::get_modified_ground() const {

  return modified_ground;
}

/**
 * \brief Returns the damage this destructible item can cause to enemies
 * \return the damage on enemies
 */
int Destructible::get_damage_on_enemies() {
  return features[subtype].damage_on_enemies;
}

/**
 * \brief Returns the animation set of this destructible item.
 * \return the animations of the sprite
 */
const std::string& Destructible::get_animation_set_id() {
  return features[subtype].animation_set_id;
}

/**
 * \brief Returns the id of the sound to play when this item is destroyed.
 * \return the destruction sound id
 */
const std::string& Destructible::get_destruction_sound_id() {
  return features[subtype].destruction_sound_id;
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 *
 * For a destructible item, this does not depend on the other
 * entity but only on the subtype of destructible item.
 *
 * \param other another entity
 * \return true if this entity is an obstacle for others
 */
bool Destructible::is_obstacle_for(MapEntity& other) {
  return features[subtype].can_be_lifted && !is_being_cut && other.is_destructible_obstacle(*this);
}

/**
 * \brief Tests whether an entity's collides with this entity.
 *
 * This custom collision test is used for destructible items that change the ground drawn under the hero.
 *
 * \param entity an entity
 * \return true if the entity's collides with this entity
 */
bool Destructible::test_collision_custom(MapEntity &entity) {
  return overlaps(entity.get_x(), entity.get_y() - 2);
}

/**
 * \brief Adds to the map the pickable treasure (if any) hidden under this destructible item.
 */
void Destructible::create_pickable() {

  get_entities().add_entity(Pickable::create(get_game(),
      "", get_layer(), get_x(), get_y(),
      treasure, FALLING_MEDIUM, false));
}

/**
 * \brief This function is called by the engine when an entity overlaps the destructible item.
 *
 * If the entity is the hero, we allow him to lift the item.
 *
 * \param entity_overlapping the entity overlapping the detector
 * \param collision_mode the collision mode that detected the collision
 */
void Destructible::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  entity_overlapping.notify_collision_with_destructible(*this, collision_mode);
}

/**
 * \brief This function is called when this entity detects a collision with the hero.
 * \param hero the hero
 * \param collision_mode the collision mode that detected the collision
 */
void Destructible::notify_collision_with_hero(Hero& hero, CollisionMode collision_mode) {

  if (features[subtype].can_be_lifted
      && !is_being_cut
      && !is_disabled()
      && !is_regenerating
      && get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
      && hero.is_free()) {

    int weight = features[subtype].weight;
    if (get_equipment().has_ability("lift", weight)) {
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LIFT);
    }
    else {
      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * \brief Notifies this entity that another sprite is overlapping it.
 *
 * This function is called by check_collision(MapEntity*, Sprite*) when another entity's
 * sprite overlaps a sprite of this detector.
 *
 * \param other_entity the entity overlapping this detector
 * \param other_sprite the sprite of other_entity that is overlapping this detector
 * \param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Destructible::notify_collision(MapEntity& other_entity,
    Sprite& other_sprite, Sprite& this_sprite) {

  if (features[subtype].can_be_cut
      && !is_being_cut
      && !is_disabled()
      && !is_regenerating
      && other_entity.is_hero()
      && other_sprite.contains("sword")) {

    Hero& hero = static_cast<Hero&>(other_entity);
    if (hero.is_striking_with_sword(*this)) {

      play_destroy_animation();
      hero.check_position(); // to update the ground under the hero
      create_pickable();

      if (can_explode()) {
        explode();
      }
    }
  }

  // TODO use dynamic dispatch
  if (other_entity.get_type() == EXPLOSION
      && can_explode()
      && !is_being_cut
      && !is_disabled()
      && !is_regenerating) {

    play_destroy_animation();
    create_pickable();
    explode();
  }
}

/**
 * \brief Notifies this detector that the player is interacting with it by
 * pressing the action command.
 *
 * This function is called when the player presses the action command
 * while the hero is facing this detector, and the action command effect lets
 * him do this.
 */
void Destructible::notify_action_command_pressed() {

  KeysEffect::ActionKeyEffect effect = get_keys_effect().get_action_key_effect();

  if ((effect == KeysEffect::ACTION_KEY_LIFT || effect == KeysEffect::ACTION_KEY_LOOK)
      && features[subtype].can_be_lifted
      && !is_being_cut
      && !is_disabled()
      && !is_regenerating) {

    int weight = features[subtype].weight;

    if (get_equipment().has_ability("lift", weight)) {

      uint32_t explosion_date = can_explode() ? System::now() + 6000 : 0;
      get_hero().start_lifting(new CarriedItem(get_hero(), *this,
	  get_animation_set_id(), get_destruction_sound_id(), get_damage_on_enemies(), explosion_date));

      // play the sound
      Sound::play("lift");

      // create the pickable item
      create_pickable();

      // remove the item from the map
      if (!features[subtype].can_regenerate) {
        destruction_callback();
        remove_from_map();
      }
      else {
        // the item can actually regenerate
        play_destroy_animation();
      }
    }
    else {
      if (features[subtype].can_be_cut
          && !features[subtype].can_explode
          && !get_equipment().has_ability("sword", 1)) {
        get_game().start_dialog("_cannot_lift_should_cut", LUA_REFNIL);
      }
      else if (!get_equipment().has_ability("lift", 1)) {
        get_game().start_dialog("_cannot_lift_too_heavy", LUA_REFNIL);
      }
      else {
        get_game().start_dialog("_cannot_lift_still_too_heavy", LUA_REFNIL);
      }
    }
  }
}

/**
 * \brief Plays the animation destroy of this item.
 */
void Destructible::play_destroy_animation() {

  is_being_cut = true;
  modified_ground = GROUND_EMPTY;
  Sound::play(get_destruction_sound_id());
  get_sprite().set_current_animation("destroy");
  if (!is_drawn_in_y_order()) {
    get_entities().bring_to_front(this); // show animation destroy to front
  }
  update_ground_observers();  // The ground (if any) has just disappeared.
}

/**
 * \brief Sets a Lua function to be called when this destructible item is
 * destroyed.
 * \param destruction_callback_ref a Lua ref to the callback in the registry
 * (if you pass LUA_REFNIL, this function removes the previous callback that
 * was set, if any)
 */
void Destructible::set_destruction_callback(int destruction_callback_ref) {

  if (this->destruction_callback_ref != LUA_REFNIL) {
    get_lua_context().cancel_callback(this->destruction_callback_ref);
  }
  this->destruction_callback_ref = destruction_callback_ref;
}

/**
 * \brief Calls the Lua destruction callback of this item (if any).
 */
void Destructible::destruction_callback() {

  get_lua_context().do_callback(destruction_callback_ref);
  destruction_callback_ref = LUA_REFNIL;
}

/**
 * \brief Returns whether the item is disabled.
 *
 * The item is disabled if it was lifted and is about to regenerate.
 *
 * \return true if the item is disabled
 */
bool Destructible::is_disabled() const {
  return regeneration_date != 0 && !is_regenerating;
}

/**
 * \brief Returns whether the item can explode.
 * \return true if the item will explode
 */
bool Destructible::can_explode() {
  return features[subtype].can_explode;
}

/**
 * \brief Creates an explosion on the item.
 */
void Destructible::explode() {
  get_entities().add_entity(new Explosion("", get_layer(), get_xy(), true));
  Sound::play("explosion");
}

/**
 * \brief This function is called by the map when the game is suspended or resumed.
 * \param suspended true to suspend the entity, false to resume it
 */
void Destructible::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended); // suspend the animation and the movement

  if (!suspended && regeneration_date != 0) {
    // recalculate the timer
    regeneration_date += System::now() - when_suspended;
  }
}

/**
 * \brief Updates the item.
 */
void Destructible::update() {

  MapEntity::update();

  if (suspended) {
    return;
  }

  if (is_being_cut && get_sprite().is_animation_finished()) {

    if (!features[subtype].can_regenerate) {
      // remove the item from the map
      destruction_callback();
      remove_from_map();
    }
    else {
      is_being_cut = false;
      regeneration_date = System::now() + 10000;
    }
  }

  else if (is_disabled() && System::now() >= regeneration_date && !overlaps(get_hero())) {
    get_sprite().set_current_animation("regenerating");
    is_regenerating = true;
    regeneration_date = 0;
    modified_ground = features[subtype].special_ground;
  }
  else if (is_regenerating && get_sprite().is_animation_finished()) {
    get_sprite().set_current_animation("on_ground");
    is_regenerating = false;
  }
}

/**
 * \brief Converts a value of the Subtype enumeration into a string.
 * \param subtype a subtype of destructible item
 * \return the name of this subtype
 */
const std::string& Destructible::get_subtype_name(Subtype subtype) {

  Debug::check_assertion(subtype >= 0 && subtype != DEPRECATED_1
      && subtype < SUBTYPE_NUMBER, StringConcat()
      << "Invalid destructible item subtype number: " << subtype);

  return features[subtype].name;
}

/**
 * \brief Converts a subtype name into a value of the Subtype enumeration.
 * \param subtype_name the name of a destructible item subtype
 * \return the corresponding subtype
 */
Destructible::Subtype Destructible::get_subtype_by_name(
    const std::string& subtype_name) {

  for (int i = 0; i < SUBTYPE_NUMBER; i++) {
    if (i != DEPRECATED_1 && features[i].name == subtype_name) {
      return Subtype(i);
    }
  }

  Debug::die(StringConcat() << "Invalid destructible item subtype name: '"
      << subtype_name << "'");
  throw;
}

