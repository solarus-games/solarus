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
#ifndef SOLARUS_GAME_SCRIPT_H
#define SOLARUS_GAME_SCRIPT_H

#include "Common.h"
#include "lua/Script.h"

/**
 * @brief Represents an in-game Lua script.
 */
class GameScript: public Script {

  protected:

    Game &game;					/**< the game controlled by this script */

    // calling a C++ function from the script
    static void called_by_script(lua_State *context, int nb_arguments, GameScript **game_script);

    static FunctionAvailableToScript 
      l_savegame_get_string,
      l_savegame_get_integer,
      l_savegame_get_boolean,
      l_savegame_set_string,
      l_savegame_set_integer,
      l_savegame_set_boolean,
      l_savegame_get_name,
      l_equipment_get_life,
      l_equipment_add_life,
      l_equipment_remove_life,
      l_equipment_get_money,
      l_equipment_add_money,
      l_equipment_remove_money,
      l_equipment_has_ability,
      l_equipment_get_ability,
      l_equipment_set_ability,
      l_equipment_has_item,
      l_equipment_get_item,
      l_equipment_set_item,
      l_equipment_has_item_amount,
      l_equipment_get_item_amount,
      l_equipment_add_item_amount,
      l_equipment_remove_item_amount,
      l_equipment_is_dungeon_finished,
      l_equipment_set_dungeon_finished;

    // initialization
    void register_available_functions();
    virtual bool is_new_timer_suspended(void);

  public:

    // loading and closing a script
    GameScript(Scripts &scripts, Game &game);
    virtual ~GameScript();
};

#endif

