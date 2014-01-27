/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/EntityType.h"
#include "lowlevel/InputEvent.h"
#include "lowlevel/Debug.h"
#include "Ability.h"
#include <map>
#include <set>
#include <list>

struct lua_State;
struct luaL_Reg;

namespace solarus {

/**
 * \brief This class represents a living Lua context that can execute dynamic
 * scripts at runtime.
 *
 * Such scripts includes menus, map scripts, enemy behaviors, etc.
 * All these scripts run in the same Lua context.
 *
 * On the contrary, data files written in Lua (like maps and dialogs) are
 * always parsed in their own, independent Lua states because data files only
 * have access to a few determined functions.
 */
class LuaContext {

  public:

    // Functions and types.
    static const std::string main_module_name;
    static const std::string audio_module_name;
    static const std::string video_module_name;
    static const std::string input_module_name;
    static const std::string file_module_name;
    static const std::string timer_module_name;
    static const std::string game_module_name;
    static const std::string map_module_name;
    static const std::string item_module_name;
    static const std::string surface_module_name;
    static const std::string text_surface_module_name;
    static const std::string sprite_module_name;
    static const std::string menu_module_name;
    static const std::string language_module_name;
    static const std::string movement_module_name;
    static const std::string movement_straight_module_name;
    static const std::string movement_random_module_name;
    static const std::string movement_target_module_name;
    static const std::string movement_path_module_name;
    static const std::string movement_random_path_module_name;
    static const std::string movement_path_finding_module_name;
    static const std::string movement_circle_module_name;
    static const std::string movement_jump_module_name;
    static const std::string movement_pixel_module_name;

    LuaContext(MainLoop& main_loop);
    ~LuaContext();

    static LuaContext& get_lua_context(lua_State* l);
    lua_State* get_internal_state();

    MainLoop& get_main_loop();

    // Main loop from C++.
    void initialize();
    void exit();
    void update();
    bool notify_input(const InputEvent& event);
    void notify_map_suspended(Map& map, bool suspended);
    void notify_camera_reached_target(Map& map);
    void notify_shop_treasure_interaction(ShopTreasure& shop_treasure);
    void notify_hero_brandish_treasure(
        const Treasure& treasure, int callback_ref);
    bool notify_dialog_started(Game& game, const Dialog& dialog,
        int info_ref);
    void notify_dialog_finished(Game& game, const Dialog& dialog,
        int callback_ref, int status_ref);
    void run_item(EquipmentItem& item);
    void run_map(Map& map, Destination* destination);
    void run_enemy(Enemy& enemy);

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
    void destroy_timers();
    void update_timers();
    void notify_timers_map_suspended(bool suspended);
    void do_timer_callback(Timer& timer);

    // Menus.
    void add_menu(int menu_ref, int context_index, bool on_top);
    void remove_menus(int context_index);
    void remove_menus();
    void destroy_menus();
    void update_menus();

    // Drawable objects.
    bool has_drawable(Drawable* drawable);
    void add_drawable(Drawable* drawable);
    void remove_drawable(Drawable* drawable);
    void destroy_drawables();
    void update_drawables();

    // Movements.
    void start_movement_on_point(Movement& movement, int point_index);
    void stop_movement_on_point(Movement& movement);
    void update_movements();

    // Entities.
    static const std::string& get_entity_type_name(EntityType entity_type);
    static Map& get_entity_creation_map(lua_State* l);
    static Map* get_entity_implicit_creation_map(lua_State* l);
    static void set_entity_implicit_creation_map(lua_State* l, Map* map);

    // Main loop events (sol.main).
    void main_on_started();
    void main_on_finished();
    void main_on_update();
    void main_on_draw(Surface& dst_surface);
    bool main_on_input(const InputEvent& event);

    // Menu events.
    void menu_on_started(int menu_ref);
    void menu_on_finished(int menu_ref);
    void menu_on_update(int menu_ref);
    void menu_on_draw(int menu_ref, Surface& dst_surface);
    bool menu_on_input(int menu_ref, const InputEvent& event);
    bool menu_on_command_pressed(int menu_ref, GameCommands::Command command);
    bool menu_on_command_released(int menu_ref, GameCommands::Command command);
    void menus_on_update(int context_index);
    void menus_on_draw(int context_index, Surface& dst_surface);
    bool menus_on_input(int context_index, const InputEvent& event);
    bool menus_on_command_pressed(int context_index, GameCommands::Command command);
    bool menus_on_command_released(int context_index, GameCommands::Command command);

    // Sprite events.
    void sprite_on_animation_finished(
        Sprite& sprite, const std::string& animation);
    void sprite_on_animation_changed(
        Sprite& sprite, const std::string& animation);
    void sprite_on_direction_changed(
        Sprite& sprite, const std::string& animation, int direction);
    void sprite_on_frame_changed(
        Sprite& sprite, const std::string& animation, int frame);

    // Movement events.
    void movement_on_position_changed(Movement& movement, const Rectangle& xy);
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
    void item_on_obtaining(EquipmentItem& item, const Treasure& treasure);
    void item_on_obtained(EquipmentItem& item, const Treasure& treasure);
    void item_on_variant_changed(EquipmentItem& item, int variant);
    void item_on_amount_changed(EquipmentItem& item, int amount);
    void item_on_using(EquipmentItem& item);
    void item_on_ability_used(EquipmentItem& item, Ability ability);
    void item_on_npc_interaction(EquipmentItem& item, Npc& npc);
    bool item_on_npc_interaction_item(EquipmentItem& item, Npc& npc,
        EquipmentItem& item_used);
    void item_on_npc_collision_fire(EquipmentItem& item, Npc& npc);

    // Game events.
    void game_on_started(Game& game);
    void game_on_finished(Game& game);
    void game_on_update(Game& game);
    void game_on_draw(Game& game, Surface& dst_surface);
    void game_on_map_changed(Game& game, Map& map);
    void game_on_paused(Game& game);
    void game_on_unpaused(Game& game);
    bool game_on_dialog_started(Game& game, const Dialog& dialog,
        int info_ref);
    void game_on_dialog_finished(Game& game, const Dialog& dialog);
    bool game_on_game_over_started(Game& game);
    void game_on_game_over_finished(Game& game);
    bool game_on_input(Game& game, const InputEvent& event);
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
    bool map_on_input(Map& map, const InputEvent& event);
    bool map_on_command_pressed(Map& map, GameCommands::Command command);
    bool map_on_command_released(Map& map, GameCommands::Command command);

    // Map entity events.
    // TODO entity_on_created
    void entity_on_removed(MapEntity& entity);
    void entity_on_position_changed(MapEntity& entity, const Rectangle& xy, Layer layer);
    void entity_on_obstacle_reached(MapEntity& entity, Movement& movement);
    void entity_on_movement_changed(MapEntity& entity, Movement& movement);
    void entity_on_movement_finished(MapEntity& entity);
    void hero_on_state_changed(Hero& hero, const std::string& state_name);
    void destination_on_activated(Destination& destination);
    void teletransporter_on_activated(Teletransporter& teletransporter);
    void npc_on_interaction(Npc& npc);
    bool npc_on_interaction_item(Npc& npc, EquipmentItem& item_used);
    void npc_on_collision_fire(Npc& npc);
    bool chest_on_empty(Chest& chest);
    void block_on_moving(Block& block);
    void block_on_moved(Block& block);
    void switch_on_activated(Switch& sw);
    void switch_on_inactivated(Switch& sw);
    void switch_on_left(Switch& sw);
    void sensor_on_activated(Sensor& sensor);
    void sensor_on_activated_repeat(Sensor& sensor);
    void sensor_on_left(Sensor& sensor);
    void sensor_on_collision_explosion(Sensor& sensor);
    void separator_on_activating(Separator& separator, int direction4);
    void separator_on_activated(Separator& separator, int direction4);
    void door_on_opened(Door& door);
    void door_on_closed(Door& door);
    bool shop_treasure_on_buying(ShopTreasure& shop_treasure);
    void shop_treasure_on_bought(ShopTreasure& shop_treasure);
    void destructible_on_looked(Destructible& destructible);
    void destructible_on_cut(Destructible& destructible);
    void destructible_on_lifting(Destructible& destructible);
    void destructible_on_exploded(Destructible& destructible);
    void destructible_on_regenerating(Destructible& destructible);
    void enemy_on_update(Enemy& enemy);
    void enemy_on_suspended(Enemy& enemy, bool suspended);
    void enemy_on_created(Enemy& enemy);  // TODO remove?
    void enemy_on_enabled(Enemy& enemy);
    void enemy_on_disabled(Enemy& enemy);
    void enemy_on_restarted(Enemy& enemy);
    void enemy_on_pre_draw(Enemy& enemy);
    void enemy_on_post_draw(Enemy& enemy);
    void enemy_on_collision_enemy(Enemy& enemy,
        Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite);
    void enemy_on_custom_attack_received(Enemy& enemy,
        EnemyAttack attack, Sprite* sprite);
    void enemy_on_hurt(Enemy& enemy, EnemyAttack attack, int life_lost);
    void enemy_on_dying(Enemy& enemy);
    void enemy_on_dead(Enemy& enemy);
    void enemy_on_immobilized(Enemy& enemy);
    bool enemy_on_attacking_hero(Enemy& enemy, Hero& hero, Sprite* enemy_sprite);

    // Implementation of the API.

    /**
     * \brief Type of the functions that can be called by Lua.
     */
    typedef int (FunctionExportedToLua) (lua_State* l);

    // All functions named <type>_api_<name> can be called by Lua.
    static FunctionExportedToLua

      // Main API.
      main_api_load_file,
      main_api_do_file,
      main_api_reset,
      main_api_exit,
      main_api_get_elapsed_time,
      main_api_get_quest_write_dir,
      main_api_set_quest_write_dir,
      main_api_load_settings,
      main_api_save_settings,
      main_api_get_distance,  // TODO remove?
      main_api_get_angle,     // TODO remove?
      main_api_get_metatable,

      // Audio API.
      audio_api_get_sound_volume,
      audio_api_set_sound_volume,
      audio_api_play_sound,
      audio_api_preload_sounds,
      audio_api_get_music_volume,
      audio_api_set_music_volume,
      audio_api_play_music,
      audio_api_stop_music,
      audio_api_get_music,
      audio_api_get_music_format,
      audio_api_get_music_num_channels,
      audio_api_get_music_channel_volume,
      audio_api_set_music_channel_volume,
      audio_api_get_music_tempo,
      audio_api_set_music_tempo,

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
      video_api_get_quest_size,
      video_api_get_window_size,
      video_api_set_window_size,
      video_api_reset_window_size,

      // Input API.
      input_api_is_joypad_enabled,
      input_api_set_joypad_enabled,
      input_api_is_key_pressed,
      input_api_get_key_modifiers,
      input_api_is_joypad_button_pressed,
      input_api_get_joypad_axis_state,
      input_api_get_joypad_hat_direction,

      // File API.
      file_api_open,
      file_api_exists,
      file_api_remove,
      file_api_mkdir,

      // Menu API.
      menu_api_start,
      menu_api_stop,
      menu_api_stop_all,
      menu_api_is_started,

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
      timer_api_get_remaining_time,
      timer_api_set_remaining_time,
      // TODO remove is_with_sound, set_with_sound (do this in pure Lua, possibly with a second timer)

      // Language API.
      language_api_get_language,
      language_api_set_language,
      language_api_get_language_name,
      language_api_get_languages,
      language_api_get_string,
      language_api_get_dialog,

      // Drawable API (i.e. common to surfaces, text surfaces and sprites).
      drawable_api_draw,
      drawable_api_draw_region,
      drawable_api_fade_in,
      drawable_api_fade_out,
      drawable_api_get_xy,
      drawable_api_set_xy,
      drawable_api_get_movement,
      drawable_api_stop_movement,
      drawable_meta_gc,

      // Surface API.
      surface_api_create,
      surface_api_get_size,
      surface_api_clear,
      surface_api_fill_color,
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
      text_surface_api_get_color,
      text_surface_api_set_color,
      text_surface_api_get_text,
      text_surface_api_set_text,
      text_surface_api_set_text_key,
      text_surface_api_get_size,

      // Sprite API.
      sprite_api_create,
      sprite_api_get_animation,
      sprite_api_set_animation,  // TODO allow to pass the on_animation_finished callback as a parameter?
      sprite_api_get_direction,
      sprite_api_set_direction,
      sprite_api_get_frame,
      sprite_api_set_frame,
      sprite_api_get_frame_delay,
      sprite_api_set_frame_delay,
      sprite_api_is_paused,
      sprite_api_set_paused,
      sprite_api_set_ignore_suspend,  // TODO rename to set_suspended_with_map() like timers
      sprite_api_synchronize,

      // Movement API.
      movement_api_create,
      movement_api_get_xy,
      movement_api_set_xy,
      movement_api_get_ignore_obstacles,
      movement_api_set_ignore_obstacles,
      movement_api_start,
      movement_api_stop,
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
      game_api_is_pause_allowed,
      game_api_set_pause_allowed,
      game_api_is_dialog_enabled,
      game_api_start_dialog,
      game_api_stop_dialog,
      game_api_is_game_over_enabled,
      game_api_start_game_over,
      game_api_stop_game_over,
      game_api_get_map,
      game_api_get_hero,
      game_api_get_value,
      game_api_set_value,
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
      game_api_has_item,
      game_api_get_item_assigned,
      game_api_set_item_assigned,
      game_api_is_command_pressed,
      game_api_get_commands_direction,
      game_api_get_command_effect,  // TODO also return "run" for action command
      game_api_get_command_keyboard_binding,
      game_api_set_command_keyboard_binding,
      game_api_get_command_joypad_binding,
      game_api_set_command_joypad_binding,
      game_api_capture_command_binding,
      game_api_simulate_command_pressed,
      game_api_simulate_command_released,

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
      map_api_get_music,
      map_api_get_camera_position,
      map_api_move_camera,  // TODO set any movement to the camera instead
      map_api_get_ground,
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
      map_api_get_hero,
      map_api_set_entities_enabled,
      map_api_remove_entities,
      map_api_create_tile,
      map_api_create_destination,
      map_api_create_teletransporter,
      map_api_create_pickable,
      map_api_create_destructible,
      map_api_create_chest,
      map_api_create_jumper,
      map_api_create_enemy,
      map_api_create_npc,  // TODO use a real string for the subtype, improve the behavior syntax
      map_api_create_block,  // TODO make maximum_moves really be the limit (nil means no limit)
      map_api_create_dynamic_tile,
      map_api_create_switch,
      map_api_create_wall,
      map_api_create_sensor,
      map_api_create_crystal,
      map_api_create_crystal_block,
      map_api_create_shop_treasure,
      map_api_create_conveyor_belt,
      map_api_create_door,
      map_api_create_stairs,
      map_api_create_separator,
      map_api_create_custom_entity,
      map_api_create_bomb,
      map_api_create_explosion,
      map_api_create_fire,

      // Map entity API.
      entity_api_get_type,
      entity_api_get_map,
      entity_api_get_game,
      entity_api_get_name,
      entity_api_exists,
      entity_api_remove,
      entity_api_is_enabled,
      entity_api_set_enabled,
      entity_api_get_size,
      entity_api_set_size,
      entity_api_get_origin,
      entity_api_set_origin,
      entity_api_get_position,
      entity_api_set_position,
      entity_api_get_center_position,
      entity_api_get_distance,
      entity_api_get_angle,
      entity_api_get_direction4_to,
      entity_api_get_direction8_to,
      entity_api_bring_to_front,
      entity_api_bring_to_back,
      entity_api_snap_to_grid,
      entity_api_get_sprite,
      entity_api_create_sprite,
      entity_api_remove_sprite,
      entity_api_is_visible,
      entity_api_set_visible,
      entity_api_get_movement,
      entity_api_stop_movement,
      entity_api_has_layer_independent_collisions,
      entity_api_set_layer_independent_collisions,
      entity_api_test_obstacles,
      entity_api_get_optimization_distance,
      entity_api_set_optimization_distance,
      entity_api_is_in_same_region,
      hero_api_teleport,
      hero_api_get_direction,
      hero_api_set_direction,
      hero_api_get_walking_speed,
      hero_api_set_walking_speed,
      hero_api_save_solid_ground,
      hero_api_reset_solid_ground,
      hero_api_get_state,
      hero_api_freeze,
      hero_api_unfreeze,
      hero_api_walk,
      hero_api_start_jumping,
      hero_api_start_treasure,
      hero_api_start_victory,
      hero_api_start_item,
      hero_api_start_boomerang,
      hero_api_start_bow,
      hero_api_start_hookshot,
      hero_api_start_running,
      hero_api_start_hurt,
      teletransporter_api_get_sound,
      teletransporter_api_set_sound,
      teletransporter_api_get_transition,
      teletransporter_api_set_transition,
      teletransporter_api_get_destination_map,
      teletransporter_api_set_destination_map,
      teletransporter_api_get_destination_name,
      teletransporter_api_set_destination_name,
      chest_api_is_open,
      chest_api_set_open,
      block_api_reset,
      block_api_is_pushable,
      block_api_set_pushable,
      block_api_is_pullable,
      block_api_set_pullable,
      block_api_get_maximum_moves,
      block_api_set_maximum_moves,
      switch_api_is_activated,
      switch_api_set_activated,
      switch_api_set_locked,
      door_api_is_open,
      door_api_is_opening,
      door_api_is_closed,
      door_api_is_closing,
      pickable_api_get_followed_entity,
      pickable_api_get_falling_height,
      pickable_api_get_treasure,
      destructible_api_get_treasure,
      destructible_api_set_treasure,
      destructible_api_get_destruction_sound,
      destructible_api_set_destruction_sound,
      destructible_api_get_weight,
      destructible_api_set_weight,
      destructible_api_get_can_be_cut,
      destructible_api_set_can_be_cut,
      destructible_api_get_can_explode,
      destructible_api_set_can_explode,
      destructible_api_get_can_regenerate,
      destructible_api_set_can_regenerate,
      destructible_api_get_damage_on_enemies,
      destructible_api_set_damage_on_enemies,
      destructible_api_get_modified_ground,
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
      enemy_api_is_traversable,
      enemy_api_set_traversable,
      enemy_api_get_obstacle_behavior,
      enemy_api_set_obstacle_behavior,
      enemy_api_restart,
      enemy_api_hurt,
      enemy_api_immobilize,
      enemy_api_create_enemy,
      custom_entity_api_get_model,

      // available to all userdata types
      userdata_meta_gc,
      userdata_meta_newindex_as_table,
      userdata_meta_index_as_table;

  private:

    /**
     * \brief Data associated to any Lua menu.
     */
    struct LuaMenuData {
      int ref;               /**< Lua ref of the table of the menu.
                              * LUA_REFNIL means that the menu will be removed. */
      const void* context;   /**< Lua table or userdata the menu is attached to. */
      bool recently_added;   /**< Used to avoid elements added during an iteration. */

      LuaMenuData(int ref, const void* context):
        ref(ref),
        context(context),
        recently_added(true) {
      }
    };

    /**
     * \brief Data associated to any Lua timer.
     */
    struct LuaTimerData {
      int callback_ref;     /**< Lua ref of the function to call after the timer. */
      const void* context;  /**< Lua table or userdata the timer is attached to. */
    };

    // Executing Lua code.
    bool userdata_has_field(
        const ExportableToLua& userdata, const char* key) const;
    bool userdata_has_field(
        const ExportableToLua& userdata, const std::string& key) const;
    bool userdata_has_metafield(
        const ExportableToLua& userdata, const char* key) const;
    bool find_method(int index, const char* function_name);
    bool find_method(const char* function_name);
    bool call_function(
        int nb_arguments,
        int nb_results,
        const char* function_name);
    static bool call_function(
        lua_State* l,
        int nb_arguments,
        int nb_results,
        const char* function_name);
    static void load_file(lua_State* l, const std::string& script_name);
    static bool load_file_if_exists(lua_State* l, const std::string& script_name);
    static void do_file(lua_State* l, const std::string& script_name);
    static bool do_file_if_exists(lua_State* l, const std::string& script_name);
    void print_stack(lua_State* l);

    // Initialization of modules.
    void register_functions(
        const std::string& module_name,
        const luaL_Reg* functions
    );
    void register_type(
        const std::string& module_name,
        const luaL_Reg* functions,
        const luaL_Reg* methods,
        const luaL_Reg* metamethods
    );
    void register_modules();
    void register_main_module();
    void register_audio_module();
    void register_video_module();
    void register_input_module();
    void register_file_module();
    void register_timer_module();
    void register_item_module();
    void register_surface_module();
    void register_text_surface_module();
    void register_sprite_module();
    void register_movement_module();
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
    static void push_dialog(lua_State* l, const Dialog& dialog);
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
    static void push_npc(lua_State* l, Npc& npc);
    static void push_teletransporter(lua_State* l, Teletransporter& teletransporter);
    static void push_chest(lua_State* l, Chest& chest);
    static void push_block(lua_State* l, Block& block);
    static void push_switch(lua_State* l, Switch& sw);
    static void push_door(lua_State* l, Door& door);
    static void push_shop_treasure(lua_State* l, ShopTreasure& shop_treasure);
    static void push_pickable(lua_State* l, Pickable& pickable);
    static void push_destructible(lua_State* l, Destructible& destructible);
    static void push_enemy(lua_State* l, Enemy& enemy);
    static void push_custom_entity(lua_State* l, CustomEntity& entity);

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
    static bool is_teletransporter(lua_State* l, int index);
    static Teletransporter& check_teletransporter(lua_State* l, int index);
    static bool is_npc(lua_State* l, int index);
    static Npc& check_npc(lua_State* l, int index);
    static bool is_chest(lua_State* l, int index);
    static Chest& check_chest(lua_State* l, int index);
    static bool is_block(lua_State* l, int index);
    static Block& check_block(lua_State* l, int index);
    static bool is_switch(lua_State* l, int index);
    static Switch& check_switch(lua_State* l, int index);
    static bool is_door(lua_State* l, int index);
    static Door& check_door(lua_State* l, int index);
    static bool is_shop_treasure(lua_State* l, int index);
    static ShopTreasure& check_shop_treasure(lua_State* l, int index);
    static bool is_pickable(lua_State* l, int index);
    static Pickable& check_pickable(lua_State* l, int index);
    static bool is_destructible(lua_State* l, int index);
    static Destructible& check_destructible(lua_State* l, int index);
    static bool is_enemy(lua_State* l, int index);
    static Enemy& check_enemy(lua_State* l, int index);
    static bool is_custom_entity(lua_State* l, int index);
    static CustomEntity& check_custom_entity(lua_State* l, int index);

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
    bool on_dialog_started(const Dialog& dialog, int info_ref);
    void on_dialog_finished(const Dialog& dialog);
    bool on_game_over_started();
    void on_game_over_finished();
    bool on_input(const InputEvent& event);
    bool on_key_pressed(const InputEvent& event);
    bool on_key_released(const InputEvent& event);
    bool on_character_pressed(const InputEvent& event);
    bool on_joypad_button_pressed(const InputEvent& event);
    bool on_joypad_button_released(const InputEvent& event);
    bool on_joypad_axis_moved(const InputEvent& event);
    bool on_joypad_hat_moved(const InputEvent& event);
    bool on_command_pressed(GameCommands::Command command);
    bool on_command_released(GameCommands::Command command);
    void on_animation_finished(const std::string& animation);
    void on_animation_changed(const std::string& animation);
    void on_direction_changed(const std::string& animation, int direction);
    void on_frame_changed(const std::string& animation, int frame);
    void on_position_changed(const Rectangle& xy);
    void on_obstacle_reached();
    void on_changed();
    void on_started(Destination* destination);
    void on_opening_transition_finished(Destination* destination);
    void on_camera_back();
    void on_obtaining_treasure(const Treasure& treasure);
    void on_obtained_treasure(const Treasure& treasure);
    void on_state_changed(const std::string& state_name);
    void on_activating();
    void on_activating(int direction4);
    void on_activated();
    void on_activated(int direction4);
    void on_activated_repeat();
    void on_inactivated();
    void on_left();
    void on_interaction();
    bool on_interaction_item(EquipmentItem& item_used);
    void on_npc_interaction(Npc& npc);
    bool on_npc_interaction_item(Npc& npc, EquipmentItem& item_used);
    void on_npc_collision_fire(Npc& npc);
    void on_collision_fire();
    void on_collision_explosion();
    void on_collision_enemy(Enemy& enemy, Sprite& other_sprite, Sprite& this_sprite);
    bool on_empty();
    bool on_buying();
    void on_bought();
    void on_opened();
    void on_closed();
    void on_moving();
    void on_moved();
    void on_map_changed(Map& map);
    void on_pickable_created(Pickable& pickable);
    void on_variant_changed(int variant);
    void on_amount_changed(int amount);
    void on_obtaining(const Treasure& treasure);
    void on_obtained(const Treasure& treasure);
    void on_using();
    void on_ability_used(Ability ability);
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
    void on_looked();
    void on_cut();
    void on_lifting();
    void on_exploded();
    void on_regenerating();
    void on_custom_attack_received(EnemyAttack attack, Sprite* sprite);
    void on_hurt(EnemyAttack attack, int life_lost);
    void on_dying();
    void on_dead();
    void on_immobilized();
    bool on_attacking_hero(Hero& hero, Sprite* attacker_sprite);

    // Functions exported to Lua for internal needs.
    static FunctionExportedToLua
      l_panic,
      l_loader,
      l_get_map_entity_or_global,
      l_camera_do_callback,
      l_camera_restore,
      l_treasure_dialog_finished,
      l_shop_treasure_description_dialog_finished,
      l_shop_treasure_question_dialog_finished;


    // Script data.
    lua_State* l;                   /**< The Lua state encapsulated. */
    MainLoop& main_loop;            /**< The Solarus main loop. */

    std::list<LuaMenuData> menus;   /**< The menus currently running in their context.
                                     * Invalid ones are to be removed at the next cycle. */
    std::map<Timer*, LuaTimerData>
        timers;                     /**< The timers currently running, with
                                     * their context and callback. */
    std::list<Timer*>
        timers_to_remove;           /**< Timers to be removed at the next cycle. */

    std::set<Drawable*> drawables;  /**< All drawable objects created by
                                     * this script. */
    std::set<Drawable*>
        drawables_to_remove;        /**< Drawable objects to be removed at the
                                     * next cycle. */
    std::map<const ExportableToLua*, std::set<std::string> >
        userdata_fields;            /**< Existing string keys created on each
                                     * userdata with our __newindex. This is
                                     * only for performance, to avoid Lua
                                     * lookups for callbacks like on_update. */

    static std::map<lua_State*, LuaContext*>
        lua_contexts;               /**< Mapping to get the encapsulating object
                                     * from the lua_State pointer. */

    static const std::string enemy_attack_names[];
    static const std::string enemy_hurt_style_names[];
    static const std::string enemy_obstacle_behavior_names[];

};

}

#endif

