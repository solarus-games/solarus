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
#include "Game.h"
#include "ItemProperties.h"
#include <lua.hpp>

/**
 * @brief Constructor.
 * @param game the game
 * @param item_properties the static properties of the item
 */
ItemScript::ItemScript(Game &game, ItemProperties &item_properties):
  MapScript(game.get_current_map()),
  game(game),
  item_properties(item_properties) {

  std::string script_name = (std::string) "items" + item_properties.get_name();
  load_if_exists(script_name);
}

/**
 * @brief Destructor.
 */
ItemScript::~ItemScript() {

}

/**
 * @brief Tells the Lua context what C++ functions it can call.
 */
void ItemScript::register_available_functions() {

  // functions available to all map scripts
  MapScript::register_available_functions();

  // functions specific to item scripts
  static luaL_Reg functions[] = {
      { "get_amount", l_get_amount },
      { "set_amount", l_set_amount },
      { "add_amount", l_add_amount },
      { "remove_amount", l_remove_amount },
      { "start_movement", l_start_movement },
      { "set_finished", l_set_finished },
      { NULL, NULL }
  };

  luaL_register(context, "sol.item", functions);
}

/**
 * @brief Checks the number of arguments provided to a C++ function called by the Lua script.
 *
 * In any C++ function called by the Lua script (i.e. a function prefixed by "l_"),
 * the first instruction calls this function.
 * It checks the number of arguments provided to the C++ function called by the Lua script
 * and retrieves the current MapScript object.
 *
 * @param context the Lua context
 * @param nb_arguments the number of arguments to check (if it is incorrect, the program stops)
 * @param map_script if not NULL, a pointer to the MapScript object will be copied there so that the static C++ function
 * called by the Lua script can access it
 */
void ItemScript::called_by_script(lua_State *context, int nb_arguments, ItemScript **item_script) {

  Script *script;
  Script::called_by_script(context, nb_arguments, &script);
  *item_script = (ItemScript*) script;
}

// TODO
int ItemScript::l_get_amount(lua_State *l) {

  return 0;
}

int ItemScript::l_set_amount(lua_State *l) {

  return 0;
}

int ItemScript::l_add_amount(lua_State *l) {

  return 0;
}

int ItemScript::l_remove_amount(lua_State *l) {

  return 0;
}

int ItemScript::l_start_movement(lua_State *l) {

  return 0;
}

int ItemScript::l_set_finished(lua_State *l) {

  return 0;
}

void ItemScript::event_appear(PickableItem &pickable_item) {

}

void ItemScript::event_obtaining(const Treasure &treasure) {

}

void ItemScript::event_obtained(const Treasure &treasure) {

}

void ItemScript::event_use(InventoryItem &inventory_item) {

}

