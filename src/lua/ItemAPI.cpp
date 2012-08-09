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
#include "lua/LuaContext.h"
#include "entities/PickableItem.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "movements/Movement.h"
#include "EquipmentItem.h"
#include "Equipment.h"
#include "Game.h"
#include "Map.h"
#include "InventoryItem.h"
#include "Sprite.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

const std::string Script::item_module_name = "sol.item";

/**
 * @brief Initializes the item features provided to Lua.
 */
void Script::register_item_module() {

  static const luaL_Reg methods[] = {
      { "get_game", item_api_get_game },
      { "get_map", item_api_get_map },
      { "get_variant", item_api_get_variant },
      { "set_variant", item_api_set_variant },
      { "get_amount", item_api_get_amount },
      { "set_amount", item_api_set_amount },
      { "add_amount", item_api_add_amount },
      { "remove_amount", item_api_remove_amount },
      { "get_sprite", item_api_get_sprite },
      { "get_movement", item_api_get_movement },
      { "is_following_entity", item_api_is_following_entity },
      { "start_movement", item_api_start_movement },
      { "get_position", item_api_get_position },
      { "set_position", item_api_set_position },
      { "set_layer_independent_collisions", item_api_set_layer_independent_collisions },
      { "set_finished", item_api_set_finished },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__eq", userdata_meta_eq },
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };
  register_type(item_module_name, methods, metamethods);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is an
 * equipment item and returns it.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return The equipment item.
 */
EquipmentItem& Script::check_item(lua_State* l, int index) {
  return static_cast<EquipmentItem&>(check_userdata(l, index, item_module_name));
}

/**
 * @brief Pushes an equipment item userdata onto the stack.
 * @param l A Lua context.
 * @param item An item.
 */
void Script::push_item(lua_State* l, EquipmentItem& item) {

  push_userdata(l, item);
}

/**
 * @brief Implementation of \ref lua_api_item_get_game.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_get_game(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  push_game(l, item.get_equipment().get_savegame());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_item_get_map.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_get_map(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  Game* game = item.get_game();
  if (game != NULL) {
    push_map(l, game->get_current_map());
  }
  else {
    lua_pushnil(l);
  }
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_get_variant(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  lua_pushinteger(l, item.get_current_variant());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_set_variant(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);
  int variant = luaL_checkinteger(l, 2);

  item.set_current_variant(variant);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_get_amount(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  lua_pushinteger(l, item.get_current_amount());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_set_amount(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);
  int amount = luaL_checkinteger(l, 2);
  item.set_current_amount(amount);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_add_amount(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);
  int amount = luaL_checkinteger(l, 2);

  item.set_current_amount(item.get_current_amount() + amount);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_remove_amount(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);
  int amount = luaL_checkinteger(l, 2);

  item.set_current_amount(item.get_current_amount() - amount);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_get_sprite(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  PickableItem* pickable_item = item.get_pickable_item();
  Debug::check_assertion(pickable_item != NULL,
                "Cannot call item:get_sprite(): there is no current pickable item");

  push_sprite(l, pickable_item->get_sprite());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_get_movement(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  PickableItem* pickable_item = item.get_pickable_item();
  Debug::check_assertion(pickable_item != NULL,
                "Cannot call item:get_movement(): there is no current pickable item");
  Movement* movement = pickable_item->get_movement();

  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_start_movement(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);
  Movement& movement = check_movement(l, 2);

  movement.set_suspended(false);
  PickableItem* pickable_item = item.get_pickable_item();
  Debug::check_assertion(pickable_item != NULL,
                "Cannot call item:start_movement(): there is no current pickable item");
  get_script(l).increment_refcount(&movement);
  pickable_item->clear_movement();
  pickable_item->set_movement(&movement);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_is_following_entity(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  PickableItem* pickable_item = item.get_pickable_item();
  Debug::check_assertion(pickable_item != NULL,
                "Cannot call item:is_following_entity(): there is no current pickable item");
  bool result = pickable_item->get_entity_followed() != NULL;

  lua_pushboolean(l, result);
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_get_position(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  PickableItem* pickable_item = item.get_pickable_item();
  Debug::check_assertion(pickable_item != NULL,
      "Cannot call item:get_position(): there is no current pickable item");
  const Rectangle& xy = pickable_item->get_xy();

  lua_pushinteger(l, xy.get_x());
  lua_pushinteger(l, xy.get_y());
  lua_pushinteger(l, pickable_item->get_layer());
  return 3;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_set_position(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);
  int x = luaL_checkinteger(l, 2);
  int y = luaL_checkinteger(l, 3);
  int layer = -1;
  if (lua_gettop(l) >= 4) {
    layer = luaL_checkinteger(l, 4);
  }

  PickableItem* pickable_item = item.get_pickable_item();
  Debug::check_assertion(pickable_item != NULL,
                "Cannot call item:set_position(): there is no current pickable item");
  pickable_item->set_xy(x, y);
  if (layer != -1) {
    MapEntities& entities = pickable_item->get_map().get_entities();
    entities.set_entity_layer(*pickable_item, Layer(layer));
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_set_layer_independent_collisions(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);
  bool independent = lua_toboolean(l, 2) != 0;

  PickableItem* pickable_item = item.get_pickable_item();
  Debug::check_assertion(pickable_item != NULL,
                "Cannot call item:set_layer_independent_collisions(): there is no current pickable item");
  pickable_item->set_layer_independent_collisions(independent);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_item_.
 * @param l The Lua context that is calling this function.
 * @return Number of values to return to Lua.
 */
int Script::item_api_set_finished(lua_State* l) {

  EquipmentItem& item = check_item(l, 1);

  // retrieve the inventory item from the hero
  Hero& hero = item.get_game()->get_hero();
  if (hero.is_using_inventory_item()) {  // Do nothing if the script has already changed the hero's state.

    InventoryItem& inventory_item = hero.get_current_inventory_item();
    Debug::check_assertion(inventory_item.get_name() == item.get_name(),
        StringConcat() << "Trying to finish inventory item '" << item.get_name()
        << "' but the current inventory item is '" << inventory_item.get_name() << "'");
    inventory_item.set_finished();
  }

  return 0;
}

/**
 * @brief Calls the on_update() method of a Lua equipment item.
 * @param item An equipment item.
 */
void LuaContext::item_on_update(EquipmentItem& item) {

  push_item(l, item);
  on_update();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_suspended() method of a Lua equipment item.
 * @param item An equipment item.
 * @param suspended true if the game is suspended.
 */
void LuaContext::item_on_suspended(EquipmentItem& item, bool suspended) {

  push_item(l, item);
  on_suspended(suspended);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_map_changed() method of a Lua equipment item.
 * @param item An equipment item.
 * @param map A map.
 */
void LuaContext::item_on_map_changed(EquipmentItem& item, Map& map) {

  push_item(l, item);
  on_map_changed(map);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_appear() method of a Lua equipment item.
 * @param item An equipment item.
 * @param pickable The instance of pickable item that has just appeared.
 */
void LuaContext::item_on_appear(EquipmentItem& item, PickableItem& pickable) {

  push_item(l, item);
  on_appear(pickable);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_movement_changed() method of a Lua equipment item.
 * @param item An equipment item.
 * @param pickable The instance of pickable item whose movement has changed.
 */
void LuaContext::item_on_movement_changed(EquipmentItem& item, PickableItem& pickable) {

  push_item(l, item);
  on_movement_changed(pickable);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_obtaining() method of a Lua equipment item.
 * @param item An equipment item.
 * @param treasure The treasure being obtained.
 */
void LuaContext::item_on_obtaining(EquipmentItem& item, const Treasure& treasure) {

  push_item(l, item);
  on_obtaining(treasure);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_obtained() method of a Lua equipment item.
 * @param item An equipment item.
 * @param treasure The treasure just obtained.
 */
void LuaContext::item_on_obtained(EquipmentItem& item, const Treasure& treasure) {

  push_item(l, item);
  on_obtained(treasure);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_variant_changed() method of a Lua equipment item.
 * @param item An equipment item.
 * @param variant The possession state.
 */
void LuaContext::item_on_variant_changed(EquipmentItem& item, int variant) {

  push_item(l, item);
  on_variant_changed(variant);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_amount_changed() method of a Lua equipment item.
 * @param item An equipment item.
 * @param amount The amount of this item.
 */
void LuaContext::item_on_amount_changed(EquipmentItem& item, int amount) {

  push_item(l, item);
  on_amount_changed(amount);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_use() method of a Lua equipment item.
 * @param item An equipment item.
 * @param amount The amount of this item.
 */
void LuaContext::item_on_use(EquipmentItem& item, InventoryItem& inventory_item) {

  push_item(l, item);
  on_use(inventory_item);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_ability_used() method of a Lua equipment item.
 * @param item An equipment item.
 * @param ability_name The ability just used.
 */
void LuaContext::item_on_ability_used(EquipmentItem& item, const std::string& ability_name) {

  push_item(l, item);
  on_ability_used(ability_name);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_npc_interaction() method of a Lua equipment item.
 * @param item An equipment item.
 * @param npc An NPC.
 */
void LuaContext::item_on_npc_interaction(EquipmentItem& item, NPC& npc) {

  push_item(l, item);
  on_npc_interaction(npc);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_npc_interaction_item() method of a Lua equipment item.
 * @param item An equipment item.
 * @param npc An NPC.
 * @param item_name Name of an equipment item.
 * @param variant Variant of this equipment item.
 * @return true if an interaction occurred.
 */
bool LuaContext::item_on_npc_interaction_item(EquipmentItem& item, NPC& npc,
    const std::string& item_name, int variant) {

  push_item(l, item);
  bool result = on_npc_interaction_item(npc, item_name, variant);
  lua_pop(l, 1);
  return result;
}

/**
 * @brief Calls the on_npc_collision_fire() method of a Lua equipment item.
 * @param item An equipment item.
 * @param npc An NPC.
 */
void LuaContext::item_on_npc_collision_fire(EquipmentItem& item, NPC& npc) {

  push_item(l, item);
  on_npc_collision_fire(npc);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_dialog_started() method of a Lua equipment item.
 * @param item An equipment item.
 * @param dialog_id Id a the dialog just started.
 */
void LuaContext::item_on_dialog_started(EquipmentItem& item, const std::string& dialog_id) {

  push_item(l, item);
  on_dialog_started(dialog_id);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_dialog_finished() method of a Lua equipment item.
 * @param item An equipment item.
 * @param dialog_id Id a the dialog just finished.
 * @param answer The answer selected by the player: 0 for the first one,
 * 1 for the second one, -1 if there was no question.
 */
void LuaContext::item_on_dialog_finished(EquipmentItem& item,
    const std::string& dialog_id, int answer) {

  push_item(l, item);
  on_dialog_finished(dialog_id, answer);
  lua_pop(l, 1);
}
