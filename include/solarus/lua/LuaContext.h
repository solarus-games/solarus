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

#include "solarus/Common.h"
#include "solarus/GameCommands.h"
#include "solarus/entities/Layer.h"
#include "solarus/entities/EnemyAttack.h"
#include "solarus/entities/EntityType.h"
#include "solarus/entities/Ground.h"
#include "solarus/entities/MapEntityPtr.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/InputEvent.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include "solarus/lua/ExportableToLuaPtr.h"
#include "solarus/lua/ScopedLuaRef.h"
#include "solarus/Ability.h"
#include "solarus/SpritePtr.h"
#include "solarus/TimerPtr.h"
#include <map>
#include <set>
#include <list>

struct lua_State;
struct luaL_Reg;
typedef int (*lua_CFunction) (lua_State* l);

namespace Solarus {

class Block;
class Chest;
class CircleMovement;
class Color;
class CustomEntity;
class Destination;
class Destructible;
class Dialog;
class Door;
class Drawable;
class Enemy;
class EntityData;
class ExportableToLua;
class EquipmentItem;
class Hero;
class Game;
class JumpMovement;
class MainLoop;
class Map;
class MapEntity;
class Movement;
class Npc;
class PathFindingMovement;
class PathMovement;
class PixelMovement;
class Pickable;
class Point;
class RandomMovement;
class RandomPathMovement;
class Sensor;
class Separator;
class ShopTreasure;
class Sprite;
class StraightMovement;
class Stream;
class Switch;
class TargetMovement;
class Teletransporter;
class TextSurface;
class Timer;
class Treasure;

/**
 * \brief This class represents a living Lua context that can execute quest
 * scripts at runtime.
 *
 * Such scripts include the quest main script, map scripts, enemy behaviors,
 * etc. This class implements the Solarus scripting API that scripts can use.
 *
 * All these scripts run in the same Lua context. This means that they share
 * global values and other global mechanisms like the registry and metatables.
 *
 * On the contrary, data files that happen to have a Lua-compatible syntax
 * (like maps) are always parsed in their own, independent Lua states.
 * These files are considered as pure data (not code) and only use the
 * LuaTools class, not this class.
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
        const Treasure& treasure,
        const ScopedLuaRef& callback_ref
    );
    bool notify_dialog_started(
        Game& game,
        const Dialog& dialog,
        const ScopedLuaRef& info_ref
    );
    void notify_dialog_finished(
        Game& game,
        const Dialog& dialog,
        const ScopedLuaRef& callback_ref,
        const ScopedLuaRef& status_ref
    );
    void run_item(EquipmentItem& item);
    void run_map(Map& map, Destination* destination);
    void run_enemy(Enemy& enemy);
    void run_custom_entity(CustomEntity& custom_entity);

    // Lua refs.
    ScopedLuaRef create_ref();
    static void push_ref(lua_State* l, const ScopedLuaRef& ref);

    // Calling Lua functions.
    bool call_function(
        int nb_arguments,
        int nb_results,
        const char* function_name
    );

    bool userdata_has_field(
        const ExportableToLua& userdata,
        const char* key
    ) const;
    bool userdata_has_field(
        const ExportableToLua& userdata,
        const std::string& key
    ) const;

    // Timers.
    void add_timer(
        const TimerPtr& timer,
        int context_index,
        const ScopedLuaRef& callback_index
    );
    void remove_timer(const TimerPtr& timer);
    void remove_timers(int context_index);
    void destroy_timers();
    void update_timers();
    void notify_timers_map_suspended(bool suspended);
    void set_entity_timers_suspended(MapEntity& entity, bool suspended);
    void do_timer_callback(const TimerPtr& timer);

    // Menus.
    void add_menu(
        const ScopedLuaRef& menu_ref,
        int context_index,
        bool on_top
    );
    void remove_menus(int context_index);
    void remove_menus();
    void destroy_menus();
    void update_menus();

    // Drawable objects.
    bool has_drawable(const std::shared_ptr<Drawable>& drawable);
    void add_drawable(const std::shared_ptr<Drawable>& drawable);
    void remove_drawable(const std::shared_ptr<Drawable>& drawable);
    void destroy_drawables();
    void update_drawables();

    // Movements.
    void start_movement_on_point(
        const std::shared_ptr<Movement>& movement,
        int point_index
    );
    void stop_movement_on_point(const std::shared_ptr<Movement>& movement);
    void update_movements();

    // Entities.
    static const std::string& get_entity_internal_type_name(EntityType entity_type);
    void create_map_entity_from_data(Map& map, const EntityData& entity_data);

    bool do_custom_entity_traversable_test_function(
        const ScopedLuaRef& traversable_test_ref,
        CustomEntity& custom_entity,
        MapEntity& other_entity
    );
    bool do_custom_entity_collision_test_function(
        const ScopedLuaRef& collision_test_ref,
        CustomEntity& custom_entity,
        MapEntity& other_entity
    );
    void do_custom_entity_collision_callback(
        const ScopedLuaRef& callback_ref,
        CustomEntity& custom_entity,
        MapEntity& other_entity
    );
    void do_custom_entity_collision_callback(
        const ScopedLuaRef& callback_ref,
        CustomEntity& custom_entity,
        MapEntity& other_entity,
        Sprite& custom_entity_sprite,
        Sprite& other_entity_sprite
    );

    // Main loop events (sol.main).
    void main_on_started();
    void main_on_finished();
    void main_on_update();
    void main_on_draw(const SurfacePtr& dst_surface);
    bool main_on_input(const InputEvent& event);

    // Menu events.
    void menu_on_started(const ScopedLuaRef& menu_ref);
    void menu_on_finished(const ScopedLuaRef& menu_ref);
    void menu_on_update(const ScopedLuaRef& menu_ref);
    void menu_on_draw(const ScopedLuaRef& menu_ref, const SurfacePtr& dst_surface);
    bool menu_on_input(const ScopedLuaRef& menu_ref, const InputEvent& event);
    bool menu_on_command_pressed(
        const ScopedLuaRef& menu_ref,
        GameCommand command
    );
    bool menu_on_command_released(
        const ScopedLuaRef& menu_ref,
        GameCommand command
    );
    void menus_on_update(int context_index);
    void menus_on_draw(int context_index, const SurfacePtr& dst_surface);
    bool menus_on_input(int context_index, const InputEvent& event);
    bool menus_on_command_pressed(int context_index, GameCommand command);
    bool menus_on_command_released(int context_index, GameCommand command);

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
    void movement_on_position_changed(Movement& movement, const Point& xy);
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
    void game_on_draw(Game& game, const SurfacePtr& dst_surface);
    void game_on_map_changed(Game& game, Map& map);
    void game_on_paused(Game& game);
    void game_on_unpaused(Game& game);
    bool game_on_dialog_started(
        Game& game,
        const Dialog& dialog,
        const ScopedLuaRef& info_ref
    );
    void game_on_dialog_finished(Game& game, const Dialog& dialog);
    bool game_on_game_over_started(Game& game);
    void game_on_game_over_finished(Game& game);
    bool game_on_input(Game& game, const InputEvent& event);
    bool game_on_command_pressed(Game& game, GameCommand command);
    bool game_on_command_released(Game& game, GameCommand command);

    // Map events.
    void map_on_started(Map& map, Destination* destination);
    void map_on_finished(Map& map);
    void map_on_update(Map& map);
    void map_on_draw(Map& map, const SurfacePtr& dst_surface);
    void map_on_suspended(Map& map, bool suspended);
    void map_on_opening_transition_finished(Map& map,
        Destination* destination);
    void map_on_camera_back(Map& map);
    void map_on_obtaining_treasure(Map& map, const Treasure& treasure);
    void map_on_obtained_treasure(Map& map, const Treasure& treasure);
    bool map_on_input(Map& map, const InputEvent& event);
    bool map_on_command_pressed(Map& map, GameCommand command);
    bool map_on_command_released(Map& map, GameCommand command);

    // Map entity events.
    void entity_on_update(MapEntity& entity);
    void entity_on_suspended(MapEntity& entity, bool suspended);
    void entity_on_created(MapEntity& entity);
    void entity_on_removed(MapEntity& entity);
    void entity_on_enabled(MapEntity& entity);
    void entity_on_disabled(MapEntity& entity);
    void entity_on_pre_draw(MapEntity& entity);
    void entity_on_post_draw(MapEntity& entity);
    void entity_on_position_changed(MapEntity& entity, const Point& xy, Layer layer);
    void entity_on_obstacle_reached(MapEntity& entity, Movement& movement);
    void entity_on_movement_changed(MapEntity& entity, Movement& movement);
    void entity_on_movement_finished(MapEntity& entity);
    bool entity_on_interaction(MapEntity& entity);
    bool entity_on_interaction_item(MapEntity& entity, EquipmentItem& item_used);
    void hero_on_state_changed(Hero& hero, const std::string& state_name);
    bool hero_on_taking_damage(Hero& hero, int damage);
    void destination_on_activated(Destination& destination);
    void teletransporter_on_activated(Teletransporter& teletransporter);
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
    void enemy_on_restarted(Enemy& enemy);
    void enemy_on_collision_enemy(Enemy& enemy,
        Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite);
    void enemy_on_custom_attack_received(Enemy& enemy,
        EnemyAttack attack, Sprite* sprite);
    bool enemy_on_hurt_by_sword(Enemy& enemy, Hero& hero, Sprite& enemy_sprite);
    void enemy_on_hurt(Enemy& enemy, EnemyAttack attack);
    void enemy_on_dying(Enemy& enemy);
    void enemy_on_dead(Enemy& enemy);
    void enemy_on_immobilized(Enemy& enemy);
    bool enemy_on_attacking_hero(Enemy& enemy, Hero& hero, Sprite* enemy_sprite);
    void custom_entity_on_ground_below_changed(
        CustomEntity& custom_entity, Ground ground_below);

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
      main_api_get_os,

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
      input_api_is_mouse_button_pressed,
      input_api_is_mouse_button_released,
      input_api_get_mouse_position,

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
      surface_api_get_pixels,
      surface_api_set_pixels,

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
      text_surface_api_get_font_size,
      text_surface_api_set_font_size,
      text_surface_api_get_text,
      text_surface_api_set_text,
      text_surface_api_set_text_key,
      text_surface_api_get_size,

      // Sprite API.
      sprite_api_create,
      sprite_api_get_animation_set,
      sprite_api_get_animation,
      sprite_api_set_animation,  // TODO allow to pass the on_animation_finished callback as a parameter?
      sprite_api_has_animation,
      sprite_api_get_direction,
      sprite_api_set_direction,
      sprite_api_get_num_directions,
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
      map_api_create_entity,  // Same function used for all entity types.

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
      entity_api_get_bounding_box,
      entity_api_overlaps,
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
      hero_api_get_solid_ground_position,
      hero_api_get_animation,
      hero_api_set_animation,
      hero_api_get_tunic_sprite_id,
      hero_api_set_tunic_sprite_id,
      hero_api_get_sword_sprite_id,
      hero_api_set_sword_sprite_id,
      hero_api_get_sword_sound_id,
      hero_api_set_sword_sound_id,
      hero_api_get_shield_sprite_id,
      hero_api_set_shield_sprite_id,
      hero_api_is_blinking,
      hero_api_set_blinking,
      hero_api_is_invincible,
      hero_api_set_invincible,
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
      stream_api_get_direction,
      stream_api_set_direction,
      stream_api_get_speed,
      stream_api_set_speed,
      stream_api_get_allow_movement,
      stream_api_set_allow_movement,
      stream_api_get_allow_attack,
      stream_api_set_allow_attack,
      stream_api_get_allow_item,
      stream_api_set_allow_item,
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
      enemy_api_get_attack_consequence,
      enemy_api_set_attack_consequence,
      enemy_api_get_attack_consequence_sprite,
      enemy_api_set_attack_consequence_sprite,
      enemy_api_set_default_attack_consequences,
      enemy_api_set_default_attack_consequences_sprite,
      enemy_api_set_invincible,
      enemy_api_set_invincible_sprite,
      enemy_api_get_treasure,
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
      custom_entity_api_get_direction,
      custom_entity_api_set_direction,
      custom_entity_api_is_drawn_in_y_order,
      custom_entity_api_set_drawn_in_y_order,
      custom_entity_api_set_traversable_by,
      custom_entity_api_set_can_traverse,
      custom_entity_api_can_traverse_ground,
      custom_entity_api_set_can_traverse_ground,
      custom_entity_api_add_collision_test,
      custom_entity_api_clear_collision_tests,
      custom_entity_api_get_modified_ground,
      custom_entity_api_set_modified_ground,

      // available to all userdata types
      userdata_meta_gc,
      userdata_meta_newindex_as_table,
      userdata_meta_index_as_table;

  private:

    /**
     * \brief Data associated to any Lua menu.
     */
    struct LuaMenuData {
      ScopedLuaRef ref;      /**< Lua ref of the table of the menu.
                              * LUA_REFNIL means that the menu will be removed. */
      const void* context;   /**< Lua table or userdata the menu is attached to. */
      bool recently_added;   /**< Used to avoid elements added during an iteration. */

      LuaMenuData(const ScopedLuaRef& ref, const void* context):
        ref(ref),
        context(context),
        recently_added(true) {
      }
    };

    /**
     * \brief Data associated to any Lua timer.
     */
    struct LuaTimerData {
      ScopedLuaRef callback_ref;  /**< Lua ref of the function to call after the timer. */
      const void* context;        /**< Lua table or userdata the timer is attached to. */
    };

    // Executing Lua code.
    bool userdata_has_metafield(
        const ExportableToLua& userdata, const char* key) const;
    bool find_method(int index, const char* function_name);
    bool find_method(const char* function_name);
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
    void register_testing_module();

    // Pushing objects to Lua.
    static void push_main(lua_State* l);
    static void push_string(lua_State* l, const std::string& text);
    static void push_color(lua_State* l, const Color& color);
    static void push_userdata(lua_State* l, ExportableToLua& userdata);
    static void push_dialog(lua_State* l, const Dialog& dialog);
    static void push_timer(lua_State* l, const TimerPtr& timer);
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
    static void push_stream(lua_State* l, Stream& stream);
    static void push_door(lua_State* l, Door& door);
    static void push_shop_treasure(lua_State* l, ShopTreasure& shop_treasure);
    static void push_pickable(lua_State* l, Pickable& pickable);
    static void push_destructible(lua_State* l, Destructible& destructible);
    static void push_enemy(lua_State* l, Enemy& enemy);
    static void push_custom_entity(lua_State* l, CustomEntity& entity);

    // Getting userdata objects from Lua.
    static bool is_userdata(lua_State* l, int index,
        const std::string& module_name);
    static const ExportableToLuaPtr& check_userdata(
        lua_State* l,
        int index,
        const std::string& module_name
    );
    static bool is_timer(lua_State* l, int index);
    static TimerPtr check_timer(lua_State* l, int index);
    static bool is_drawable(lua_State* l, int index);
    static std::shared_ptr<Drawable> check_drawable(lua_State* l, int index);
    static bool is_surface(lua_State* l, int index);
    static SurfacePtr check_surface(lua_State* l, int index);
    static bool is_text_surface(lua_State* l, int index);
    static std::shared_ptr<TextSurface> check_text_surface(lua_State* l, int index);
    static bool is_sprite(lua_State* l, int index);
    static SpritePtr check_sprite(lua_State* l, int index);
    static bool is_item(lua_State* l, int index);
    static std::shared_ptr<EquipmentItem> check_item(lua_State* l, int index);
    static bool is_movement(lua_State* l, int index);
    static std::shared_ptr<Movement> check_movement(lua_State* l, int index);
    static bool is_straight_movement(lua_State* l, int index);
    static std::shared_ptr<StraightMovement> check_straight_movement(lua_State* l, int index);
    static bool is_random_movement(lua_State* l, int index);
    static std::shared_ptr<RandomMovement> check_random_movement(lua_State* l, int index);
    static bool is_target_movement(lua_State* l, int index);
    static std::shared_ptr<TargetMovement> check_target_movement(lua_State* l, int index);
    static bool is_path_movement(lua_State* l, int index);
    static std::shared_ptr<PathMovement> check_path_movement(lua_State* l, int index);
    static bool is_random_path_movement(lua_State* l, int index);
    static std::shared_ptr<RandomPathMovement> check_random_path_movement(lua_State* l, int index);
    static bool is_path_finding_movement(lua_State* l, int index);
    static std::shared_ptr<PathFindingMovement> check_path_finding_movement(lua_State* l, int index);
    static bool is_circle_movement(lua_State* l, int index);
    static std::shared_ptr<CircleMovement> check_circle_movement(lua_State* l, int index);
    static bool is_jump_movement(lua_State* l, int index);
    static std::shared_ptr<JumpMovement> check_jump_movement(lua_State* l, int index);
    static bool is_pixel_movement(lua_State* l, int index);
    static std::shared_ptr<PixelMovement> check_pixel_movement(lua_State* l, int index);
    static bool is_game(lua_State* l, int index);
    static std::shared_ptr<Savegame> check_game(lua_State* l, int index);
    static bool is_map(lua_State* l, int index);
    static std::shared_ptr<Map> check_map(lua_State* l, int index);
    static bool is_entity(lua_State* l, int index);
    static MapEntityPtr check_entity(lua_State* l, int index);
    static bool is_hero(lua_State* l, int index);
    static std::shared_ptr<Hero> check_hero(lua_State* l, int index);
    static bool is_teletransporter(lua_State* l, int index);
    static std::shared_ptr<Teletransporter> check_teletransporter(lua_State* l, int index);
    static bool is_npc(lua_State* l, int index);
    static std::shared_ptr<Npc> check_npc(lua_State* l, int index);
    static bool is_chest(lua_State* l, int index);
    static std::shared_ptr<Chest> check_chest(lua_State* l, int index);
    static bool is_block(lua_State* l, int index);
    static std::shared_ptr<Block> check_block(lua_State* l, int index);
    static bool is_switch(lua_State* l, int index);
    static std::shared_ptr<Switch> check_switch(lua_State* l, int index);
    static bool is_stream(lua_State* l, int index);
    static std::shared_ptr<Stream> check_stream(lua_State* l, int index);
    static bool is_door(lua_State* l, int index);
    static std::shared_ptr<Door> check_door(lua_State* l, int index);
    static bool is_shop_treasure(lua_State* l, int index);
    static std::shared_ptr<ShopTreasure> check_shop_treasure(lua_State* l, int index);
    static bool is_pickable(lua_State* l, int index);
    static std::shared_ptr<Pickable> check_pickable(lua_State* l, int index);
    static bool is_destructible(lua_State* l, int index);
    static std::shared_ptr<Destructible> check_destructible(lua_State* l, int index);
    static bool is_enemy(lua_State* l, int index);
    static std::shared_ptr<Enemy> check_enemy(lua_State* l, int index);
    static bool is_custom_entity(lua_State* l, int index);
    static std::shared_ptr<CustomEntity> check_custom_entity(lua_State* l, int index);

    // Events.
    void on_started();
    void on_finished();
    void on_update();
    void on_draw(const SurfacePtr& dst_surface);
    void on_pre_draw(const SurfacePtr& dst_surface);
    void on_post_draw(const SurfacePtr& dst_surface);
    void on_suspended(bool suspended);
    void on_paused();
    void on_unpaused();
    bool on_dialog_started(const Dialog& dialog, const ScopedLuaRef& info_ref);
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
    bool on_mouse_button_pressed(const InputEvent& event);
    bool on_mouse_button_released(const InputEvent& event);
    bool on_command_pressed(GameCommand command);
    bool on_command_released(GameCommand command);
    void on_animation_finished(const std::string& animation);
    void on_animation_changed(const std::string& animation);
    void on_direction_changed(const std::string& animation, int direction);
    void on_frame_changed(const std::string& animation, int frame);
    void on_position_changed(const Point& xy);
    void on_obstacle_reached();
    void on_changed();
    void on_started(Destination* destination);
    void on_opening_transition_finished(Destination* destination);
    void on_camera_back();
    void on_obtaining_treasure(const Treasure& treasure);
    void on_obtained_treasure(const Treasure& treasure);
    void on_state_changed(const std::string& state_name);
    bool on_taking_damage(int damage);
    void on_activating();
    void on_activating(int direction4);
    void on_activated();
    void on_activated(int direction4);
    void on_activated_repeat();
    void on_inactivated();
    void on_left();
    bool on_interaction();
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
    void on_position_changed(const Point& xy, Layer layer);
    void on_obstacle_reached(Movement& movement);
    void on_movement_changed(Movement& movement);
    void on_movement_finished();
    void on_looked();
    void on_cut();
    void on_lifting();
    void on_exploded();
    void on_regenerating();
    void on_custom_attack_received(EnemyAttack attack, Sprite* sprite);
    bool on_hurt_by_sword(Hero& hero, Sprite& enemy_sprite);
    void on_hurt(EnemyAttack attack);
    void on_dying();
    void on_dead();
    void on_immobilized();
    bool on_attacking_hero(Hero& hero, Sprite* attacker_sprite);
    void on_ground_below_changed(Ground ground_below);

    // Functions exported to Lua for internal needs.
    static FunctionExportedToLua
      l_panic,
      l_loader,
      l_get_map_entity_or_global,
      l_camera_do_callback,
      l_camera_restore,
      l_treasure_dialog_finished,
      l_shop_treasure_description_dialog_finished,
      l_shop_treasure_question_dialog_finished,
      l_create_tile,
      l_create_destination,
      l_create_teletransporter,
      l_create_pickable,
      l_create_destructible,
      l_create_chest,
      l_create_jumper,
      l_create_enemy,
      l_create_npc,
      l_create_block,
      l_create_dynamic_tile,
      l_create_switch,
      l_create_wall,
      l_create_sensor,
      l_create_crystal,
      l_create_crystal_block,
      l_create_shop_treasure,
      l_create_stream,
      l_create_door,
      l_create_stairs,
      l_create_separator,
      l_create_custom_entity,
      l_create_bomb,
      l_create_explosion,
      l_create_fire;

    // Script data.
    lua_State* l;                   /**< The Lua state encapsulated. */
    MainLoop& main_loop;            /**< The Solarus main loop. */

    std::list<LuaMenuData> menus;   /**< The menus currently running in their context.
                                     * Invalid ones are to be removed at the next cycle. */
    std::map<TimerPtr, LuaTimerData>
        timers;                     /**< The timers currently running, with
                                     * their context and callback. */
    std::list<TimerPtr>
        timers_to_remove;           /**< Timers to be removed at the next cycle. */

    std::set<std::shared_ptr<Drawable>>
        drawables;                  /**< All drawable objects created by
                                     * this script. */
    std::set<std::shared_ptr<Drawable>>
        drawables_to_remove;        /**< Drawable objects to be removed at the
                                     * next cycle. */
    std::map<const ExportableToLua*, std::set<std::string>>
        userdata_fields;            /**< Existing string keys created on each
                                     * userdata with our __newindex. This is
                                     * only for performance, to avoid Lua
                                     * lookups for callbacks like on_update. */

    static const std::map<EntityType, lua_CFunction>
        entity_creation_functions;  /**< Creation function of each entity type. */
    static std::map<lua_State*, LuaContext*>
        lua_contexts;               /**< Mapping to get the encapsulating object
                                     * from the lua_State pointer. */

};

}

#endif

