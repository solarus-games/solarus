/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_MAP_SCRIPT_H
#define ZSDX_MAP_SCRIPT_H

#include "Common.h"
#include "Treasure.h"
#include "InventoryItemId.h"
#include <list>

struct lua_State;

/**
 * Represents the Lua script of a map.
 * This class makes the interface between the engine C++ code and the map Lua script.
 */
class MapScript {

 private:

  typedef int (FunctionAvailableToScript) (lua_State *l); /**< functions that can be called by the Lua script */

  Map *map;                   /**< the map associated to this script */
  lua_State* context;         /**< the execution context of the Lua script */
  std::list<Timer*> timers;   /**< the timers currently running for this script */
  bool just_freezed;          /**< indicates that this script has just freezed the hero */

  bool call_lua_function(const std::string &function_name);
  bool call_lua_function(const std::string &function_name, const std::string &arg1);
  bool call_lua_function(const std::string &function_name, const std::string &arg1, int arg2);
  bool call_lua_function(const std::string &function_name, const std::string &arg1, int arg2, int arg3);
  bool call_lua_function(const std::string &function_name, int arg1);
  bool call_lua_function(const std::string &function_name, int arg1, int arg2);
  bool call_lua_function(const std::string &function_name, bool arg1);

  // C++ functions that can be called by the script
  static FunctionAvailableToScript l_freeze,
    l_unfreeze,
    l_play_sound,
    l_play_music,
    l_start_message,
    l_set_message_variable,
    l_give_treasure,
    l_give_treasure_with_amount,
    l_savegame_get_string,
    l_savegame_get_integer,
    l_savegame_get_boolean,
    l_savegame_set_string,
    l_savegame_set_integer,
    l_savegame_set_boolean,
    l_get_player_name,
    l_start_timer,
    l_stop_timer,
    l_move_camera,
    l_restore_camera,
    l_npc_get_position,
    l_npc_set_position,
    l_npc_walk,
    l_npc_random_walk,
    l_npc_jump,
    l_npc_set_animation,
    l_npc_set_animation_ignore_suspend,
    l_npc_set_direction,
    l_npc_remove,
    l_hero_set_direction,
    l_hero_align_on_sensor,
    l_hero_set_map,
    l_hero_walk,
    l_hero_start_victory_sequence,
    l_chest_set_open,
    l_chest_set_hidden,
    l_chest_is_hidden,
    l_get_rupees,
    l_remove_rupees,
    l_inventory_item_get,
    l_inventory_item_set,
    l_inventory_item_get_amount,
    l_inventory_item_remove_amount,
    l_inventory_item_is_bottle,
    l_tile_is_enabled,
    l_tile_set_enabled,
    l_tiles_set_enabled,
    l_reset_block,
    l_reset_blocks,
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
    l_interactive_entity_remove,
    l_equipment_get_tunic,
    l_equipment_get_sword,
    l_equipment_get_shield,
    l_shop_item_remove,
    l_switch_set_enabled,
    l_enemy_is_dead,
    l_enemies_are_dead,
    l_enemy_set_enabled,
    l_boss_start_battle,
    l_boss_end_battle,
    l_sensor_remove,
    l_door_open,
    l_door_close,
    l_door_is_open,
    l_door_set_open;

  static void check_nb_arguments(lua_State *context, int nb_arguments);

  void load(void);
  void register_c_functions(void);
  void add_timer(Timer *timer);
  void remove_timer(const std::string &callback_name);

 public:

  // loading and closing a script
  MapScript(Map *map);
  ~MapScript(void);
  void initialize(const std::string &destination_point_name);

  // update functions
  void update(void);
  void set_suspended(bool suspended);

  // C++ functions that call script functions
  void event_map_started(const std::string &destination_point_name);
  void event_opening_transition_finished(void);
  void event_message_started(const MessageId &message_id);
  void event_message_sequence_finished(const MessageId &first_message_id, int answer);
  void event_switch_enabled(const std::string &switch_name);
  void event_switch_disabled(const std::string &switch_name);
  void event_hero_on_sensor(const std::string &sensor_name);
  void event_hero_victory_sequence_finished(void);
  void event_camera_reached_target(void);
  void event_interaction(const std::string &entity_name);
  bool event_interaction_item(const std::string &entity_name, InventoryItemId item_id, int variant);
  void event_npc_dialog(const std::string &npc_name);
  void event_npc_movement_finished(const std::string &npc_name);
  bool event_open_empty_chest(const std::string &chest_name);
  void event_obtaining_treasure(Treasure::Content content, int savegame_variable);
  void event_obtained_treasure(Treasure::Content content, int savegame_variable);
  void event_shop_item_bought(const std::string &shop_item_name);
  void event_enemy_dead(const std::string &enemy_name);
  void event_dungeon_ending_sequence(void);
};

#endif

