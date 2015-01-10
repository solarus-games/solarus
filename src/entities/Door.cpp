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
#include "solarus/entities/MapEntities.h"
#include "solarus/entities/Door.h"
#include "solarus/entities/DynamicTile.h"
#include "solarus/entities/Explosion.h"
#include "solarus/entities/Hero.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Geometry.h"
#include "solarus/lowlevel/Sound.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItem.h"
#include "solarus/Game.h"
#include "solarus/KeysEffect.h"
#include "solarus/Map.h"
#include "solarus/Savegame.h"
#include "solarus/Sprite.h"
#include <list>
#include <lua.hpp>

namespace Solarus {

/**
 * \brief Lua name of each value of the OpeningMethod enum.
 */
const std::map<Door::OpeningMethod, std::string> Door::opening_method_names = {
    { OpeningMethod::NONE, "none" },
    { OpeningMethod::BY_INTERACTION, "interaction" },
    { OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE, "interaction_if_savegame_variable" },
    { OpeningMethod::BY_INTERACTION_IF_ITEM, "interaction_if_item" },
    { OpeningMethod::BY_EXPLOSION, "explosion" }
};

/**
 * \brief Creates a door.
 * \param game The game.
 * \param name Name identifying this entity.
 * \param layer Layer of the entity to create.
 * \param xy Coordinates of the entity to create.
 * \param direction Direction of the door.
 * \param sprite_name Name of the animation set of the
 * sprite to create for the door.
 * The sprite must have an animation \c "closed", that will be shown while
 * the door is closed. Optionally, they sprite can also have animations
 * \c "opening" and \c "closing", that will be shown (if they exist)
 * while the door is being opened or closed, respectively.
 * If they don't exist, the door will get opened or closed instantly.
 * \param savegame_variable Boolean variable where the door's state is saved
 * (an empty string lets the door unsaved).
 */
Door::Door(Game& game,
    const std::string& name,
    Layer layer, const Point& xy,
    int direction,
    const std::string& sprite_name,
    const std::string& savegame_variable):
  Detector(COLLISION_FACING | COLLISION_SPRITE, name, layer, xy, Size(16, 16)),
  savegame_variable(savegame_variable),
  opening_method(OpeningMethod::NONE),
  opening_condition(),
  opening_condition_consumed(false),
  cannot_open_dialog_id(),
  state(OPEN),
  initialized(false),
  next_hint_sound_date(0) {

  Sprite& sprite = *create_sprite(sprite_name, true);
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
 * \brief Returns the type of entity.
 * \return the type of entity
 */
EntityType Door::get_type() const {
  return EntityType::DOOR;
}

/**
 * \brief Returns whether this entity is an obstacle for another one.
 * \param other another entity
 * \return true
 */
bool Door::is_obstacle_for(MapEntity& /* other */) {
  return !is_open();
}

/**
 * \brief Returns whether this door is completely open.
 * \return true if this door is open, false if it is closed, closing or
 * opening.
 */
bool Door::is_open() const {
  return state == OPEN;
}

/**
 * \brief Returns whether this door is being opened.
 * \return true if this door is being opened.
 */
bool Door::is_opening() const {
  return state == OPENING;
}

/**
 * \brief Returns whether this door is completely closed.
 * \return true if this door is closed, false if it is open, opening or
 * closing.
 */
bool Door::is_closed() const {
  return state == CLOSED;
}

/**
 * \brief Returns whether this door is being closed.
 * \return true if this door is being closed.
 */
bool Door::is_closing() const {
  return state == CLOSING;
}

/**
 * \brief Returns true if the door is currently being open or being closed.
 * \return true if the door is currently being open or being closed.
 */
bool Door::is_changing() const {
  return state == OPENING || state == CLOSING;
}

/**
 * \brief Makes the door immediately open or closed.
 * \param door_open true to make it opened, false to make it closed.
 */
void Door::set_open(bool door_open) {

  state = door_open ? OPEN : CLOSED;

  if (door_open) {
    set_collision_modes(COLLISION_NONE); // to avoid being the hero's facing entity
  }
  else {
    get_sprite().set_current_animation("closed");
    set_collision_modes(COLLISION_FACING | COLLISION_SPRITE);

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
      get_lua_context().door_on_opened(*this);
    }
    else {
      get_lua_context().door_on_closed(*this);
    }
  }
}

/**
 * \brief Enables or disables the dynamic tiles related to this door.
 *
 * The dynamic tiles impacted by this function are the ones whose prefix is the door's name
 * followed by "_closed" or "_open", depending on the door state.
 */
void Door::update_dynamic_tiles() {

  std::list<MapEntity*> tiles = get_entities().get_entities_with_prefix(EntityType::DYNAMIC_TILE, get_name() + "_closed");
  for (MapEntity* tile: tiles) {
    tile->set_enabled(is_closed() || is_opening());
  }

  tiles = get_entities().get_entities_with_prefix(EntityType::DYNAMIC_TILE, get_name() + "_open");
  for (MapEntity* tile: tiles) {
    tile->set_enabled(is_open() || is_closing());
  }
}

/**
 * \brief Notifies this detector that a collision was just detected with another entity.
 *
 * This function is called by the engine when there is a collision with another entity.
 *
 * \param entity_overlapping the entity overlapping the detector
 * \param collision_mode the collision mode that detected the collision
 */
void Door::notify_collision(MapEntity& entity_overlapping, CollisionMode /* collision_mode */) {

  if (is_closed() && entity_overlapping.is_hero()) {

    Hero& hero = static_cast<Hero&>(entity_overlapping);

    if (get_keys_effect().get_action_key_effect() == KeysEffect::ACTION_KEY_NONE
        && hero.is_free()) {

      if (can_open()) {
        // The action command opens the door.
        get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_OPEN);
      }
      else if (!get_cannot_open_dialog_id().empty()) {
        // The action command shows a dialog.
        get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_LOOK);
      }
    }
  }
}

/**
 * \copydoc Detector::notify_collision(MapEntity&, Sprite&, Sprite&)
 */
void Door::notify_collision(MapEntity& other_entity, Sprite& /* this_sprite */, Sprite& other_sprite) {

  if (other_entity.get_type() == EntityType::EXPLOSION) {
    notify_collision_with_explosion(static_cast<Explosion&>(other_entity), other_sprite);
  }
}

/**
 * \brief This function is called when an explosion's sprite
 * detects a pixel-perfect collision with a sprite of this entity.
 * \param explosion the explosion
 * \param sprite_overlapping the sprite of the current entity that collides with the explosion
 */
void Door::notify_collision_with_explosion(Explosion& /* explosion */, Sprite& /* sprite_overlapping */) {

  if (get_opening_method() == OpeningMethod::BY_EXPLOSION
      && is_closed()) {
    set_opening();
  }
}

/**
 * \brief Returns whether the state of this door is saved.
 * \return \c true if this door is saved.
 */
bool Door::is_saved() const {
  return !get_savegame_variable().empty();
}

/**
 * \brief Returns the name of the boolean saved variable that stores the
 * state of this door.
 * \return The savegame variable, or an empty string if this door is not saved.
 */
const std::string& Door::get_savegame_variable() const {
  return savegame_variable;
}

/**
 * \brief Returns the opening method of this door.
 * \return How this door can be opened.
 */
Door::OpeningMethod Door::get_opening_method() const {
  return opening_method;
}

/**
 * \brief Returns whether this door should be opened by pressing the action
 * command in front of it.
 * \return \c true if the opening method is one of OpeningMethod::BY_INTERACTION,
 * OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE or
 * OpeningMethod::BY_INTERACTION_IF_ITEM.
 */
bool Door::is_interaction_required() const {

  return opening_method == OpeningMethod::BY_INTERACTION
    || opening_method == OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE
    || opening_method == OpeningMethod::BY_INTERACTION_IF_ITEM;
}

/**
 * \brief Sets the opening method of this door.
 * \param opening_method How this door should be opened.
 */
void Door::set_opening_method(OpeningMethod opening_method) {
  this->opening_method = opening_method;
}

/**
 * \brief Returns the savegame variable or the equipment item name required to
 * open this door.
 *
 * A savegame variable is returned if the opening mode is
 * OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE.
 * The hero is allowed to open the door if this saved value is either
 * \c true, an integer greater than zero or a non-empty string.
 *
 * An equipment item's name is returned if the opening mode is
 * OpeningMethod::BY_INTERACTION_IF_ITEM.
 * The hero is allowed to open the door if he has that item and,
 * for items with an amount, if the amount is greater than zero.
 *
 * For the other opening methods, this setting has no effect.
 *
 * \return The savegame variable or the equipment item name required.
 */
const std::string& Door::get_opening_condition() const {
  return opening_condition;
}

/**
 * \brief Sets the savegame variable or the equipment item name required to
 * open this door.
 *
 * You must set a savegame variable if the opening mode is
 * OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE.
 * The hero will be allowed to open the door if this saved value is either
 * \c true, an integer greater than zero or a non-empty string.
 *
 * You must set an equipment item's name if the opening mode is
 * OpeningMethod::BY_INTERACTION_IF_ITEM.
 * The hero will be allowed to open the door if he has that item and,
 * for items with an amount, if the amount is greater than zero.
 *
 * For the other opening methods, this setting has no effect.
 *
 * \param opening_condition The savegame variable or the equipment item name
 * required.
 */
void Door::set_opening_condition(const std::string& opening_condition) {
  this->opening_condition = opening_condition;
}

/**
 * \brief Returns whether opening this door consumes the condition that
 * was required.
 *
 * If this setting is \c true, here is the behavior when the hero opens
 * the door:
 * - If the opening method is OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE,
 *   then the required savegame variable is either:
 *   - set to \c false if it is a boolean,
 *   - decremented if it is an integer,
 *   - set to an empty string if it is a string.
 * - If the opening method is OpeningMethod::BY_INTERACTION_IF_ITEM, then:
 *   - if the required item has an amount, the amount is decremented.
 *   - if the required item has no amount, its possession state is set to zero.
 * - With other opening methods, this setting has no effect.
 *
 * \return \c true if opening this door consumes the condition that was
 * required.
 */
bool Door::is_opening_condition_consumed() const {
  return opening_condition_consumed;
}

/**
 * \brief Sets whether opening this door should consume the condition that
 * was required.
 *
 * If this setting is \c true, here is the behavior when the hero opens
 * the door:
 * - If the opening method is OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE,
 *   then the required savegame variable is either:
 *   - set to \c false if it is a boolean,
 *   - decremented if it is an integer,
 *   - set to an empty string if it is a string.
 * - If the opening method is OpeningMethod::BY_INTERACTION_IF_ITEM, then:
 *   - if the required item has an amount, the amount is decremented.
 *   - if the required item has no amount, its possession state is set to zero.
 * - With other opening methods, this setting has no effect.
 *
 * \param opening_condition_consumed \c true if opening this door should
 * consume the condition that was required.
 */
void Door::set_opening_condition_consumed(bool opening_condition_consumed) {
   this->opening_condition_consumed = opening_condition_consumed;
}

/**
 * \brief Returns whether the required conditions are met to open this door.
 * \return \c true if the hero can currently open the door by pressing the
 * action command.
 */
bool Door::can_open() const {

  switch (get_opening_method()) {

    case OpeningMethod::BY_INTERACTION:
      // No condition: the hero can always open the door.
      return true;

    case OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE:
    {
      // The hero can open the door if a savegame variable is set.
      const std::string& required_savegame_variable = get_opening_condition();
      if (required_savegame_variable.empty()) {
        return false;
      }

      const Savegame& savegame = get_savegame();
      if (savegame.is_boolean(required_savegame_variable)) {
        return savegame.get_boolean(required_savegame_variable);
      }

      if (savegame.is_integer(required_savegame_variable)) {
        return savegame.get_integer(required_savegame_variable) > 0;
      }

      if (savegame.is_string(required_savegame_variable)) {
        return !savegame.get_string(required_savegame_variable).empty();
      }

      return false;
    }

    case OpeningMethod::BY_INTERACTION_IF_ITEM:
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
 * \brief Returns the id of the dialog to show when the player presses the action
 * command on the door but cannot open it (i.e. if can_open() is false).
 *
 * \return The id of the "cannot open" dialog for this door
 * (an empty string means no dialog).
 */
const std::string& Door::get_cannot_open_dialog_id() const {
  return cannot_open_dialog_id;
}

/**
 * \brief Sets the id of the dialog to show when the player presses the action
 * command on the door but cannot open it (i.e. if can_open() is false).
 * \param cannot_open_dialog_id The id of the "cannot open" dialog for this door
 * (an empty string means no dialog).
 */
void Door::set_cannot_open_dialog_id(const std::string& cannot_open_dialog_id) {
  this->cannot_open_dialog_id = cannot_open_dialog_id;
}

/**
 * \brief Suspends or resumes the entity.
 * \param suspended true to suspend the entity
 */
void Door::set_suspended(bool suspended) {

  Detector::set_suspended(suspended);

  if (!suspended && next_hint_sound_date > 0) {
    next_hint_sound_date += System::now() - get_when_suspended();
  }
}

/**
 * \brief Updates the entity.
 */
void Door::update() {

  Detector::update();

  if (!initialized) {
    update_dynamic_tiles();
    initialized = true;
  }

  if (is_closed()
      && get_opening_method() == OpeningMethod::BY_EXPLOSION
      && get_equipment().has_ability(Ability::DETECT_WEAK_WALLS)
      && Geometry::get_distance(get_center_point(), get_hero().get_center_point()) < 40
      && !is_suspended()
      && System::now() >= next_hint_sound_date) {
    Sound::play("cane");
    next_hint_sound_date = System::now() + 500;
  }

  if (is_changing() && get_sprite().is_animation_finished()) {
    // Toggle door_open when the changing animation finishes.
    set_open(is_opening());
  }

  if (is_saved() && !is_changing()) {
    bool open_in_savegame = get_savegame().get_boolean(savegame_variable);
    if (open_in_savegame && is_closed()) {
      set_opening();
    }
    else if (!open_in_savegame && is_open()) {
      set_closing();
    }
  }
}

/**
 * \brief Draws the entity on the map.
 */
void Door::draw_on_map() {

  if (!is_drawn()) {
    return;
  }

  if (has_sprite() && !is_open()) {
    Detector::draw_on_map();
  }
}

/**
 * \copydoc Detector::notify_action_command_pressed
 */
bool Door::notify_action_command_pressed() {

  if (get_hero().is_free() &&
      is_closed() &&
      get_keys_effect().get_action_key_effect() != KeysEffect::ACTION_KEY_NONE
  ) {

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
      get_game().start_dialog(cannot_open_dialog_id, ScopedLuaRef(), ScopedLuaRef());
    }

    return true;
  }

  return false;
}

/**
 * \brief Consumes the savegame variable or the equipment item that was required
 * to open the door.
 */
void Door::consume_opening_condition() {

  switch (get_opening_method()) {

    case OpeningMethod::BY_INTERACTION_IF_SAVEGAME_VARIABLE:
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

    case OpeningMethod::BY_INTERACTION_IF_ITEM:
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
 * \brief This function is called when the player is tapping his sword against this detector.
 * \return The sound to play when tapping this detector with the sword.
 */
std::string Door::get_sword_tapping_sound() {

  return get_opening_method() == OpeningMethod::BY_EXPLOSION ?
    "sword_tapping_weak_wall" : "sword_tapping";
}

/**
 * \brief Starts opening the door and plays the corresponding animations.
 *
 * Nothing is done if the door is already in the process of being open.
 */
void Door::open() {

  if (is_open() || is_opening()) {
    // The door is already open or being open: nothing to do.
    return;
  }

  set_opening();

  if (is_saved()) {
    get_savegame().set_boolean(savegame_variable, true);
  }
}

/**
 * \brief Makes the door being opened.
 */
void Door::set_opening() {

  if (get_sprite().has_animation("opening")) {
    state = OPENING;
    get_sprite().set_current_animation("opening");
  }
  else {
    set_open(true);
  }
}

/**
 * \brief Starts closing the door and plays the corresponding animations.
 *
 * Nothing is done if the door is already in the process of being closed.
 */
void Door::close() {

  if (is_closed() || is_closing()) {
    // The door is already closed or being closed: nothing to do.
    return;
  }

  set_closing();

  if (is_saved()) {
    get_savegame().set_boolean(savegame_variable, false);
  }
}

/**
 * \brief Makes the door being closed.
 */
void Door::set_closing() {

  if (get_sprite().has_animation("closing")) {
    state = CLOSING;
    get_sprite().set_current_animation("closing");
  }
  else {
    set_open(false);
  }
}

}

