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
#ifndef SOLARUS_LUA_CONTEXT_H
#define SOLARUS_LUA_CONTEXT_H

#include "Common.h"
#include "GameCommands.h"
#include "entities/Layer.h"
#include "entities/EnemyAttack.h"
#include "lowlevel/InputEvent.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <map>
#include <set>
#include <list>
#include <lua.hpp>

/**
 * @brief This class represents a living Lua context that can execute dynamic
 * scripts at runtime.
 *
 * Such scripts includes menus, map scripts, enemy behaviors, etc.
 * All these scripts run in the same Lua context.
 *
 * On the contrary, data files written in Lua (like maps and dialogs) are
 * always parsed in their own, independent Lua states because data files only
 * have access to a few determined functions.
 *
 * TODO: this class also provides general Lua utilities that should be moved
 * to a separate LuaTools class. And rename this class to SolarusAPI?
 */
class LuaContext {

  public:

    // Functions and types.
    static const std::string main_module_name;                   /**< sol.main */
    static const std::string audio_module_name;                  /**< sol.audio */
    static const std::string timer_module_name;                  /**< sol.timer */
    static const std::string game_module_name;                   /**< sol.game */
    static const std::string map_module_name;                    /**< sol.map */
    static const std::string item_module_name;                   /**< sol.item */
    static const std::string surface_module_name;                /**< sol.surface */
    static const std::string text_surface_module_name;           /**< sol.text_surface */
    static const std::string sprite_module_name;                 /**< sol.sprite */
    static const std::string input_module_name;                  /**< sol.input */
    static const std::string video_module_name;                  /**< sol.video */
    static const std::string menu_module_name;                   /**< sol.menu */
    static const std::string language_module_name;               /**< sol.language */
    static const std::string movement_module_name;               /**< sol.movement */
    static const std::string movement_straight_module_name;      /**< sol.movement.straight */
    static const std::string movement_random_module_name;        /**< sol.movement.random */
    static const std::string movement_target_module_name;        /**< sol.movement.target */
    static const std::string movement_path_module_name;          /**< sol.movement.path */
    static const std::string movement_random_path_module_name;   /**< sol.movement.random_path */
    static const std::string movement_path_finding_module_name;  /**< sol.movement.path_finding */
    static const std::string movement_circle_module_name;        /**< sol.movement.circle */
    static const std::string movement_jump_module_name;          /**< sol.movement.jump */
    static const std::string movement_pixel_module_name;         /**< sol.movement.pixel */
    static const std::string entity_module_name;                 /**< sol.entity */
    static const std::string entity_hero_module_name;            /**< sol.entity.hero */
    static const std::string entity_npc_module_name;             /**< sol.entity.npc */
    static const std::string entity_chest_module_name;           /**< sol.entity.chest */
    static const std::string entity_block_module_name;           /**< sol.entity.block */
    static const std::string entity_switch_module_name;          /**< sol.entity.switch */
    static const std::string entity_door_module_name;            /**< sol.entity.door */
    static const std::string entity_pickable_module_name;        /**< sol.entity.pickable */
    static const std::string entity_enemy_module_name;           /**< sol.entity.enemy */

    LuaContext(MainLoop& main_loop);
    ~LuaContext();

    static LuaContext& get_lua_context(lua_State* l);

    MainLoop& get_main_loop();

    // Main loop from C++.
    void initialize();
    void exit();
    void update();
    bool notify_input(InputEvent& event);
    void notify_map_suspended(Map& map, bool suspended);
    void notify_camera_reached_target(Map& map);
    void notify_dialog_finished(int callback_ref, int answer);
    void run_item(EquipmentItem& item);
    void run_map(Map& map, Destination* destination);
    void run_enemy(Enemy& enemy);

    // Lua helpers.
    static bool is_color(lua_State* l, int index);
    static Color check_color(lua_State* l, int index);

    static int check_int_field(
        lua_State* l, int table_index, const std::string& key
    );
    static int opt_int_field(
        lua_State* l, int table_index, const std::string& key,
        int default_value
    );
    static double check_number_field(
        lua_State* l, int table_index, const std::string& key
    );
    static double opt_number_field(
        lua_State* l, int table_index, const std::string& key,
        double default_value
    );
    static const std::string check_string_field(
        lua_State* l, int table_index, const std::string& key
    );
    static const std::string opt_string_field(
        lua_State* l, int table_index, const std::string& key,
        const std::string& default_value
    );
    static bool check_boolean_field(
        lua_State* l, int table_index, const std::string& key
    );
    static bool opt_boolean_field(
        lua_State* l, int table_index, const std::string& key,
        bool default_value
    );
    static int check_function_field(
        lua_State* l, int table_index, const std::string& key
    );
    static int opt_function_field(
        lua_State* l, int table_index, const std::string& key
    );
    template<typename E>
    static E check_enum_field(
        lua_State* l, int table_index, const std::string& key,
        const std::string names[]
    );
    template<typename E>
    static E opt_enum_field(
        lua_State* l, int table_index, const std::string& key,
        const std::string names[], E default_value
    );
    template<typename E>
    static E check_enum(
        lua_State* l, int index, const std::string names[]
    );
    template<typename E>
    static E opt_enum(
        lua_State* l, int index, const std::string names[], E default_value
    );

    static int get_positive_index(lua_State* l, int index);
    static void print_stack(lua_State* l);
    static bool is_valid_lua_identifier(const std::string& name);

    // Lua refs.
    int create_ref();
    void destroy_ref(int ref);
    void push_callback(int callback_ref);
    void do_callback(int callback_ref);
    void cancel_callback(int callback_ref);

    // Timers.
    void add_timer(Timer* timer, int context_index, int callback_index);
    void remove_timer(Timer* timer);
    void remove_timers(int context_index);
    void remove_timers();
    void update_timers();
    void notify_timers_map_suspended(bool suspended);

    // Menus.
    void add_menu(int menu_ref, int context_index);
    void remove_menus(int context_index);
    void remove_menus();

    // Drawable objects.
    void add_drawable(Drawable* drawable);
    void remove_drawable(Drawable* drawable);
    void update_drawables();

    // Entities.
    static Map& get_entity_creation_map(lua_State* l);
    static Map* get_entity_implicit_creation_map(lua_State* l);
    static void set_entity_implicit_creation_map(lua_State* l, Map* map);

    // Main loop events (sol.main).
    void main_on_started();
    void main_on_finished();
    void main_on_update();
    void main_on_draw(Surface& dst_surface);
    bool main_on_input(InputEvent& event);

    // Menu events.
    void menu_on_started(int menu_ref);
    void menu_on_finished(int menu_ref);
    void menu_on_update(int menu_ref);
    void menu_on_draw(int menu_ref, Surface& dst_surface);
    bool menu_on_input(int menu_ref, InputEvent& event);
    bool menu_on_command_pressed(int menu_ref, GameCommands::Command command);
    bool menu_on_command_released(int menu_ref, GameCommands::Command command);
    void menus_on_update(int context_index);
    void menus_on_draw(int context_index, Surface& dst_surface);
    bool menus_on_input(int context_index, InputEvent& event);
    bool menus_on_command_pressed(int context_index, GameCommands::Command command);
    bool menus_on_command_released(int context_index, GameCommands::Command command);

    // Sprite events.
    void sprite_on_animation_finished(Sprite& sprite, const std::string& animation);
    void sprite_on_frame_changed(Sprite& sprite, const std::string& animation, int frame);

    // Movement events.
    void movement_on_position_changed(Movement& movement);
    void movement_on_obstacle_reached(Movement& movement);
    void movement_on_changed(Movement& movement);
    void movement_on_finished(Movement& movement);

    // Equipment item events.
    void item_on_created(EquipmentItem& item);
    void item_on_started(EquipmentItem& item);
    void item_on_finished(EquipmentItem& item);
    void item_on_update(EquipmentItem& item);
    void item_on_suspended(EquipmentItem& item, bool suspended);
    void item_on_map_changed(EquipmentItem& item, Map& map);
    void item_on_pickable_created(EquipmentItem& item, Pickable& pickable);
    void item_on_pickable_movement_changed(EquipmentItem& item, Pickable& pickable, Movement& movement);  // TODO remove this, use movement:on_changed instead
    void item_on_obtaining(EquipmentItem& item, const Treasure& treasure);
    void item_on_obtained(EquipmentItem& item, const Treasure& treasure);  // FIXME call from picked items too
    void item_on_variant_changed(EquipmentItem& item, int variant);
    void item_on_amount_changed(EquipmentItem& item, int amount);
    void item_on_using(EquipmentItem& item);
    void item_on_ability_used(EquipmentItem& item, const std::string& ability_name);
    void item_on_npc_interaction(EquipmentItem& item, NPC& npc);
    bool item_on_npc_interaction_item(EquipmentItem& item, NPC& npc,
        EquipmentItem& item_used);
    void item_on_npc_collision_fire(EquipmentItem& item, NPC& npc);

    // Game events.
    void game_on_started(Game& game);
    void game_on_finished(Game& game);
    void game_on_update(Game& game);
    void game_on_draw(Game& game, Surface& dst_surface);
    void game_on_map_changed(Game& game, Map& map);
    void game_on_paused(Game& game);
    void game_on_unpaused(Game& game);
    bool game_on_input(Game& game, InputEvent& event);
    bool game_on_command_pressed(Game& game, GameCommands::Command command);
    bool game_on_command_released(Game& game, GameCommands::Command command);

    // Map events.
    void map_on_started(Map& map, Destination* destination);
    void map_on_finished(Map& map);
    void map_on_update(Map& map);
    void map_on_draw(Map& map, Surface& dst_surface);
    void map_on_suspended(Map& map, bool suspended);
    void map_on_opening_transition_finished(Map& map,
        Destination* destination);
    void map_on_camera_back(Map& map);
    void map_on_obtaining_treasure(Map& map, const Treasure& treasure);
    void map_on_obtained_treasure(Map& map, const Treasure& treasure);
    bool map_on_input(Map& map, InputEvent& event);

    // Map entity events.
    // TODO entity_on_created
    void entity_on_removed(MapEntity& entity);
    // TODO add destination_on_activated
    void npc_on_movement_finished(NPC& npc);  // TODO remove (movement:on_finished() exists now)
    void npc_on_interaction(NPC& npc);
    bool npc_on_interaction_item(NPC& npc, EquipmentItem& item_used);
    void npc_on_collision_fire(NPC& npc);
    bool chest_on_empty(Chest& chest);
    void block_on_moved(Block& block);
    void switch_on_activated(Switch& sw);
    void switch_on_inactivated(Switch& sw);
    void switch_on_left(Switch& sw);
    void sensor_on_activated(Sensor& sensor);
    void sensor_on_activated_repeat(Sensor& sensor);
    void sensor_on_collision_explosion(Sensor& sensor);
    void door_on_open(Door& door);  // TODO rename to door:on_opened()
    void door_on_closed(Door& door);
    bool shop_item_on_buying(ShopItem& shop_item);
    void shop_item_on_bought(ShopItem& shop_item);
    void enemy_on_update(Enemy& enemy);
    void enemy_on_suspended(Enemy& enemy, bool suspended);
    void enemy_on_created(Enemy& enemy);  // TODO remove?
    void enemy_on_enabled(Enemy& enemy);
    void enemy_on_disabled(Enemy& enemy);
    void enemy_on_restarted(Enemy& enemy);
    void enemy_on_pre_draw(Enemy& enemy);
    void enemy_on_post_draw(Enemy& enemy);
    void enemy_on_position_changed(Enemy& enemy, const Rectangle& xy, Layer layer);
    void enemy_on_obstacle_reached(Enemy& enemy, Movement& movement);
    void enemy_on_movement_changed(Enemy& enemy, Movement& movement);
    void enemy_on_movement_finished(Enemy& enemy);
    void enemy_on_collision_enemy(Enemy& enemy,
        Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite);
    void enemy_on_custom_attack_received(Enemy& enemy,
        EnemyAttack attack, Sprite* sprite);
    void enemy_on_hurt(Enemy& enemy, EnemyAttack attack, int life_lost);
    void enemy_on_dying(Enemy& enemy);
    void enemy_on_dead(Enemy& enemy);
    void enemy_on_immobilized(Enemy& enemy);

    // Implementation of the API.

    /**
     * @brief Type of the functions that can be called by Lua.
     */
    typedef int (FunctionExportedToLua) (lua_State* l);

    // All functions named <type>_api_<name> can be called by Lua.
    static FunctionExportedToLua

      // Main API.
      main_api_load_file,
      main_api_do_file,
      main_api_reset,
      main_api_exit,
      main_api_is_debug_enabled,
      main_api_get_quest_write_dir,
      main_api_set_quest_write_dir,
      main_api_load_settings,
      main_api_save_settings,
      main_api_get_distance,  // TODO remove?
      main_api_get_angle,     // TODO remove?

      // Audio API.
      audio_api_play_sound,
      audio_api_preload_sounds,
      audio_api_play_music,
      audio_api_stop_music,
      audio_api_get_sound_volume,
      audio_api_set_sound_volume,
      audio_api_get_music_volume,
      audio_api_set_music_volume,

      // Video API.
      video_api_get_window_title,
      video_api_set_window_title,
      video_api_get_mode,
      video_api_set_mode,
      video_api_switch_mode,
      video_api_is_mode_supported,
      video_api_get_modes,
      video_api_is_fullscreen,
      video_api_set_fullscreen,

      // Menu API.
      menu_api_start,
      menu_api_stop,
      menu_api_stop_all,

      // Timer API.
      timer_api_start,
      timer_api_stop,
      timer_api_stop_all,
      timer_api_is_with_sound,
      timer_api_set_with_sound,
      timer_api_is_suspended,
      timer_api_set_suspended,
      timer_api_is_suspended_with_map,
      timer_api_set_suspended_with_map,
      // TODO add get_remaining_time, set_remaining_time
      // TODO remove is_with_sound, set_with_sound (do this in pure Lua, possibly with a second timer)
      // TODO game:is_suspended, timer:is/set_suspended_with_map, sprite:get/set_ignore_suspend
      // are the same concept, make these names consistent

      // Language API.
      language_api_get_language,
      language_api_set_language,
      language_api_get_language_name,
      language_api_get_languages,
      language_api_get_default_language,
      language_api_get_string,
      language_api_get_dialog,

      // Drawable API (i.e. common to surfaces, text surfaces and sprites).
      drawable_api_draw,
      drawable_api_fade_in,
      drawable_api_fade_out,
      drawable_api_start_movement,
      drawable_api_stop_movement,
      drawable_meta_gc,

      // Surface API.
      surface_api_create,
      surface_api_get_size,
      surface_api_fill_color,
      surface_api_get_transparency_color,
      surface_api_set_transparency_color,
      surface_api_set_opacity,

      // Text surface API.
      text_surface_api_create,
      text_surface_api_get_horizontal_alignment,
      text_surface_api_set_horizontal_alignment,
      text_surface_api_get_vertical_alignment,
      text_surface_api_set_vertical_alignment,
      text_surface_api_get_font,
      text_surface_api_set_font,
      text_surface_api_get_rendering_mode,
      text_surface_api_set_rendering_mode,
      text_surface_api_get_text_color,  // TODO rename to get/set_color
      text_surface_api_set_text_color,
      text_surface_api_get_text,
      text_surface_api_set_text,
      text_surface_api_set_text_key,
      text_surface_api_get_size,

      // Sprite API.
      sprite_api_create,
      sprite_api_get_animation,
      sprite_api_set_animation,
      sprite_api_get_direction,
      sprite_api_set_direction,
      sprite_api_get_frame,
      sprite_api_set_frame,
      sprite_api_get_frame_delay,  // TODO use nil for no delay (instead of 0)
      sprite_api_set_frame_delay,
      sprite_api_is_paused,
      sprite_api_set_paused,
      sprite_api_set_ignore_suspend,
      sprite_api_synchronize,

      // Movement API.
      movement_api_create,
      movement_api_get_xy,
      movement_api_set_xy,
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

      // Game API.
      game_api_exists,
      game_api_delete,
      game_api_load,
      game_api_save,  // TODO allow to change the file name (e.g. to copy)
      game_api_start,
      game_api_is_started,
      game_api_is_suspended,
      game_api_is_paused,
      game_api_set_paused,
      game_api_get_map,
      game_api_get_value,
      game_api_set_value,  // TODO allow nil (delete a value)
      game_api_get_starting_location,
      game_api_set_starting_location,  // TODO don't do it automatically, use on_map_changed
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
      game_api_get_max_money,
      game_api_set_max_money,
      game_api_get_magic,
      game_api_set_magic,
      game_api_add_magic,
      game_api_remove_magic,
      game_api_get_max_magic,
      game_api_set_max_magic,
      game_api_has_ability,
      game_api_get_ability,
      game_api_set_ability,
      game_api_get_item,
      game_api_get_item_assigned,
      game_api_set_item_assigned,
      game_api_get_command_effect,  // TODO also return "run" for action command
      game_api_get_command_keyboard_binding,
      game_api_set_command_keyboard_binding,
      game_api_get_command_joypad_binding,
      game_api_set_command_joypad_binding,
      game_api_is_command_pressed,
      game_api_get_commands_direction,  // TODO return nil instead of -1

      // Equipment item API.
      item_api_get_name,
      item_api_get_game,
      item_api_get_map,
      item_api_get_savegame_variable,
      item_api_set_savegame_variable,
      item_api_get_amount_savegame_variable,
      item_api_set_amount_savegame_variable,
      item_api_is_obtainable,
      item_api_set_obtainable,
      item_api_is_assignable,   // TODO remove
      item_api_set_assignable,  // TODO remove
      item_api_get_can_disappear,
      item_api_set_can_disappear,
      item_api_get_brandish_when_picked,
      item_api_set_brandish_when_picked,
      item_api_get_shadow,
      item_api_set_shadow,
      item_api_get_sound_when_picked,
      item_api_set_sound_when_picked,
      item_api_get_sound_when_brandished,
      item_api_set_sound_when_brandished,
      item_api_has_variant,
      item_api_get_variant,
      item_api_set_variant,
      item_api_has_amount,
      item_api_get_amount,
      item_api_set_amount,
      item_api_add_amount,
      item_api_remove_amount,
      item_api_get_max_amount,
      item_api_set_max_amount,
      item_api_set_finished,

      // Map API.
      map_api_get_id,
      map_api_get_game,
      map_api_get_world,
      map_api_get_floor,
      map_api_get_size,
      map_api_get_location,
      map_api_get_tileset,
      map_api_set_tileset,
      map_api_is_dialog_enabled,  // TODO script the dialog box, remove set_dialog_style, set_dialog_position, draw_dialog_box
      map_api_start_dialog,
      map_api_set_dialog_variable,
      map_api_set_dialog_style,
      map_api_set_dialog_position,
      map_api_draw_dialog_box,
      map_api_set_pause_enabled,  // TODO move to game api?
      map_api_get_light,
      map_api_set_light,
      map_api_move_camera,  // TODO get_camera_position
      map_api_draw_sprite,  // TODO allow to also draw a surface or a text surface
      map_api_get_crystal_state,
      map_api_set_crystal_state,
      map_api_change_crystal_state,
      map_api_open_doors,
      map_api_close_doors,
      map_api_set_doors_open,
      map_api_get_entity,
      map_api_has_entity,
      map_api_get_entities,
      map_api_get_entities_count,
      map_api_has_entities,
      map_api_set_entities_enabled,
      map_api_remove_entities,
      map_api_create_tile,
      map_api_create_destination,
      map_api_create_teletransporter,  // TODO stringify transition_style
      map_api_create_pickable,
      map_api_create_destructible,
      map_api_create_chest,
      map_api_create_jumper,
      map_api_create_enemy,
      map_api_create_npc,  // TODO use a real string for the subtype, improve the behavior syntax
      map_api_create_block,  // Allow direction to be nil, make maximum_moves really be the limit (nil means no limit)
      map_api_create_dynamic_tile,
      map_api_create_switch,
      map_api_create_wall,
      map_api_create_sensor,
      map_api_create_crystal,
      map_api_create_crystal_block,
      map_api_create_shop_item,  // TODO rename to shop treasure
      map_api_create_conveyor_belt,
      map_api_create_door,
      map_api_create_stairs,
      map_api_create_bomb,
      map_api_create_explosion,
      map_api_create_fire,

      // Map entity API.
      // TODO entity:get_type()
      entity_api_get_map,
      entity_api_get_name,  // TODO return nil if no name
      entity_api_exists,
      entity_api_remove,
      entity_api_is_enabled,
      entity_api_set_enabled,
      entity_api_get_size,
      entity_api_set_size,
      entity_api_get_origin,
      entity_api_set_origin,
      entity_api_get_position,
      entity_api_set_position,  // TODO register on all entity types
      entity_api_get_distance,
      entity_api_get_angle,
      entity_api_snap_to_grid,
      entity_api_get_sprite,
      entity_api_create_sprite,
      entity_api_remove_sprite,
      entity_api_get_movement,  // TODO also add to NPC
      entity_api_start_movement,  // TODO register on all entity types?
      entity_api_stop_movement,
      entity_api_has_layer_independent_collisions,  // TODO never registered
      entity_api_set_layer_independent_collisions,
      entity_api_test_obstacles,  // TODO register on all entity types
      entity_api_get_optimization_distance,
      entity_api_set_optimization_distance,  // TODO register on all entity types
      hero_api_teleport,
      hero_api_set_visible,
      hero_api_get_direction,
      hero_api_set_direction,
      hero_api_set_position,
      hero_api_save_solid_ground,
      hero_api_reset_solid_ground,
      hero_api_freeze,
      hero_api_unfreeze,
      hero_api_walk,
      hero_api_start_jumping,
      hero_api_start_treasure,  // TODO don't die if the savegame variable is invalid
      hero_api_start_victory,
      hero_api_start_boomerang,
      hero_api_start_bow,
      hero_api_start_hookshot,
      hero_api_start_running,
      hero_api_start_hurt,
      chest_api_is_open,
      chest_api_set_open,
      block_api_reset,
      switch_api_is_activated,
      switch_api_set_activated,
      switch_api_set_locked,
      door_api_is_open,
      pickable_api_get_followed_entity,
      pickable_api_get_falling_height,
      pickable_api_get_treasure,  // TODO return the item, not the item name
      enemy_api_get_breed,
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
      enemy_api_get_can_attack,
      enemy_api_set_can_attack,
      enemy_api_get_minimum_shield_needed,
      enemy_api_set_minimum_shield_needed,
      enemy_api_set_attack_consequence,
      enemy_api_set_attack_consequence_sprite,
      enemy_api_set_default_attack_consequences,
      enemy_api_set_default_attack_consequences_sprite,
      enemy_api_set_invincible,
      enemy_api_set_invincible_sprite,
      enemy_api_set_treasure,
      enemy_api_get_obstacle_behavior,
      enemy_api_set_obstacle_behavior,
      enemy_api_restart,
      enemy_api_hurt,
      enemy_api_create_enemy,

      // available to all userdata types
      userdata_meta_gc,
      userdata_meta_newindex_as_table,
      userdata_meta_index_as_table;

  private:

    /**
     * @brief Data associated to any Lua menu.
     */
    struct LuaMenuData {
      int ref;               /**< Lua ref of the table of the menu. */
      const void* context;  /**< Lua table or userdata the menu is attached to. */

      LuaMenuData(int ref, const void* context):
        ref(ref),
        context(context) {
      }
    };

    /**
     * @brief Data associated to any Lua timer.
     */
    struct LuaTimerData {
      int callback_ref;     /**< Lua ref of the function to call after the timer. */
      const void* context;  /**< Lua table or userdata the timer is attached to. */
    };

    // Executing Lua code.
    bool find_global_function(const std::string& function_name);
    bool find_local_function(int index, const std::string& function_name);
    bool find_local_function(const std::string& function_name);
    bool find_method(int index, const std::string& function_name);
    bool find_method(const std::string& function_name);
    bool call_function(int nb_arguments, int nb_results,
        const std::string& function_name);
    static bool call_function(lua_State* l, int nb_arguments, int nb_results,
        const std::string& function_name);
    static void load_file(lua_State* l, const std::string& script_name);
    static bool load_file_if_exists(lua_State* l, const std::string& script_name);
    static void do_file(lua_State* l, const std::string& script_name);
    static bool do_file_if_exists(lua_State* l, const std::string& script_name);

    // Initialization of modules.
    void register_functions(const std::string& module_name, const luaL_Reg* functions);
    void register_type(const std::string& module_name, const luaL_Reg* methods,
        const luaL_Reg* metamethods);
    void register_modules();
    void register_main_module();
    void register_audio_module();
    void register_timer_module();
    void register_item_module();
    void register_surface_module();
    void register_text_surface_module();
    void register_sprite_module();
    void register_movement_module();
    void register_input_module();
    void register_video_module();
    void register_menu_module();
    void register_language_module();
    void register_game_module();
    void register_map_module();
    void register_entity_module();

    // Pushing objects to Lua.
    static void push_ref(lua_State* l, int ref);
    static void push_main(lua_State* l);
    static void push_string(lua_State* l, const std::string& text);
    static void push_userdata(lua_State* l, ExportableToLua& userdata);
    static void push_color(lua_State* l, const Color& color);
    static void push_timer(lua_State* l, Timer& timer);
    static void push_surface(lua_State* l, Surface& surface);
    static void push_text_surface(lua_State* l, TextSurface& text_surface);
    static void push_sprite(lua_State* l, Sprite& sprite);
    static void push_item(lua_State* l, EquipmentItem& item);
    static void push_movement(lua_State* l, Movement& movement);
    static void push_game(lua_State* l, Savegame& game);
    static void push_map(lua_State* l, Map& map);
    static void push_entity(lua_State* l, MapEntity& entity);
    static void push_hero(lua_State* l, Hero& hero);
    static void push_npc(lua_State* l, NPC& npc);
    static void push_chest(lua_State* l, Chest& chest);
    static void push_block(lua_State* l, Block& block);
    static void push_switch(lua_State* l, Switch& sw);
    static void push_door(lua_State* l, Door& door);
    static void push_pickable(lua_State* l, Pickable& pickable);
    static void push_enemy(lua_State* l, Enemy& enemy);

    // Getting userdata objects from Lua.
    static bool is_userdata(lua_State* l, int index,
        const std::string& module_name);
    static ExportableToLua& check_userdata(lua_State* l, int index,
        const std::string& module_name);
    static bool is_timer(lua_State* l, int index);
    static Timer& check_timer(lua_State* l, int index);
    static bool is_drawable(lua_State* l, int index);
    static Drawable& check_drawable(lua_State* l, int index);
    static bool is_surface(lua_State* l, int index);
    static Surface& check_surface(lua_State* l, int index);
    static bool is_text_surface(lua_State* l, int index);
    static TextSurface& check_text_surface(lua_State* l, int index);
    static bool is_sprite(lua_State* l, int index);
    static Sprite& check_sprite(lua_State* l, int index);
    static bool is_item(lua_State* l, int index);
    static EquipmentItem& check_item(lua_State* l, int index);
    static bool is_movement(lua_State* l, int index);
    static Movement& check_movement(lua_State* l, int index);
    static bool is_straight_movement(lua_State* l, int index);
    static StraightMovement& check_straight_movement(lua_State* l, int index);
    static bool is_random_movement(lua_State* l, int index);
    static RandomMovement& check_random_movement(lua_State* l, int index);
    static bool is_target_movement(lua_State* l, int index);
    static TargetMovement& check_target_movement(lua_State* l, int index);
    static bool is_path_movement(lua_State* l, int index);
    static PathMovement& check_path_movement(lua_State* l, int index);
    static bool is_random_path_movement(lua_State* l, int index);
    static RandomPathMovement& check_random_path_movement(lua_State* l, int index);
    static bool is_path_finding_movement(lua_State* l, int index);
    static PathFindingMovement& check_path_finding_movement(lua_State* l, int index);
    static bool is_circle_movement(lua_State* l, int index);
    static CircleMovement& check_circle_movement(lua_State* l, int index);
    static bool is_jump_movement(lua_State* l, int index);
    static JumpMovement& check_jump_movement(lua_State* l, int index);
    static bool is_pixel_movement(lua_State* l, int index);
    static PixelMovement& check_pixel_movement(lua_State* l, int index);
    static bool is_game(lua_State* l, int index);
    static Savegame& check_game(lua_State* l, int index);
    static bool is_map(lua_State* l, int index);
    static Map& check_map(lua_State* l, int index);
    static bool is_entity(lua_State* l, int index);
    static MapEntity& check_entity(lua_State* l, int index);
    static bool is_hero(lua_State* l, int index);
    static Hero& check_hero(lua_State* l, int index);
    static bool is_npc(lua_State* l, int index);
    static NPC& check_npc(lua_State* l, int index);
    static bool is_chest(lua_State* l, int index);
    static Chest& check_chest(lua_State* l, int index);
    static bool is_block(lua_State* l, int index);
    static Block& check_block(lua_State* l, int index);
    static bool is_switch(lua_State* l, int index);
    static Switch& check_switch(lua_State* l, int index);
    static bool is_door(lua_State* l, int index);
    static Door& check_door(lua_State* l, int index);
    static bool is_pickable(lua_State* l, int index);
    static Pickable& check_pickable(lua_State* l, int index);
    static bool is_enemy(lua_State* l, int index);
    static Enemy& check_enemy(lua_State* l, int index);

    // Events.
    void on_started();
    void on_finished();
    void on_update();
    void on_draw(Surface& dst_surface);
    void on_pre_draw(Surface& dst_surface);
    void on_post_draw(Surface& dst_surface);
    void on_suspended(bool suspended);
    void on_paused();
    void on_unpaused();
    bool on_input(InputEvent& event);
    bool on_key_pressed(InputEvent& event);
    bool on_key_released(InputEvent& event);
    bool on_character_pressed(InputEvent& event);
    bool on_joypad_button_pressed(InputEvent& event);
    bool on_joypad_button_released(InputEvent& event);
    bool on_joypad_axis_moved(InputEvent& event);
    bool on_joypad_hat_moved(InputEvent& event);
    bool on_command_pressed(GameCommands::Command command);
    bool on_command_released(GameCommands::Command command);
    void on_animation_finished(const std::string& animation);
    void on_frame_changed(const std::string& animation, int frame);
    void on_position_changed();
    void on_obstacle_reached();
    void on_changed();
    void on_started(Destination* destination);
    void on_opening_transition_finished(Destination* destination);
    void on_camera_back();
    void on_obtaining_treasure(const Treasure& treasure);
    void on_obtained_treasure(const Treasure& treasure);
    void on_activated();
    void on_activated_repeat();
    void on_inactivated();
    void on_left();
    void on_interaction();
    bool on_interaction_item(EquipmentItem& item_used);
    void on_npc_interaction(NPC& npc);
    bool on_npc_interaction_item(NPC& npc, EquipmentItem& item_used);
    void on_npc_collision_fire(NPC& npc);
    void on_collision_fire();
    void on_collision_explosion();
    void on_collision_enemy(Enemy& enemy, Sprite& other_sprite, Sprite& this_sprite);
    bool on_empty();
    bool on_buying();
    void on_bought();
    void on_open();
    void on_closed();
    void on_moved();
    void on_map_changed(Map& map);
    void on_pickable_created(Pickable& pickable);
    void on_pickable_movement_changed(Pickable& pickable, Movement& movement);
    void on_variant_changed(int variant);
    void on_amount_changed(int amount);
    void on_obtaining(const Treasure& treasure);
    void on_obtained(const Treasure& treasure);
    void on_using();
    void on_ability_used(const std::string& ability_name);
    void on_created();
    void on_removed();
    void on_enabled();
    void on_disabled();
    void on_restarted();
    void on_pre_draw();
    void on_post_draw();
    void on_position_changed(const Rectangle& xy, Layer layer);
    void on_obstacle_reached(Movement& movement);
    void on_movement_changed(Movement& movement);
    void on_movement_finished();
    void on_custom_attack_received(EnemyAttack attack, Sprite* sprite);
    void on_hurt(EnemyAttack attack, int life_lost);
    void on_dying();
    void on_dead();
    void on_immobilized();

    // Functions exported to Lua for internal needs.
    static FunctionExportedToLua
      l_panic,
      l_loader,
      l_get_map_entity_or_global,
      l_camera_do_callback,
      l_camera_restore;

    // Script data.
    lua_State* l;                   /**< The Lua state encapsulated. */
    MainLoop& main_loop;            /**< The Solarus main loop. */

    std::list<LuaMenuData> menus;   /**< The menus currently running in their context. */
    std::map<Timer*, LuaTimerData>
        timers;                     /**< The timers currently running, with
                                     * their context and callback. */

    std::set<Drawable*> drawables;  /**< All drawable objects created by
                                     * this script. */

    static const std::string enemy_attack_names[];
    static const std::string enemy_hurt_style_names[];
    static const std::string enemy_obstacle_behavior_names[];
    static const std::string transition_style_names[];
};

/**
 * @brief Checks whether a value is the name of an enumeration value and
 * returns this value.
 *
 * Raises a Lua error if the value is not a string or if the string cannot
 * be found in the array.
 * This is a useful function for mapping strings to C enums.
 *
 * This function is similar to luaL_checkoption except that it accepts an
 * array of std::string instead of char*, and returns a value of enumerated
 * type E instead of int.
 *
 * @param l A Lua state.
 * @param index Index of a string in the Lua stack.
 * @param names An array of strings to search in. This array must be
 * terminated by an empty string.
 * @return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaContext::check_enum(
    lua_State* l, int index, const std::string names[]) {

  Debug::check_assertion(!names[0].empty(), "Invalid list of names");

  const std::string& name = luaL_checkstring(l, index);
  for (int i = 0; !names[i].empty(); ++i) {
    if (names[i] == name) {
      return E(i);
    }
  }

  // The value was not found. Build an error message with possible values.
  std::string allowed_names;
  for (int i = 0; !names[i].empty(); ++i) {
    allowed_names += "\"" + names[i] + "\", ";
  }
  allowed_names = allowed_names.substr(0, allowed_names.size() - 2);

  luaL_argerror(l, index, (StringConcat() <<
      "Invalid name '" << name << "'. Allowed names are: " << allowed_names).c_str());
  throw;  // Make sure the compiler is happy.
}

/**
 * @brief Like check_enum but with a default value.
 *
 * @param l A Lua state.
 * @param index Index of a string in the Lua stack.
 * @param names An array of strings to search in. This array must be
 * terminated by an empty string.
 * @param default_value The default value to return.
 * @return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaContext::opt_enum(
    lua_State* l, int index, const std::string names[], E default_value) {

  E value = default_value;
  if (!lua_isnoneornil(l, index)) {
    value = check_enum<E>(l, index, names);
  }
  return value;
}

/**
 * @brief Checks that a table field is the name of an enumeration value and
 * returns this value.
 *
 * This function acts like lua_getfield() followed by check_enum().
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @param names An array of strings to search in. This array must be
 * terminated by an empty string.
 * @return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaContext::check_enum_field(
    lua_State* l, int table_index, const std::string& key,
    const std::string names[]) {

  lua_getfield(l, table_index, key.c_str());
  if (!lua_isstring(l, -1)) {
    luaL_argerror(l, table_index, (StringConcat() <<
        "Bad field '" << key << "' (string expected, got " <<
        luaL_typename(l, -1)).c_str()
    );
  }

  E value = check_enum<E>(l, -1, names);
  lua_pop(l, 1);
  return value;
}

/**
 * @brief Like check_enum_field but with a default value.
 *
 * @param l A Lua state.
 * @param table_index Index of a table in the stack.
 * @param key Key of the field to get in that table.
 * @param names An array of strings to search in. This array must be
 * terminated by an empty string.
 * @param default_value The default value to return.
 * @return The index (converted to the enumerated type E) where the string was
 * found in the array.
 */
template<typename E>
E LuaContext::opt_enum_field(
    lua_State* l, int table_index, const std::string& key,
    const std::string names[], E default_value) {

  lua_getfield(l, table_index, key.c_str());
  E value = default_value;
  if (!lua_isnil(l, -1)) {
    if (!lua_isstring(l, -1)) {
      luaL_argerror(l, table_index, (StringConcat() <<
          "Bad field '" << key << "' (string expected, got " <<
          luaL_typename(l, -1)).c_str()
      );
    }
    value = check_enum<E>(l, -1, names);
  }

  return value;
}

#endif

