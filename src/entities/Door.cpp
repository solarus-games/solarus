/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/MapEntities.h"
#include "entities/Door.h"
#include "entities/Hero.h"
#include "entities/DynamicTile.h"
#include "lua/LuaContext.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"
#include "lowlevel/Geometry.h"
#include "Sprite.h"
#include "Game.h"
#include "DialogBox.h"
#include "Equipment.h"
#include "KeysEffect.h"
#include "Savegame.h"
#include "EquipmentItem.h"
#include "Map.h"
#include <list>
#include <sstream>

/**
 * @brief Lua name of each value of the OpeningMethod enum.
 */
const std::string Door::opening_method_names[] = {
  "none",
  "interaction",
  "interaction_if_savegame_variable",
  "interaction_if_item",
  "explosion",
  ""  // Sentinel.
};

/**
 * @brief Creates a door.
 * @param game The game.
 * @param name Name identifying this entity.
 * @param layer Layer of the entity to create.
 * @param x X coordinate of the entity to create.
 * @param y Y coordinate of the entity to create.
 * @param direction Direction of the door.
 * @param sprite_name Name of the animation set of the
 * sprite to create for the door.
 * The sprite must have an animation \c "closed", that will be shown while
 * the door is closed. Optionally, they sprite can also have animations
 * \c "opening" and \c "closing", that will be shown (if they exist)
 * while the door is being opened or closed, respectively.
 * If they don't exist, the door will get opened or closed instantly.
 * @param savegame_variable Boolean variable where the door's state is saved
 * (an empty string lets the door unsaved).
 */
Door::Door(Game& game,
    const std::string& name,
    Layer layer, int x, int y,
    int direction,
    const std::string& sprite_name,
    const std::string& savegame_variable):
  Detector(COLLISION_FACING_POINT | COLLISION_SPRITE, name, layer, x, y, 16, 16),
  savegame_variable(savegame_variable),
  opening_method(OPENING_NONE),
  opening_condition(),
  opening_condition_consumed(false),
  cannot_open_dialog_id(),
  door_open(true),
  changing(false),
  initialized(false),
  next_hint_sound_date(0) {

  Sprite& sprite = create_sprite(sprite_name, true);
  sprite.set_ignore_suspend(true);  // Continue the animation while the camera is moving.
  set_direction(direction);

  if (is_saved()) {
    set_open(game.get_savegame().get_boolean(savegame_variable));
  }
  else {
    set_open(false);
  }
  sprite.set_current_direction(direction);
}

/**
 * @brief Destructor.
 */
Door::~Door() {

}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Door::get_type() {
  return DOOR;
}

/**
 * @brief Returns whether this entity is an obstacle for another one.
 * @param other another entity
 * @return true
 */
bool Door::is_obstacle_for(MapEntity &other) {
  return !is_open() || changing;
}

/**
 * @brief Returns whether this door is open.
 * @return true if this door is open
 */
bool Door::is_open() const {
  return door_open;
}

/**
 * @brief Makes the door opened or closed.
 * @param door_open true to make it opened, false to make it closed
 */
void Door::set_open(bool door_open) {
  
  this->door_open = door_open;

  if (door_open) {
    set_collision_modes(COLLISION_NONE); // to avoid being the hero's facing entity
  }
  else {
    get_sprite().set_current_animation("closed");
    set_collision_modes(COLLISION_FACING_POINT | COLLISION_SPRITE);

    // ensure that we are not closing the door on the hero
    if (is_on_map() && overlaps(get_hero())) {
      get_hero().avoid_collision(*this, (get_direction() + 2) % 4);
    }
  }

  if (is_on_map()) {
    update_dynamic_tiles();

    if (is_saved()) {
      get_savegame().set_boolean(savegame_variable, door_open);
    }

    if (door_open) {
      get_lua_context().door_on_open(*this);
    }
    else {
      get_lua_context().door_on_closed(*this);
    }
  }
}

/**
 * @brief Enables or disables the dynamic tiles related to this door.
 *
 * The dynamic tiles impacted by this function are the ones whose prefix is the door's name
 * followed by "_closed" or "_open", depending on the door state.
 */
void Door::update_dynamic_tiles() {

  std::list<MapEntity*> tiles = get_entities().get_entities_with_prefix(DYNAMIC_TILE, get_name() + "_closed");
  std::list<MapEntity*>::iterator it;
  for (it = tiles.begin(); it != tiles.end(); it++) {
    DynamicTile *tile = (DynamicTile*) *it;
    tile->set_enabled(!door_open);
  }

  tiles = get_entities().get_entities_with_prefix(DYNAMIC_TILE, get_name() + "_open");
  for (it = tiles.begin(); it != tiles.end(); it++) {
    DynamicTile *tile = (DynamicTile*) *it;
    tile->set_enabled(door_open);
  }
}

/**
 * @brief Notifies this detector that a collision was just detected with another entity.
 *
 * This function is called by the engine when there is a collision with another entity.
 *
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Door::notify_collision(MapEntity &entity_overlapping, CollisionMode collision_mode) {

  if (!is_open()
      && is_interaction_required()
      && entity_overlapping.is_hero()
      && !is_changing()) {

    Hero& hero = static_cast<Hero&>(entity_overlapping);

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
        && hero.is_free()) {

      // we show the action icon
      get_keys_effect().set_action_key_effect(can_open() ?
          KeysEffect::ACTION_KEY_OPEN : KeysEffect::ACTION_KEY_LOOK);
    }
  }
}

/**
 * @brief Notifies this detector that a pixel-perfect collision was just detected with another sprite.
 *
 * This function is called by check_collision(MapEntity&, Sprite&) when another entity's
 * sprite overlaps a sprite of this detector.
 *
 * @param other_entity the entity overlapping this detector
 * @param other_sprite the sprite of other_entity that is overlapping this detector
 * @param this_sprite the sprite of this detector that is overlapping the other entity's sprite
 */
void Door::notify_collision(MapEntity& other_entity, Sprite& other_sprite, Sprite& this_sprite) {

  if (other_entity.get_type() == EXPLOSION) {
    notify_collision_with_explosion(static_cast<Explosion&>(other_entity), other_sprite);
  }
}

/**
 * @brief This function is called when an explosion's sprite
 * detects a pixel-perfect collision with a sprite of this entity.
 * @param explosion the explosion
 * @param sprite_overlapping the sprite of the current entity that collides with the explosion
 */
void Door::notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping) {

  if (get_opening_method() == OPENING_BY_EXPLOSION
      && !is_open()
      && !changing) {
    set_opening();
  }
}

/**
 * @brief Returns whether the state of this door is saved.
 * @return \c true if this door is saved.
 */
bool Door::is_saved() const {
  return !get_savegame_variable().empty();
}

/**
 * @brief Returns the name of the boolean saved variable that stores the
 * state of this door.
 * @return The savegame variable, or an empty string if this door is not saved.
 */
const std::string& Door::get_savegame_variable() const {
  return savegame_variable;
}

/**
 * @brief Returns the opening method of this door.
 * @return How this door can be opened.
 */
Door::OpeningMethod Door::get_opening_method() const {
  return opening_method;
}

/**
 * @brief Returns whether this door should be opened by pressing the action
 * command in front of it.
 * @return \c true if the opening method is one of OPENING_BY_INTERACTION,
 * OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE or
 * OPENING_BY_INTERACTION_IF_ITEM.
 */
bool Door::is_interaction_required() const {

  return opening_method == OPENING_BY_INTERACTION
    || opening_method == OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE
    || opening_method == OPENING_BY_INTERACTION_IF_ITEM;
}

/**
 * @brief Sets the opening method of this door.
 * @param opening_method How this door should be opened.
 */
void Door::set_opening_method(OpeningMethod opening_method) {
  this->opening_method = opening_method;
}

/**
 * @brief Returns the savegame variable or the equipment item name required to
 * open this door.
 *
 * A savegame variable is returned if the opening mode is
 * OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE.
 * The hero is allowed to open the door if this saved value is either
 * \c true, an integer greater than zero or a non-empty string.
 *
 * An equipment item's name is returned if the opening mode is
 * OPENING_BY_INTERACTION_IF_ITEM.
 * The hero is allowed to open the door if he has that item and,
 * for items with an amount, if the amount is greater than zero.
 *
 * For the other opening methods, this setting has no effect.
 *
 * @return The savegame variable or the equipment item name required.
 */
const std::string& Door::get_opening_condition() const {
  return opening_condition;
}

/**
 * @brief Sets the savegame variable or the equipment item name required to
 * open this door.
 *
 * You must set a savegame variable if the opening mode is
 * OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE.
 * The hero will be allowed to open the door if this saved value is either
 * \c true, an integer greater than zero or a non-empty string.
 *
 * You must set an equipment item's name if the opening mode is
 * OPENING_BY_INTERACTION_IF_ITEM.
 * The hero will be allowed to open the door if he has that item and,
 * for items with an amount, if the amount is greater than zero.
 *
 * For the other opening methods, this setting has no effect.
 *
 * @param opening_condition The savegame variable or the equipment item name
 * required.
 */
void Door::set_opening_condition(const std::string& opening_condition) {
  this->opening_condition = opening_condition;
}

/**
 * @brief Returns whether opening this door consumes the condition that
 * was required.
 *
 * If this setting is \c true, here is the behavior when the hero opens
 * the door:
 * - If the opening method is OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE,
 *   then the required savegame variable is either:
 *   - set to \c false if it is a boolean,
 *   - decremented if it is an integer,
 *   - set to an empty string if it is a string.
 * - If the opening method is OPENING_BY_INTERACTION_IF_ITEM, then:
 *   - if the required item has an amount, the amount is decremented.
 *   - if the required item has no amount, its possession state is set to zero.
 * - With other opening methods, this setting has no effect.
 *
 * @return \c true if opening this door consumes the condition that was
 * required.
 */
bool Door::is_opening_condition_consumed() const {
  return opening_condition_consumed;
}

/**
 * @brief Sets whether opening this door should consume the condition that
 * was required.
 *
 * If this setting is \c true, here is the behavior when the hero opens
 * the door:
 * - If the opening method is OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE,
 *   then the required savegame variable is either:
 *   - set to \c false if it is a boolean,
 *   - decremented if it is an integer,
 *   - set to an empty string if it is a string.
 * - If the opening method is OPENING_BY_INTERACTION_IF_ITEM, then:
 *   - if the required item has an amount, the amount is decremented.
 *   - if the required item has no amount, its possession state is set to zero.
 * - With other opening methods, this setting has no effect.
 *
 * @param opening_condition_consumed \c true if opening this door should
 * consume the condition that was required.
 */
void Door::set_opening_condition_consumed(bool opening_condition_consumed) {
   this->opening_condition_consumed = opening_condition_consumed;
}

/**
 * @brief Returns whether the required conditions are met to open this door.
 * @return \c true if the hero can currently open the door by pressing the
 * action command.
 */
bool Door::can_open() const {

  switch (get_opening_method()) {

    case OPENING_BY_INTERACTION:
      // No condition: the hero can always open the door.
      return true;

    case OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE:
    {
      // The hero can open the door if a savegame variable is set.
      const std::string& required_savegame_variable = get_opening_condition();
      if (required_savegame_variable.empty()) {
        return false;
      }
      return get_savegame().get_boolean(required_savegame_variable)
        || get_savegame().get_integer(required_savegame_variable) > 0
        || !get_savegame().get_string(required_savegame_variable).empty();
    }

    case OPENING_BY_INTERACTION_IF_ITEM:
    {
      // The hero can open the door if he has an item.
      const std::string& required_item_name = get_opening_condition();
      if (required_item_name.empty()) {
        return false;
      }
      const EquipmentItem& item = get_equipment().get_item(required_item_name);
      return item.is_saved()
        && item.get_variant() > 0
        && (!item.has_amount() || item.get_amount() > 0);
    }

    default:
      return false;
  }
}

/**
 * @brief Returns the id of the dialog to show when the player presses the action
 * command on the door but cannot open it (i.e. if can_open() is false).
 *
 * @return The id of the "cannot open" dialog for this door
 * (an empty string means no dialog).
 */
const std::string& Door::get_cannot_open_dialog_id() const {
  return cannot_open_dialog_id;
}

/**
 * @brief Sets the id of the dialog to show when the player presses the action
 * command on the door but cannot open it (i.e. if can_open() is false).
 * @param cannot_open_dialog_id The id of the "cannot open" dialog for this door
 * (an empty string means no dialog).
 */
void Door::set_cannot_open_dialog_id(const std::string& cannot_open_dialog_id) {
  this->cannot_open_dialog_id = cannot_open_dialog_id;
}

/**
 * @brief Suspends or resumes the entity.
 * @param suspended true to suspend the entity
 */
void Door::set_suspended(bool suspended) {

  Detector::set_suspended(suspended);

  if (!suspended && next_hint_sound_date > 0) {
    next_hint_sound_date += System::now() - when_suspended;
  }
}

/**
 * @brief Updates the entity.
 */
void Door::update() {

  Detector::update();

  if (!initialized) {
    update_dynamic_tiles();
    initialized = true;
  }

  if (!is_open()
      && get_opening_method() == OPENING_BY_EXPLOSION
      && get_equipment().has_ability("detect_weak_walls")
      && Geometry::get_distance(get_center_point(), get_hero().get_center_point()) < 40
      && !is_suspended()
      && System::now() >= next_hint_sound_date) {
    Sound::play("cane");
    next_hint_sound_date = System::now() + 500;
  }

  if (changing && get_sprite().is_animation_finished()) {
    changing = false;
    set_open(!is_open());
  }

  if (is_saved()) {
    bool open_in_savegame = get_savegame().get_boolean(savegame_variable);
    if (open_in_savegame && !is_open() && !changing) {
      set_opening();
    }
    else if (!open_in_savegame && is_open() && !changing) {
      set_closing();
    }
  }
}

/**
 * @brief Draws the entity on the map.
 */
void Door::draw_on_map() {

  if (has_sprite() && (!is_open() || changing)) {
    Detector::draw_on_map();
  }
}

/**
 * @brief Notifies this detector that the player is interacting with it by
 * pressing the action command.
 *
 * This function is called when the player presses the action command
 * while the hero is facing this detector, and the action command effect lets
 * him do this.
 * The hero opens the door if possible, otherwise a message is shown.
 */
void Door::notify_action_command_pressed() {

  if (get_hero().is_free()
      && is_interaction_required()
      && !is_changing()) {
    // The action command is indeed the method to open the door.

    if (can_open()) {
      Sound::play("door_unlocked");
      Sound::play("door_open");

      if (is_saved()) {
        get_savegame().set_boolean(savegame_variable, true);
      }

      if (is_opening_condition_consumed()) {
        consume_opening_condition();
      }

      set_opening();

      get_hero().check_position();
    }
    else if (!cannot_open_dialog_id.empty()) {
      Sound::play("wrong");
      get_dialog_box().start_dialog(cannot_open_dialog_id);
    }
  }
}

/**
 * @brief Consumes the savegame variable or the equipment item that was required
 * to open the door.
 */
void Door::consume_opening_condition() {

  switch (get_opening_method()) {

    case OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE:
    {
      // Reset or decrement the savegame variable that was required.
      const std::string& required_savegame_variable = get_opening_condition();
      Savegame& savegame = get_savegame();
      if (!required_savegame_variable.empty()) {
        if (savegame.is_boolean(required_savegame_variable)) {
          savegame.set_boolean(required_savegame_variable, false);
        }
        else if (savegame.is_integer(required_savegame_variable)) {
          int current_value = savegame.get_integer(required_savegame_variable);
          savegame.set_integer(required_savegame_variable, current_value - 1);
        }
        else if (savegame.is_string(required_savegame_variable)) {
          savegame.set_string(required_savegame_variable, "");
        }
      }
      break;
    }

    case OPENING_BY_INTERACTION_IF_ITEM:
    {
      // Remove the equipment item that was required.
      if (!opening_condition.empty()) {
        EquipmentItem& item = get_equipment().get_item(opening_condition);
        if (item.is_saved() && item.get_variant() > 0) {
          if (item.has_amount()) {
            item.set_amount(item.get_amount() - 1);
          }
          else {
            item.set_variant(0);
          }
        }
      }
      break;
    }

    default:
      // Ignored.
      break;
  }
}

/**
 * @brief This function is called when the player is tapping his sword against this detector.
 * @return The sound to play when tapping this detector with the sword.
 */
std::string Door::get_sword_tapping_sound() {

  return get_opening_method() == OPENING_BY_EXPLOSION ?
    "sword_tapping_weak_wall" : "sword_tapping";
}

/**
 * @brief Starts opening the door and plays the corresponding animations.
 *
 * Nothing is done if the door is already in the process of being open.
 */
void Door::open() {

  Debug::check_assertion(!is_open() || changing,
      StringConcat() << "Door '" << get_name() << "' is already open");

  if (changing) {
    if (is_open()) {
      // the door is being closed: mark it as closed so that we can open it
      door_open = false;
    }
    else {
      // the door is already being open: nothing to do
      return;
    }
  }

  set_opening();

  if (is_saved()) {
    get_savegame().set_boolean(savegame_variable, true);
  }
}

/**
 * @brief Makes the door being opened.
 */
void Door::set_opening() {

  if (get_sprite().has_animation("opening")) {
    get_sprite().set_current_animation("opening");
    changing = true;
  }
  else {
    set_open(true);
  }
}

/**
 * @brief Starts closing the door and plays the corresponding animations.
 *
 * This function can be called only for a door with subtype CLOSED.
 * Nothing is done if the door is already in the process of being closed.
 */
void Door::close() {

  Debug::check_assertion(is_open() || changing,
      StringConcat() << "Door '" << get_name() << "' is already closed");

  if (changing) {
    if (!is_open()) {
      // the door is being open: mark it as open so that we can close it
      door_open = true;
    }
    else {
      // the door is already being closed: nothing to do
      return;
    }
  }

  set_closing();

  if (is_saved()) {
    get_savegame().set_boolean(savegame_variable, false);
  }
}

/**
 * @brief Makes the door being closed.
 */
void Door::set_closing() {

  if (get_sprite().has_animation("closing")) {
    get_sprite().set_current_animation("closing");
  }
  changing = true;
}

/**
 * @brief Returns true if the door is currently being open or closed.
 * @return true if the door is currently being open or closed
 */
bool Door::is_changing() const {
  return changing;
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& Door::get_lua_type_name() const {
  return LuaContext::entity_door_module_name;
}

