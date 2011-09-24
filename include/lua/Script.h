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
#ifndef SOLARUS_SCRIPT_H
#define SOLARUS_SCRIPT_H

#include "Common.h"
#include <list>
#include <map>

struct lua_State;

/**
 * @brief Handles a Lua script that is running.
 *
 * This class and its subclasses provide an API that allows Lua scripts to call C++ functions.
 */
class Script {

  protected:

    /**
     * @brief The APIs available depending on the type of scripts.
     */
    enum API {
      MAIN_API          = 0x0001,
      GAME_API          = 0x0002,
      MAP_API           = 0x0004,
      ITEM_API          = 0x0008,
      ENEMY_API         = 0x0010
    };

  private:

    typedef int (FunctionAvailableToScript) (lua_State *l);  /**< type of the functions that can be called by a Lua script */

    // script data
    std::list<Timer*> timers;                            /**< the timers currently running for this script */

    std::map<int, Sprite*> sprites;                      /**< the sprites accessible from this script */
    std::map<int, Sprite*> unassigned_sprites;           /**< the sprites accessible from this script and that
                                                          * are not assigned to an object yet (the script has to delete them) */
    std::map<int, Movement*> movements;                  /**< the movements accessible from this script */
    std::map<int, Movement*> unassigned_movements;       /**< the movements accessible from this script and that
                                                          * are not assigned to an object yet (the script has to delete them) */
    bool music_played;

    // APIs
    uint32_t apis_enabled;                               /**< an OR combination of APIs enabled */

    // calling a Lua function from C++
    bool find_lua_function(const std::string &function_name);

    // calling a C++ function from Lua (and retrieve the instance of Script)
    static Script& get_script(lua_State* context, int min_arguments, int max_arguments = -1);

    // initialization
    void register_apis();
    void register_main_api();
    void register_game_api();
    void register_map_api();
    void register_item_api();
    void register_enemy_api();

    // timers
    void add_timer(Timer *timer);
    void remove_timer(const std::string &callback_name);
    void remove_all_timers();
    bool is_new_timer_suspended(void);

    // debugging
    void print_stack();

    static FunctionAvailableToScript 

      // main API
      main_api_include,
      main_api_play_sound,
      main_api_play_music,
      main_api_timer_start,
      main_api_timer_stop,
      main_api_timer_stop_all,
      main_api_sprite_create,
      main_api_sprite_get_animation,
      main_api_sprite_set_animation,
      main_api_sprite_get_direction,
      main_api_sprite_set_direction,
      main_api_sprite_get_frame,
      main_api_sprite_set_frame,
      main_api_sprite_get_frame_delay,
      main_api_sprite_set_frame_delay,
      main_api_sprite_is_paused,
      main_api_sprite_set_paused,
      main_api_sprite_set_animation_ignore_suspend,
      main_api_sprite_fade,
      main_api_sprite_synchronize,
      main_api_pixel_movement_create,
      main_api_random_movement_create,
      main_api_path_movement_create,
      main_api_random_path_movement_create,
      main_api_path_finding_movement_create,
      main_api_target_movement_create,
      main_api_rectilinear_movement_create,
      main_api_temporal_movement_create,
      main_api_circle_movement_create,
      main_api_jump_movement_create,
      main_api_movement_get_property,
      main_api_movement_set_property,
      main_api_movement_test_obstacles,
      main_api_get_angle,

      // game API
      game_api_save,
      game_api_reset,
      game_api_restart,
      game_api_savegame_get_string,
      game_api_savegame_get_integer,
      game_api_savegame_get_boolean,
      game_api_savegame_set_string,
      game_api_savegame_set_integer,
      game_api_savegame_set_boolean,
      game_api_savegame_get_name,
      game_api_get_life,
      game_api_add_life,
      game_api_remove_life,
      game_api_get_max_life,
      game_api_set_max_life,
      game_api_add_max_life,
      game_api_get_money,
      game_api_add_money,
      game_api_remove_money,
      game_api_get_magic,
      game_api_add_magic,
      game_api_remove_magic,
      game_api_start_decreasing_magic,
      game_api_stop_decreasing_magic,
      game_api_get_max_magic,
      game_api_set_max_magic,
      game_api_has_ability,
      game_api_get_ability,
      game_api_set_ability,
      game_api_has_item,
      game_api_get_item,
      game_api_set_item,
      game_api_has_item_amount,
      game_api_get_item_amount,
      game_api_add_item_amount,
      game_api_remove_item_amount,
      game_api_is_dungeon_finished,
      game_api_set_dungeon_finished,

      // map API
      map_api_hero_freeze,
      map_api_hero_unfreeze,
      map_api_dialog_start,
      map_api_dialog_set_variable,
      map_api_dialog_set_style,
      map_api_hud_set_enabled,
      map_api_hud_set_pause_enabled,
      map_api_light_get,
      map_api_light_set,
      map_api_camera_move,
      map_api_camera_restore,
      map_api_sprite_display,
      map_api_tileset_get,
      map_api_tileset_set,
      map_api_treasure_give,
      map_api_hero_set_map,
      map_api_hero_set_visible,
      map_api_hero_get_direction,
      map_api_hero_set_direction,
      map_api_hero_get_position,
      map_api_hero_set_position,
      map_api_hero_align_on_sensor,
      map_api_hero_walk,
      map_api_hero_start_jumping,
      map_api_hero_start_victory_sequence,
      map_api_hero_start_boomerang,
      map_api_hero_start_bow,
      map_api_hero_start_hookshot,
      map_api_hero_start_running,
      map_api_hero_start_hurt,
      map_api_npc_get_position,
      map_api_npc_set_position,
      map_api_npc_start_movement,
      map_api_npc_get_sprite,
      map_api_npc_remove,
      map_api_npc_exists,
      map_api_interactive_entity_get_sprite,
      map_api_interactive_entity_remove,
      map_api_interactive_entity_exists,
      map_api_chest_set_open,
      map_api_chest_is_enabled,
      map_api_chest_set_enabled,
      map_api_chest_set_group_enabled,
      map_api_tile_is_enabled,
      map_api_tile_set_enabled,
      map_api_tile_set_group_enabled,
      map_api_stairs_is_enabled,
      map_api_stairs_set_enabled,
      map_api_stairs_set_group_enabled,
      map_api_obstacle_is_enabled,
      map_api_obstacle_set_enabled,
      map_api_obstacle_set_group_enabled,
      map_api_sensor_is_enabled,
      map_api_sensor_set_enabled,
      map_api_sensor_set_group_enabled,
      map_api_jump_sensor_is_enabled,
      map_api_jump_sensor_set_enabled,
      map_api_jump_sensor_set_group_enabled,
      map_api_crystal_switch_is_enabled,
      map_api_crystal_switch_set_enabled,
      map_api_crystal_switch_set_group_enabled,
      map_api_crystal_switch_get_state,
      map_api_crystal_switch_set_state,
      map_api_crystal_switch_change_state,
      map_api_teletransporter_is_enabled,
      map_api_teletransporter_set_enabled,
      map_api_teletransporter_set_group_enabled,
      map_api_block_is_enabled,
      map_api_block_set_enabled,
      map_api_block_set_group_enabled,
      map_api_block_reset,
      map_api_block_reset_all,
      map_api_block_get_position,
      map_api_block_set_position,
      map_api_shop_item_remove,
      map_api_switch_is_activated,
      map_api_switch_set_activated,
      map_api_switch_set_locked,
      map_api_switch_is_enabled,
      map_api_switch_set_enabled,
      map_api_switch_set_group_enabled,
      map_api_door_open,
      map_api_door_close,
      map_api_door_is_open,
      map_api_door_set_open,
      map_api_pickable_item_create,
      map_api_bomb_create,
      map_api_explosion_create,
      map_api_fire_create,
      map_api_enemy_create,
      map_api_enemy_remove,
      map_api_enemy_remove_group,
      map_api_enemy_is_enabled,
      map_api_enemy_set_enabled,
      map_api_enemy_set_group_enabled,
      map_api_enemy_is_dead,
      map_api_enemy_is_group_dead,
      map_api_enemy_get_group_count,
      map_api_enemy_get_position,
      map_api_enemy_set_position,
      map_api_enemy_get_layer,
      map_api_enemy_set_layer,
      map_api_enemy_set_treasure,
      map_api_enemy_set_no_treasure,
      map_api_enemy_set_random_treasure,

      // item API
      item_api_get_variant,
      item_api_set_variant,
      item_api_get_amount,
      item_api_set_amount,
      item_api_add_amount,
      item_api_remove_amount,
      item_api_get_sprite,
      item_api_get_movement,
      item_api_start_movement,
      item_api_is_following_entity,
      item_api_get_position,
      item_api_set_position,
      item_api_get_layer,
      item_api_set_layer,
      item_api_set_layer_independent_collisions,
      item_api_set_finished,

      // enemy API
      enemy_api_get_name,
      enemy_api_get_life,
      enemy_api_set_life,
      enemy_api_add_life,
      enemy_api_remove_life,
      enemy_api_get_damage,
      enemy_api_set_damage,
      enemy_api_get_magic_damage,
      enemy_api_set_magic_damage,
      enemy_api_is_pushed_back_when_hurt,
      enemy_api_set_pushed_back_when_hurt,
      enemy_api_get_hurt_sound_style,
      enemy_api_set_hurt_sound_style,
      enemy_api_get_minimum_shield_needed,
      enemy_api_set_minimum_shield_needed,
      enemy_api_set_attack_consequence,
      enemy_api_set_attack_consequence_sprite,
      enemy_api_set_default_attack_consequences,
      enemy_api_set_default_attack_consequences_sprite,
      enemy_api_set_invincible,
      enemy_api_set_invincible_sprite,
      enemy_api_set_treasure,
      enemy_api_set_no_treasure,
      enemy_api_set_random_treasure,
      enemy_api_get_obstacle_behavior,
      enemy_api_set_obstacle_behavior,
      enemy_api_get_size,
      enemy_api_set_size,
      enemy_api_get_origin,
      enemy_api_set_origin,
      enemy_api_get_position,
      enemy_api_set_position,
      enemy_api_get_layer,
      enemy_api_set_layer,
      enemy_api_get_distance_to_hero,
      enemy_api_snap_to_grid,
      enemy_api_get_movement,
      enemy_api_start_movement,
      enemy_api_stop_movement,
      enemy_api_restart,
      enemy_api_get_sprite,
      enemy_api_has_sprite,
      enemy_api_create_sprite,
      enemy_api_remove_sprite,
      enemy_api_is_displayed_in_y_order,
      enemy_api_set_displayed_in_y_order,
      enemy_api_create_son,
      enemy_api_get_father;

  protected:

    lua_State* context;                                                 /**< the execution context of the Lua script */

    Script(uint32_t apis_enabled);

    // sprites
    int create_sprite_handle(Sprite &sprite);
    Sprite& get_sprite(int sprite_handle);

    // movements
    int create_movement_handle(Movement &movement);
    Movement& get_movement(int movement_handle);
    Movement& start_movement(int movement_handle);

    // Lua
    bool notify_script(const std::string &function_name, const std::string &format = "", ...);
    void initialize_lua_context();
    void load(const std::string &script_name);
    void load_if_exists(const std::string &script_name);
    bool is_loaded();

    // game objects
    virtual Game& get_game();
    virtual Map& get_map();
    virtual ItemProperties& get_item_properties();
    virtual Enemy& get_enemy();

  public:

    virtual ~Script();

    virtual void update();
    virtual void set_suspended(bool suspended);
    bool has_played_music();

    void event_map_changed(Map &map);
    void event_dialog_started(const MessageId &message_id);
    void event_dialog_finished(const MessageId &first_message_id, int answer);
    void event_hero_interaction(const std::string &entity_name);
    bool event_hero_interaction_item(const std::string &entity_name, const std::string &item_name, int variant);
    void event_npc_dialog(const std::string &npc_name);
    bool event_npc_dialog_item(const std::string &npc_name, const std::string &item_name, int variant);
    void event_npc_collision_fire(const std::string &npc_name);
};

#endif

