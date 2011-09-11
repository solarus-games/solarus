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
#include "lua/ItemScript.h"
#include "entities/PickableItem.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/Movement.h"
#include "ItemProperties.h"
#include "Equipment.h"
#include "Game.h"
#include "Map.h"
#include "InventoryItem.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
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
 * This function should be called only when there is current pickable item,
 * e.g. from event_appear() or event_movement_changed().
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

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.get_sprite(): there is no current pickable item");

  int handle = script->create_sprite_handle(pickable_item->get_sprite());
  lua_pushinteger(l, handle);

  return 1;
}

/**
 * @brief Makes the movement of the current pickable item accessible from the script.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from event_appear() or event_movement_changed().
 * - Return value (movement): the movement of the current pickable item
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

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.get_movement(): there is no current pickable item");

  Movement *movement = pickable_item->get_movement();
  int handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, handle);

  return 1;
}

/**
 * @brief Sets a movement to the pickable item that just appeared.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from the event_appear() function.
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

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.start_movement(): there is no current pickable item");

  pickable_item->clear_movement();
  pickable_item->set_movement(&movement);

  return 0;
}

/**
 * @brief Returns whether the pickable item is following an entity
 * such as the boomerang or the hookshot.
 *
 * When this function returns true, the movement of the pickable item
 * is an instance of FollowMovement.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from the event_appear() function.
 *
 * - Return value (boolean): true if the pickable item is currently following an entity
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_is_following_entity(lua_State* l) {

  Script *script;
  called_by_script(l, 0, &script);

  // retrieve the pickable item
  const std::string& item_name = script->get_item_properties().get_name();
  Equipment& equipment = script->get_game().get_equipment();
  ItemScript& item_script = equipment.get_item_script(item_name);
  PickableItem* pickable_item = item_script.get_pickable_item();

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.is_following_entity(): there is no current pickable item");

  bool result = pickable_item->get_entity_followed() != NULL;
  lua_pushinteger(l, result);

  return 1;
}

/**
 * @brief Returns the position of the pickable item that just appeared.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from the event_appear() function.
 *
 * - Return value 1 (integer): x coordinate of the pickable item
 * - Return value 2 (integer): y coordinate of the pickable item
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_get_position(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.get_position(): there is no current pickable item");

  const Rectangle& xy = pickable_item->get_xy();
  lua_pushinteger(l, xy.get_x());
  lua_pushinteger(l, xy.get_y());

  return 2;
}

/**
 * @brief Sets the position of the pickable item that just appeared.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from the event_appear() function.
 *
 * - Argument 1 (integer): x coordinate of the pickable item
 * - Argument 2 (integer): y coordinate of the pickable item
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_set_position(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);

  int x = luaL_checkinteger(l, 1);
  int y = luaL_checkinteger(l, 2);

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.set_position(): there is no current pickable item");

  pickable_item->set_xy(x, y);

  return 0;
}

/**
 * @brief Returns the layer of the pickable item that just appeared.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from the event_appear() function.
 *
 * - Return value (integer): layer of the pickable item (0 to 2)
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_get_layer(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.get_layer(): there is no current pickable item");

  lua_pushinteger(l, pickable_item->get_layer());

  return 1;
}

/**
 * @brief Sets the layer of the pickable item that just appeared.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from the event_appear() function.
 *
 * - Argument 1 (integer): layer of the pickable item
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_set_layer(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  int layer = luaL_checkinteger(l, 1);

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.set_layer(): there is no current pickable item");

  MapEntities &entities = script->get_map().get_entities();
  entities.set_entity_layer(pickable_item, Layer(layer));

  return 0;
}

/**
 * @brief Sets whether the pickable item detects collisions on every layer.
 *
 * This function should be called only when there is a current pickable item,
 * e.g. from the event_appear() function.
 *
 * - Argument 1 (boolean): true to detect collisions from every layer (default: false)
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_set_layer_independent_collisions(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);

  bool independent = lua_toboolean(l, 1) != 0;

  // retrieve the pickable item
  const std::string &item_name = script->get_item_properties().get_name();
  Equipment &equipment = script->get_game().get_equipment();
  ItemScript &item_script = equipment.get_item_script(item_name);
  PickableItem *pickable_item = item_script.get_pickable_item();

  Debug::check_assertion(pickable_item != NULL,
                "Cannot call sol.item.set_layer_independent_collisions(): there is no current pickable item");

  pickable_item->set_layer_independent_collisions(independent);

  return 0;
}

/**
 * @brief Indicates that the player has finished using the current inventory item.
 *
 * This function should be called only when there is a current inventory item that is being used,
 * i.e. after the event_use() function.
 *
 * @param l the Lua context that is calling this function
 */
int Script::item_api_set_finished(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  // retrieve the inventory item from the hero
  Hero &hero = script->get_game().get_hero();
  const std::string &item_name = script->get_item_properties().get_name();

  if (hero.is_using_inventory_item()) { // we do nothing if the script has already changed the hero's state

    InventoryItem &inventory_item = hero.get_current_inventory_item();
    Debug::check_assertion(inventory_item.get_name() == item_name,
                  StringConcat() << "This script controls the item '" << item_name
                  << "' but the current inventory item is '" << inventory_item.get_name() << "'");

    inventory_item.set_finished();
  }

  return 0;
}
