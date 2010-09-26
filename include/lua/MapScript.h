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
#ifndef SOLARUS_MAP_SCRIPT_H
#define SOLARUS_MAP_SCRIPT_H

#include "Common.h"
#include "lua/GameScript.h"

/**
 * @brief Represents the Lua script of a map.
 *
 * This class makes the interface between the engine C++ code and the Lua script of a map.
 */
class MapScript: public GameScript {

  protected:

    Map &map;			/**< the map controlled by this script */
    Hero &hero;			/**< the hero */

    // calling a C++ function from the script
    static void called_by_script(lua_State *context, int nb_arguments, MapScript **map_script);

    static FunctionAvailableToScript 
      l_hero_freeze,
      l_hero_unfreeze,
      l_dialog_start,
      l_dialog_set_variable,
      l_dialog_set_style,
      l_hud_set_enabled,
      l_hud_set_pause_enabled,
      l_camera_move,
      l_camera_restore,
      l_treasure_give,
      l_hero_set_map,
      l_hero_set_direction,
      l_hero_align_on_sensor,
      l_hero_walk,
      l_hero_jump,
      l_hero_start_victory_sequence,
      l_hero_set_visible,
      l_npc_get_position,
      l_npc_set_position,
      l_npc_walk,
      l_npc_random_walk,
      l_npc_jump,
      l_npc_set_animation,
      l_npc_set_animation_ignore_suspend,
      l_npc_set_direction,
      l_npc_remove,
      l_chest_set_open,
      l_chest_set_hidden,
      l_chest_is_hidden,
      l_tile_is_enabled,
      l_tile_set_enabled,
      l_tile_set_group_enabled,
      l_block_reset,
      l_block_reset_all,
      l_interactive_entity_get_animation,
      l_interactive_entity_get_animation_delay,
      l_interactive_entity_get_animation_frame,
      l_interactive_entity_get_direction,
      l_interactive_entity_is_animation_paused,
      l_interactive_entity_set_animation,
      l_interactive_entity_set_animation_delay,
      l_interactive_entity_set_animation_frame,
      l_interactive_entity_set_direction,
      l_interactive_entity_set_animation_paused,
      l_interactive_entity_set_animation_ignore_suspend,
      l_interactive_entity_fade,
      l_interactive_entity_remove,
      l_shop_item_remove,
      l_switch_is_enabled,
      l_switch_set_enabled,
      l_switch_set_locked,
      l_enemy_is_dead,
      l_enemy_is_group_dead,
      l_enemy_set_enabled,
      l_enemy_start_boss,
      l_enemy_end_boss,
      l_enemy_start_miniboss,
      l_enemy_end_miniboss,
      l_sensor_remove,
      l_door_open,
      l_door_close,
      l_door_is_open,
      l_door_set_open;

    // initialization
    void register_available_functions();

  public:

    // loading and closing a script
    MapScript(Scripts &scripts, Map &map);
    virtual ~MapScript();
    void start(const std::string &destination_point_name);

};

#endif

