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
#include "entities/Chest.h"
#include "entities/Hero.h"
#include "KeysEffect.h"
#include "Game.h"
#include "DialogBox.h"
#include "Savegame.h"
#include "Sprite.h"
#include "Savegame.h"
#include "Equipment.h"
#include "Map.h"
#include "Dungeon.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "lua/LuaContext.h"
#include <sstream>

/**
 * @brief Creates a new chest with the specified treasure.
 * @param name name identifying this chest
 * @param layer layer of the chest to create on the map
 * @param x x coordinate of the chest to create
 * @param y y coordinate of the chest to create
 * @param big_chest true to make a big chest, false to make a normal chest
 * @param treasure the treasure in the chest
 */
Chest::Chest(const std::string &name, Layer layer, int x, int y,
	     bool big_chest, const Treasure& treasure):

  Detector(COLLISION_FACING_POINT, name, layer, x, y, 16, 16),
  treasure(treasure),
  big_chest(big_chest),
  open(treasure.is_found()),
  treasure_given(open),
  treasure_date(0) {

  initialize_sprite();
}

/**
 * @brief Destructor.
 */
Chest::~Chest() {
}

/**
 * @brief Returns the type of entity.
 * @return the type of entity
 */
EntityType Chest::get_type() {
  return CHEST;
}

/**
 * @brief Returns whether this entity has to be drawn in y order.
 * @return \c true if this type of entity should be drawn at the same level
 * as the hero.
 */
bool Chest::is_drawn_in_y_order() {
  return big_chest;
}

/**
 * @brief Creates the chest sprite depending on its size and the savegame.
 */
void Chest::initialize_sprite() {

  // create the sprite
  create_sprite("entities/chest");
  Sprite &sprite = get_sprite();

  // set its animation
  std::string animation = big_chest ? "big_" : "small_";
  animation += is_open() ? "open" : "closed";
  sprite.set_current_animation(animation);

  // set the entity size
  if (big_chest) {
    set_origin(0, -8);
    set_size(32, 16);
  }
  else {
    set_size(16, 16);
  }
}

/**
 * @brief Notifies this entity that it was just enabled or disabled.
 * @param enabled \c true if the entity is now enabled.
 */
void Chest::notify_enabled(bool enabled) {

  // Make sure the chest does not appear on the hero.
  if (enabled && overlaps(get_hero())) {
    get_hero().avoid_collision(*this, 3);
  }
}

/**
 * @brief Returns whether the player has found the treasure in this chest.
 * @return true if the chest is open
 */
bool Chest::is_open() {
  return open;
}

/**
 * @brief Sets whether the chest is open.
 *
 * If you don't change the chest state, this function has no effect.
 * If you make the chest opened, its sprite is updated but this function does not give any treasure
 * to the player.
 * If you close the chest, its sprite is updated and the chest will then be empty.
 *
 * @param open true to open the chest, false to close it
 */
void Chest::set_open(bool open) {

  if (open != this->open) {

    this->open = open;

    if (open) {
      // open the chest
      get_sprite().set_current_animation(big_chest ? "big_open" : "small_open");
    }
    else {
      // close the chest
      get_sprite().set_current_animation(big_chest ? "big_closed" : "small_closed");
      treasure_given = false;
    }
  }
}

/**
 * @brief Returns whether the player is able to open this chest now.
 * @return true if this is a small chest or if the player has the big key.
 */
bool Chest::can_open() {

  if (!big_chest) {
    return true;
  }

  // We are in a dungeon.
  std::ostringstream oss;
  oss << "dungeon_" << get_game().get_current_dungeon().get_number();
  const std::string& dungeon_number = oss.str();

  return get_savegame().get_boolean(dungeon_number + "_big_key");
}

/**
 * @brief Returns whether this entity is an obstacle for another one when
 * it is enabled.
 * @param other Another entity.
 * @return \c true if this entity is an obstacle for the other one.
 */
bool Chest::is_obstacle_for(MapEntity& other) {
  return true;
}

/**
 * @brief This function is called by the engine when an entity overlaps the chest.
 * @param entity_overlapping the entity overlapping the detector
 * @param collision_mode the collision mode that detected the collision
 */
void Chest::notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) {

  if (!is_suspended()) {
    entity_overlapping.notify_collision_with_chest(*this);
  }
}

/**
 * @brief Updates the chest.
 *
 * This function is called repeatedly by the map.
 * This is a redefinition of MapEntity::update()
 * the handle the chest opening.
 */
void Chest::update() {

  if (is_open() && !suspended) {

    if (!treasure_given && treasure_date != 0 && System::now() >= treasure_date) {

      treasure_date = 0;

      if (treasure.get_item_name() != "_none") {
        // give a treasure to the player

        get_hero().start_treasure(treasure);
        treasure_given = true;
      }
      else { // the chest is empty

        // mark the treasure as found in the savegame
        if (treasure.is_saved()) {
          get_savegame().set_boolean(treasure.get_savegame_variable(), true);
        }

        treasure_given = true;

        bool done = get_lua_context().chest_on_empty(*this);
        if (!done) {

          // the script does not define any behavior:
          // by default, we tell the player the chest is empty
          Sound::play("wrong");
          get_dialog_box().start_dialog("_empty_chest");
          get_hero().start_free();
        }
      }
    }
  }

  MapEntity::update();
}

/**
 * @brief This function is called when the player interacts with this chest.
 *
 * This function is called when the player presses the action key
 * when the hero is facing this detector, and the action icon lets him do this.
 * The hero opens the chest if possible.
 */
void Chest::action_key_pressed() {

  if (is_enabled() && get_hero().is_free()) {

    if (can_open()) {
      Sound::play("chest_open");
      set_open(true);
      treasure_date = System::now() + 300;

      get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);
      get_hero().start_freezed();
    }
    else {
      Sound::play("wrong");
      get_dialog_box().start_dialog("_big_key_required");
    }
  }
}

/**
 * @brief This function is called by the map when the game is suspended or resumed.
 *
 * This is a redefinition of MapEntity::set_suspended() to suspend the timer
 * of the chest being opened.
 *
 * @param suspended true to suspend the entity, false to resume it
 */
void Chest::set_suspended(bool suspended) {

  MapEntity::set_suspended(suspended);

  if (!suspended && treasure_date != 0) {
    // restore the timer
    treasure_date = System::now() + (treasure_date - when_suspended);
  }
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return The name identifying this type in Lua.
 */
const std::string& Chest::get_lua_type_name() const {
  return LuaContext::entity_chest_module_name;
}

