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
#include "lua/Script.h"
#include "lua/ItemScript.h"
#include "entities/PickableItem.h"
#include "movements/Movement.h"
#include "ItemProperties.h"
#include "Equipment.h"
#include "Game.h"
#include "InventoryItem.h"
#include "lowlevel/Debug.h"
#include <lua.hpp>

/**
 * @brief Returns the possession state of the current item
 * (only for an item whose possession state is saved).
 *
 * - Return value: the possession state
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_get_variant(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  ItemProperties &properties = script->get_item_properties();
  Equipment &equipment = script->get_game().get_equipment();
  int variant = equipment.get_item_variant(properties.get_name());
  lua_pushinteger(l, variant);

  return 1;
}

/**
 * @brief Sets the possession state of the current item
 * (only for an item whose possession state is saved).
 *
 * - Argument 1 (integer): the new possession state of this item
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_set_variant(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int variant = luaL_checkinteger(l, 1);
  ItemProperties &properties = script->get_item_properties();
  Equipment &equipment = script->get_game().get_equipment();
  equipment.set_item_variant(properties.get_name(), variant);

  return 0;
}

/**
 * @brief Returns the amount of the current item
 * (only for an item with amount).
 *
 * - Return value: the amount
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_get_amount(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  ItemProperties &properties = script->get_item_properties();
  Equipment &equipment = script->get_game().get_equipment();
  int amount = equipment.get_item_amount(properties.get_name());
  lua_pushinteger(l, amount);

  return 1;
}

/**
 * @brief Sets the amount of the current item
 * (only for an item with amount).
 *
 * - Argument 1 (integer): the new amount for this item
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_set_amount(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int amount = luaL_checkinteger(l, 1);
  ItemProperties &properties = script->get_item_properties();
  Equipment &equipment = script->get_game().get_equipment();
  equipment.set_item_amount(properties.get_name(), amount);

  return 0;
}

/**
 * @brief Increases the amount of the current item
 * (only for an item with amount).
 *
 * - Argument 1 (integer): the amount to add
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_add_amount(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int amount = luaL_checkinteger(l, 1);
  ItemProperties &properties = script->get_item_properties();
  Equipment &equipment = script->get_game().get_equipment();
  equipment.add_item_amount(properties.get_name(), amount);

  return 0;
}

/**
 * @brief Decreases the amount of the current item
 * (only for an item with amount).
 *
 * - Argument 1 (integer): the amount to remove
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_remove_amount(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int amount = luaL_checkinteger(l, 1);
  ItemProperties &properties = script->get_item_properties();
  Equipment &equipment = script->get_game().get_equipment();
  equipment.remove_item_amount(properties.get_name(), amount);

  return 0;
}

/**
 * @brief Makes the sprite of the current pickable item accessible from the script.
 *
 * This function should be called only when there a current pickable item,
 * i.e. from event_appear() or event_movement_changed().
 * - Return value (sprite): the sprite of the current pickable item
 * (your script can then pass it as a parameter
 * to all sol.main.sprite_* functions)
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_get_sprite(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::assert(pickable_item != NULL,
                "Cannot call sol.item.get_sprite(): there is no current pickable item");

  int handle = script->create_sprite_handle(pickable_item->get_sprite());
  lua_pushinteger(l, handle);

  return 1;
}

/**
 * @brief Makes the movement of the current pickable item accessible from the script.
 *
 * This function should be called only when there a current pickable item,
 * i.e. from event_appear() or event_movement_changed().
 * - Return value (mvoement): the movement of the current pickable item
 * (your script can then pass it as a parameter
 * to all sol.main.movement_* functions)
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_get_movement(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::assert(pickable_item != NULL,
                "Cannot call sol.item.get_movement(): there is no current pickable item");

  Movement *movement = pickable_item->get_movement();
  int handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, handle);

  return 1;
}

/**
 * @brief Sets a movement to the pickable item that just appeared.
 *
 * This function should be called only when there a current pickable item,
 * i.e. from the event_appear() function.
 * The default movement of the pickable item (FallingOnFloorMovement)
 * will be replaced to the one you specify.
 *
 * - Argument 1 (movement): the movement to set
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_start_movement(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  // retrieve the movement
  int movement_handle = luaL_checkinteger(l, 1);
  Movement &movement = script->start_movement(movement_handle);

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::assert(pickable_item != NULL,
                "Cannot call sol.item.start_movement(): there is no current pickable item");

  pickable_item->clear_movement();
  pickable_item->set_movement(&movement);

  return 0;
}

/**
 * @brief Indicates that the player has finished using the current inventory item.
 *
 * This function should be called only when there a current inventory item that is being used,
 * i.e. after the event_use() function.
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_set_finished(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  // retrieve the inventory item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  InventoryItem *inventory_item = item_script.get_inventory_item();

  Debug::assert(inventory_item != NULL,
                "Cannot call sol.item.set_finished(): there is no current inventory item being used");

  inventory_item->set_finished();

  return 0;
}
