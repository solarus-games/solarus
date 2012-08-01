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
#ifndef SOLARUS_SCRIPT_H
#define SOLARUS_SCRIPT_H

#include "Common.h"
#include "lowlevel/InputEvent.h"
#include <map>
#include <list>
#include <set>

struct lua_State;
struct luaL_Reg;

/**
 * @brief Handles a Lua script that is running in its own Lua context.
 *
 * This class and its subclasses provide an API that allows Lua scripts to call C++ functions.
 *
 * TODO: The Lua bindings are currently being modified so that all scripts use
 * only one shared Lua context. This shared context is managed by the
 * LuaContext class.
 * The Script class will become obsolete once all scripts are converted to
 * the LuaContext class.
 */
class Script {

  public:

    // functions and types
    static const std::string main_module_name;                   /**< sol.main */
    static const std::string audio_module_name;                  /**< sol.audio */
    static const std::string timer_module_name;                  /**< sol.timer */
    static const std::string game_module_name;                   /**< sol.game */
    static const std::string map_module_name;                    /**< sol.map */
    static const std::string item_module_name;                   /**< sol.item */
    static const std::string enemy_module_name;                  /**< sol.enemy */
    static const std::string surface_module_name;                /**< sol.surface */
    static const std::string text_surface_module_name;           /**< sol.text_surface */
    static const std::string sprite_module_name;                 /**< sol.sprite */
    static const std::string input_module_name;                  /**< sol.input */
    static const std::string movement_module_name;               /**< sol.movement */

    // subtypes
    static const std::string straight_movement_module_name;      /**< sol.movement.straight */
    static const std::string random_movement_module_name;        /**< sol.movement.random */
    static const std::string target_movement_module_name;        /**< sol.movement.target */
    static const std::string path_movement_module_name;          /**< sol.movement.path */
    static const std::string random_path_movement_module_name;   /**< sol.movement.random_path */
    static const std::string path_finding_movement_module_name;  /**< sol.movement.path_finding */
    static const std::string circle_movement_module_name;        /**< sol.movement.circle */
    static const std::string jump_movement_module_name;          /**< sol.movement.jump */
    static const std::string pixel_movement_module_name;         /**< sol.movement.pixel */

    virtual ~Script();

    MainLoop& get_main_loop();
    Game* get_current_game();
    CustomScreen* get_current_screen();

    // main loop
    virtual void update();
    virtual void set_suspended(bool suspended);

    // Lua refs.
    int create_ref();
    void destroy_ref(int ref);
    void do_callback(int callback_ref);
    void cancel_callback(int callback_ref);

    // userdata garbage collection
    bool has_created(ExportableToLua* userdata);
    void set_created(ExportableToLua* userdata);
    void increment_refcount(ExportableToLua* userdata);
    void decrement_refcount(ExportableToLua* userdata);

  protected:

    /**
     * @brief Data associated to any Lua timer.
     */
    struct LuaTimerData {
      int callback_ref;     /**< Lua ref of the function to call after the timer. */
      const void* context;  /**< Lua table or userdata the timer is attached to. */
    };

    lua_State* l;                        /**< the execution context of the Lua script */

    Script(MainLoop& main_loop, uint32_t apis_enabled = 0);

    void set_current_game(Game* current_game);
    void set_current_screen(CustomScreen* current_screen);

    // Helper functions.
    void initialize();
    void exit();
    static Script& get_script(lua_State* l);
    bool find_global_function(const std::string& function_name);
    bool call_function(int nb_arguments, int nb_results, const std::string& function_name);
    static bool call_function(lua_State* l, int nb_arguments, int nb_results, const std::string& function_name);
    static void load_file(lua_State* l, const std::string& script_name);
    static bool load_file_if_exists(lua_State* l, const std::string& script_name);
    static void do_file(lua_State* l, const std::string& script_name);
    static bool do_file_if_exists(lua_State* l, const std::string& script_name);
    static int get_positive_index(lua_State* l, int index);
    void print_stack();

    // Modules.
    void register_functions(const std::string& module_name, const luaL_Reg* functions);
    void register_type(const std::string& module_name, const luaL_Reg* functions,
        const luaL_Reg* metamethods);

    static void push_main(lua_State* l);
    static void push_userdata(lua_State* l, ExportableToLua& userdata);
    static void push_timer(lua_State* l, Timer& timer);
    static void push_surface(lua_State* l, Surface& surface);
    static void push_text_surface(lua_State* l, TextSurface& text_surface);
    static void push_sprite(lua_State* l, Sprite& sprite);
    static void push_color(lua_State* l, const Color& color);
    static void push_game(lua_State* l, Savegame& game);
    static void push_map(lua_State* l, Map& map);
    static void push_enemy(lua_State* l, Enemy& enemy);
    static void push_item(lua_State* l, EquipmentItem& item);
    static void push_movement(lua_State* l, Movement& movement);
    static void push_ref(lua_State* l, int ref);
    const std::string& input_get_key_name(InputEvent::KeyboardKey key);

    static bool is_userdata(lua_State* l, int index,
        const std::string& module_name);
    static ExportableToLua& check_userdata(lua_State* l, int index,
        const std::string& module_name);
    static Timer& check_timer(lua_State* l, int index);
    static DynamicDisplayable& check_displayable(lua_State* l, int index);
    static Surface& check_surface(lua_State* l, int index);
    static TextSurface& check_text_surface(lua_State* l, int index);
    static Sprite& check_sprite(lua_State* l, int index);
    static Color check_color(lua_State* l, int index);
    static Savegame& check_game(lua_State* l, int index);
    static Map& check_map(lua_State* l, int index);
    static Enemy& check_enemy(lua_State* l, int index);
    static EquipmentItem& check_item(lua_State* l, int index);
    static Movement& check_movement(lua_State* l, int index);
    static StraightMovement& check_straight_movement(lua_State* l, int index);
    static RandomMovement& check_random_movement(lua_State* l, int index);
    static TargetMovement& check_target_movement(lua_State* l, int index);
    static PathMovement& check_path_movement(lua_State* l, int index);
    static RandomPathMovement& check_random_path_movement(lua_State* l, int index);
    static PathFindingMovement& check_path_finding_movement(lua_State* l, int index);
    static CircleMovement& check_circle_movement(lua_State* l, int index);
    static JumpMovement& check_jump_movement(lua_State* l, int index);
    static PixelMovement& check_pixel_movement(lua_State* l, int index);

    // Timers.
    bool is_new_timer_suspended(void);
    void add_timer(Timer* timer, int context_index, int callback_index);
    void remove_timer(Timer* timer);
    void remove_timers(int context_index);
    void remove_timers();
    void update_timers();
    void set_suspended_timers(bool suspended);

  private:

    // script data
    MainLoop& main_loop;            /**< the Solarus main loop */
    Game* current_game;             /**< the game currently running if any */
    CustomScreen* current_screen;   /**< the current menu when no game is running */

    std::map<Timer*, LuaTimerData>
        timers;                     /**< the timers currently running, with
                                     * their context and callback ref */

    std::set<DynamicDisplayable*>
      displayables;                 /**< all displayable objects created by
                                     * this script */

    static std::map<InputEvent::KeyboardKey, std::string>
      input_key_names; /**< names of all existing keyboard keys in Lua */

    // initialization of modules
    void register_modules();
    void register_main_module();
    void register_audio_module();
    void register_timer_module();
    void register_game_module();
    void register_map_module();
    void register_item_module();
    void register_enemy_module();
    void register_surface_module();
    void register_text_surface_module();
    void register_sprite_module();
    void register_movement_module();
    void register_input_module();

    // displayable objects
    bool has_displayable(DynamicDisplayable* displayable);
    void add_displayable(DynamicDisplayable* displayable);
    void remove_displayable(DynamicDisplayable* displayable);
    void update_displayables();

  protected:

    // implementation of the APIs
    typedef int (FunctionAvailableToScript) (lua_State *l);  /**< type of the functions that can be
                                                              * called by a Lua script */
    static FunctionAvailableToScript 

      // main API
      main_api_load_file,
      main_api_do_file,
      main_api_reset,
      main_api_exit,
      main_api_start_screen,
      main_api_get_distance,  // TODO remove
      main_api_get_angle,     // TODO remove

      // audio API
      audio_api_play_sound,
      audio_api_preload_sounds,
      audio_api_play_music,
      audio_api_stop_music,
      audio_api_get_sound_volume,
      audio_api_set_sound_volume,
      audio_api_get_music_volume,
      audio_api_set_music_volume,

      // video API
      video_api_get_mode,
      video_api_set_mode,
      video_api_switch_mode,
      video_api_is_mode_supported,
      video_api_get_modes,
      video_api_is_fullscreen,
      video_api_set_fullscreen,

      // timer API
      timer_api_start,
      timer_api_stop,
      timer_api_is_with_sound,
      timer_api_set_with_sound,

      // language API
      language_api_get_language,
      language_api_set_language,
      language_api_get_language_name,
      language_api_get_languages,
      language_api_get_default_language,
      language_api_get_string,
      language_api_get_dialog,

      // game API
      game_api_exists,
      game_api_delete,
      game_api_load,
      game_api_save,
      game_api_start,
      game_api_is_started,
      game_api_is_suspended,
      game_api_get_string,
      game_api_get_integer,
      game_api_get_boolean,
      game_api_set_string,
      game_api_set_integer,
      game_api_set_boolean,
      game_api_get_player_name,
      game_api_set_player_name,
      game_api_get_life,
      game_api_set_life,
      game_api_add_life,
      game_api_remove_life,
      game_api_get_max_life,
      game_api_set_max_life,
      game_api_add_max_life,
      game_api_get_money,
      game_api_set_money,
      game_api_add_money,
      game_api_remove_money,
      game_api_get_magic,
      game_api_set_magic,
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
      map_api_get_game,
      map_api_dialog_start,
      map_api_dialog_set_variable,
      map_api_dialog_set_style,
      map_api_hud_set_enabled,
      map_api_hud_set_pause_enabled,
      map_api_light_get,
      map_api_light_set,
      map_api_camera_move,
      map_api_sprite_display,
      map_api_tileset_get,
      map_api_tileset_set,
      map_api_treasure_give,
      map_api_hero_freeze,
      map_api_hero_unfreeze,
      map_api_hero_set_map,
      map_api_hero_set_visible,
      map_api_hero_get_direction,
      map_api_hero_set_direction,
      map_api_hero_get_position,
      map_api_hero_set_position,
      map_api_hero_align_on_sensor,
      map_api_hero_save_solid_ground,
      map_api_hero_reset_solid_ground,
      map_api_hero_walk,
      map_api_hero_start_jumping,
      map_api_hero_start_victory_sequence,
      map_api_hero_start_boomerang,
      map_api_hero_start_bow,
      map_api_hero_start_hookshot,
      map_api_hero_start_running,
      map_api_hero_start_hurt,
      map_api_npc_is_enabled,
      map_api_npc_set_enabled,
      map_api_npc_set_group_enabled,
      map_api_npc_get_position,
      map_api_npc_set_position,
      map_api_npc_start_movement,
      map_api_npc_stop_movement,
      map_api_npc_get_sprite,
      map_api_npc_remove,
      map_api_npc_exists,
      map_api_chest_is_open,
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
      map_api_jumper_is_enabled,
      map_api_jumper_set_enabled,
      map_api_jumper_set_group_enabled,
      map_api_crystal_is_enabled,
      map_api_crystal_set_enabled,
      map_api_crystal_set_group_enabled,
      map_api_crystal_get_state,
      map_api_crystal_set_state,
      map_api_crystal_change_state,
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
      map_api_shop_item_exists,
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
      map_api_destructible_item_create,
      map_api_block_create,
      map_api_bomb_create,
      map_api_explosion_create,
      map_api_fire_create,
      map_api_arrow_remove,
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
      map_api_enemy_set_treasure,
      map_api_enemy_set_no_treasure,
      map_api_enemy_set_random_treasure,
      map_api_enemy_get_sprite,

      // item API
      item_api_get_game,
      item_api_get_map,
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
      item_api_set_layer_independent_collisions,
      item_api_set_finished,

      // enemy API
      enemy_api_get_map,
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
      enemy_api_get_push_hero_on_sword,
      enemy_api_set_push_hero_on_sword,
      enemy_api_get_can_hurt_hero_running,
      enemy_api_set_can_hurt_hero_running,
      enemy_api_get_hurt_style,
      enemy_api_set_hurt_style,
      enemy_api_get_minimum_shield_needed,
      enemy_api_set_minimum_shield_needed,
      enemy_api_get_can_attack,
      enemy_api_set_can_attack,
      enemy_api_set_attack_consequence,
      enemy_api_set_attack_consequence_sprite,
      enemy_api_set_default_attack_consequences,
      enemy_api_set_default_attack_consequences_sprite,
      enemy_api_set_invincible,
      enemy_api_set_invincible_sprite,
      enemy_api_set_layer_independent_collisions,
      enemy_api_set_treasure,
      enemy_api_set_no_treasure,
      enemy_api_set_random_treasure,
      enemy_api_get_obstacle_behavior,
      enemy_api_set_obstacle_behavior,
      enemy_api_get_optimization_distance,
      enemy_api_set_optimization_distance,
      enemy_api_get_size,
      enemy_api_set_size,
      enemy_api_get_origin,
      enemy_api_set_origin,
      enemy_api_get_position,
      enemy_api_set_position,
      enemy_api_get_distance_to_hero,
      enemy_api_get_angle_to_hero,
      enemy_api_test_obstacles,
      enemy_api_snap_to_grid,
      enemy_api_get_movement,
      enemy_api_start_movement,
      enemy_api_stop_movement,
      enemy_api_restart,
      enemy_api_hurt,
      enemy_api_create_sprite,
      enemy_api_remove_sprite,
      enemy_api_get_sprite,
      enemy_api_create_son,
      enemy_api_get_father,
      enemy_api_send_message,

      // surface API
      surface_api_create,
      surface_api_get_size,
      surface_api_fill_color,
      surface_api_set_transparency_color,
      surface_api_set_opacity,

      // text surface API
      text_surface_api_create,
      text_surface_api_get_horizontal_alignment,
      text_surface_api_set_horizontal_alignment,
      text_surface_api_get_vertical_alignment,
      text_surface_api_set_vertical_alignment,
      text_surface_api_get_font,
      text_surface_api_set_font,
      text_surface_api_get_rendering_mode,
      text_surface_api_set_rendering_mode,
      text_surface_api_get_text_color,
      text_surface_api_set_text_color,
      text_surface_api_get_text,
      text_surface_api_set_text,
      text_surface_api_set_text_key,
      text_surface_api_get_size,

      // sprite API
      sprite_api_create,
      sprite_api_get_animation,
      sprite_api_set_animation,
      sprite_api_get_direction,
      sprite_api_set_direction,
      sprite_api_get_frame,
      sprite_api_set_frame,
      sprite_api_get_frame_delay,
      sprite_api_set_frame_delay,
      sprite_api_is_paused,
      sprite_api_set_paused,
      sprite_api_set_ignore_suspend,
      sprite_api_synchronize,

      // displayable API (i.e. common to surfaces, text surfaces and sprites)
      displayable_api_draw,
      displayable_api_fade_in,
      displayable_api_fade_out,
      displayable_api_start_movement,
      displayable_api_stop_movement,
      displayable_meta_gc,

      // movement API
      movement_api_create,
      movement_api_get_ignore_obstacles,
      movement_api_set_ignore_obstacles,
      movement_api_get_direction4,
      straight_movement_api_get_speed,
      straight_movement_api_set_speed,
      straight_movement_api_get_angle,
      straight_movement_api_set_angle,
      straight_movement_api_get_max_distance,
      straight_movement_api_set_max_distance,
      straight_movement_api_is_smooth,
      straight_movement_api_set_smooth,
      random_movement_api_get_speed,
      random_movement_api_set_speed,
      random_movement_api_get_angle,
      random_movement_api_get_max_distance,
      random_movement_api_set_max_distance,
      random_movement_api_is_smooth,
      random_movement_api_set_smooth,
      target_movement_api_set_target,
      target_movement_api_get_speed,
      target_movement_api_set_speed,
      target_movement_api_get_angle,
      target_movement_api_is_smooth,
      target_movement_api_set_smooth,
      path_movement_api_get_path,
      path_movement_api_set_path,
      path_movement_api_get_speed,
      path_movement_api_set_speed,
      path_movement_api_get_loop,
      path_movement_api_set_loop,
      path_movement_api_get_snap_to_grid,
      path_movement_api_set_snap_to_grid,
      random_path_movement_api_get_speed,
      random_path_movement_api_set_speed,
      path_finding_movement_api_set_target,
      path_finding_movement_api_get_speed,
      path_finding_movement_api_set_speed,
      circle_movement_api_set_center,
      circle_movement_api_get_radius,
      circle_movement_api_set_radius,
      circle_movement_api_get_radius_speed,
      circle_movement_api_set_radius_speed,
      circle_movement_api_is_clockwise,
      circle_movement_api_set_clockwise,
      circle_movement_api_get_initial_angle,
      circle_movement_api_set_initial_angle,
      circle_movement_api_get_angle_speed,
      circle_movement_api_set_angle_speed,
      circle_movement_api_get_max_rotations,
      circle_movement_api_set_max_rotations,
      circle_movement_api_get_duration,
      circle_movement_api_set_duration,
      circle_movement_api_get_loop_delay,
      circle_movement_api_set_loop_delay,
      jump_movement_api_get_direction8,
      jump_movement_api_set_direction8,
      jump_movement_api_get_distance,
      jump_movement_api_set_distance,
      jump_movement_api_get_speed,
      jump_movement_api_set_speed,
      pixel_movement_api_get_trajectory,
      pixel_movement_api_set_trajectory,
      pixel_movement_api_get_loop,
      pixel_movement_api_set_loop,
      pixel_movement_api_get_delay,
      pixel_movement_api_set_delay,

      // available to all userdata types
      userdata_meta_eq,
      userdata_meta_gc,
      userdata_meta_newindex_as_table,
      userdata_meta_index_as_table;
};

#endif

