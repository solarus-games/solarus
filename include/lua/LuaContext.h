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
#include "entities/Layer.h"
#include "entities/EnemyAttack.h"
#include "lowlevel/InputEvent.h"
#include <map>
#include <set>

struct lua_State;
struct luaL_Reg;

/**
 * @brief This class represents the only Lua context that runs
 * dynamic scripts at runtime.
 *
 * Such scripts includes map scripts, enemy scripts, menu scripts, etc.
 * Data files are not managed by this class even if they are written in Lua.
 */
class LuaContext {

  public:

    // functions and types
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
    static const std::string entity_enemy_module_name;           /**< sol.entity.enemy */

    LuaContext(MainLoop& main_loop);
    ~LuaContext();

    static LuaContext& get_lua_context(lua_State* l);

    MainLoop& get_main_loop();
    Game* get_current_game();
    void set_current_game(Game* current_game);
    CustomScreen* get_current_screen();
    void set_current_screen(CustomScreen* current_screen);

    // Main loop from C++.
    void initialize();
    void exit();
    void update();
    void set_suspended(bool suspended);
    void notify_input(InputEvent& event);
    void notify_map_started(Map& map, Destination* destination);
    void notify_map_suspended(Map& map, bool suspended);
    void notify_item_created(EquipmentItem& item);
    void notify_enemy_created(Enemy& enemy);
    void notify_camera_reached_target(Map& map);
    void notify_dialog_finished(int callback_ref, int answer);

    // Lua refs.
    int create_ref();
    void destroy_ref(int ref);
    void push_callback(int callback_ref);
    void do_callback(int callback_ref);
    void cancel_callback(int callback_ref);

    // Timers.
    bool is_new_timer_suspended(void);
    void add_timer(Timer* timer, int context_index, int callback_index);
    void remove_timer(Timer* timer);
    void remove_timers(int context_index);
    void remove_timers();
    void update_timers();
    void set_suspended_timers(bool suspended);

    // Drawable objects.
    bool has_drawable(Drawable* drawable);
    void add_drawable(Drawable* drawable);
    void remove_drawable(Drawable* drawable);
    void update_drawables();

    // Input.
    static const std::string& input_get_key_name(InputEvent::KeyboardKey key);

    // Main loop events (sol.main).
    void main_on_update();
    void main_on_input(InputEvent& event);
    void main_on_started();
    void main_on_finished();

    // Menu events.
    void menu_on_update(int menu_ref);
    void menu_on_draw(int menu_ref, Surface& dst_surface);
    void menu_on_input(int menu_ref, InputEvent& event);
    void menu_on_started(int menu_ref);
    void menu_on_finished(int menu_ref);

    // Equipment item events.
    void item_on_update(EquipmentItem& item);
    void item_on_suspended(EquipmentItem& item, bool suspended);
    void item_on_map_changed(EquipmentItem& item, Map& map);
    void item_on_appear(EquipmentItem& item, Pickable& pickable);
    void item_on_movement_changed(EquipmentItem& item, Pickable& pickable);
    void item_on_obtaining(EquipmentItem& item, const Treasure& treasure);
    void item_on_obtained(EquipmentItem& item, const Treasure& treasure);
    void item_on_variant_changed(EquipmentItem& item, int variant);
    void item_on_amount_changed(EquipmentItem& item, int amount);
    void item_on_use(EquipmentItem& item, InventoryItem& inventory_item);
    void item_on_ability_used(EquipmentItem& item, const std::string& ability_name);
    void item_on_npc_interaction(EquipmentItem& item, NPC& npc);
    bool item_on_npc_interaction_item(EquipmentItem& item, NPC& npc,
        const std::string& item_name, int variant);
    void item_on_npc_collision_fire(EquipmentItem& item, NPC& npc);

    // Game events.
    void game_on_update(Game& game);
    void game_on_pre_draw(Game& game, Surface& dst_surface);
    void game_on_post_draw(Game& game, Surface& dst_surface);
    void game_on_started(Game& game);
    void game_on_finished(Game& game);

    // Map events.
    void map_on_update(Map& map);
    void map_on_suspended(Map& map, bool suspended);
    void map_on_started(Map& map, Destination* destination);
    void map_on_finished(Map& map);
    void map_on_opening_transition_finished(Map& map,
        Destination* destination);
    void map_on_camera_back(Map& map);

    // Map entity events.
    void switch_on_activated(Switch& sw);
    void switch_on_inactivated(Switch& sw);
    void switch_on_left(Switch& sw);
    void hero_on_obtaining_treasure(Hero& hero, const Treasure& treasure);
    void hero_on_obtained_treasure(Hero& hero, const Treasure& treasure);
    void hero_on_victory_finished(Hero& hero);  // TODO clear the hero events when changing map?
    void sensor_on_activated(Sensor& sensor);
    void sensor_on_activated_repeat(Sensor& sensor);
    void sensor_on_collision_explosion(Sensor& sensor);
    void npc_on_movement_finished(NPC& npc);
    void npc_on_interaction(NPC& npc);
    void npc_on_interaction_finished(NPC& npc);
    bool npc_on_interaction_item(NPC& npc,
        const std::string& item_name, int variant);
    void npc_on_interaction_item_finished(NPC& npc,
        const std::string& item_name, int variant);
    void npc_on_collision_fire(NPC& npc);
    bool chest_on_empty(Chest& chest);
    bool shop_item_on_buying(ShopItem& shop_item);
    void shop_item_on_bought(ShopItem& shop_item);
    void door_on_open(Door& door);
    void door_on_closed(Door& door);
    void block_on_moved(Block& block);
    void enemy_on_update(Enemy& enemy);
    void enemy_on_suspended(Enemy& enemy, bool suspended);
    void enemy_on_appear(Enemy& enemy);
    void enemy_on_enabled(Enemy& enemy);
    void enemy_on_disabled(Enemy& enemy);
    void enemy_on_restart(Enemy& enemy);
    void enemy_on_pre_draw(Enemy& enemy);
    void enemy_on_post_draw(Enemy& enemy);
    void enemy_on_position_changed(Enemy& enemy, const Rectangle& xy, Layer layer);
    void enemy_on_layer_changed(Enemy& enemy, Layer layer);
    void enemy_on_obstacle_reached(Enemy& enemy);
    void enemy_on_movement_changed(Enemy& enemy, Movement& movement);
    void enemy_on_movement_finished(Enemy& enemy);
    void enemy_on_sprite_animation_finished(Enemy& enemy,
        Sprite& sprite, const std::string& animation);
    void enemy_on_sprite_frame_changed(Enemy& enemy,
        Sprite& sprite, const std::string& animation, int frame);
    void enemy_on_collision_enemy(Enemy& enemy,
        Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite);
    void enemy_on_custom_attack_received(Enemy& enemy,
        EnemyAttack attack, Sprite* sprite);
    void enemy_on_hurt(Enemy& enemy, EnemyAttack attack, int life_lost);
    void enemy_on_dying(Enemy& enemy);
    void enemy_on_dead(Enemy& enemy);
    void enemy_on_immobilized(Enemy& enemy);
    void enemy_on_message_received(Enemy& enemy,
        Enemy& src_enemy, const std::string& message);

  private:

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

    // Lua helper functions.
    static int get_positive_index(lua_State* l, int index);
    void print_stack();

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
    static void push_enemy(lua_State* l, Enemy& enemy);

    // Getting objects from Lua.
    static bool is_userdata(lua_State* l, int index,
        const std::string& module_name);
    static ExportableToLua& check_userdata(lua_State* l, int index,
        const std::string& module_name);
    static Color check_color(lua_State* l, int index);
    static Timer& check_timer(lua_State* l, int index);
    static Drawable& check_drawable(lua_State* l, int index);
    static Surface& check_surface(lua_State* l, int index);
    static TextSurface& check_text_surface(lua_State* l, int index);
    static Sprite& check_sprite(lua_State* l, int index);
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
    static Savegame& check_game(lua_State* l, int index);
    static Map& check_map(lua_State* l, int index);
    static MapEntity& check_entity(lua_State* l, int index);
    static Hero& check_hero(lua_State* l, int index);
    static NPC& check_npc(lua_State* l, int index);
    static Chest& check_chest(lua_State* l, int index);
    static Block& check_block(lua_State* l, int index);
    static Switch& check_switch(lua_State* l, int index);
    static Door& check_door(lua_State* l, int index);
    static Enemy& check_enemy(lua_State* l, int index);

    // Events.
    void on_started();
    void on_finished();
    void on_update();
    void on_draw(Surface& dst_surface);
    void on_pre_draw(Surface& dst_surface);
    void on_post_draw(Surface& dst_surface);
    void on_suspended(bool suspended);
    void on_input(InputEvent& event);
    void on_key_pressed(InputEvent& event);
    void on_key_released(InputEvent& event);
    void on_joypad_button_pressed(InputEvent& event);
    void on_joypad_button_released(InputEvent& event);
    void on_joypad_axis_moved(InputEvent& event);
    void on_joypad_hat_moved(InputEvent& event);
    void on_direction_pressed(InputEvent& event);
    void on_started(Destination* destination);
    void on_opening_transition_finished(Destination* destination);
    void on_camera_back();
    void on_obtaining_treasure(const Treasure& treasure);
    void on_obtained_treasure(const Treasure& treasure);
    void on_victory_finished();
    void on_activated();
    void on_activated_repeat();
    void on_inactivated();
    void on_left();
    void on_interaction();
    void on_interaction_finished();
    bool on_interaction_item(const std::string &item_name, int variant);
    void on_interaction_item_finished(const std::string &item_name, int variant);
    void on_npc_interaction(NPC& npc);  // TODO simplify
    void on_npc_interaction_finished(NPC& npc);
    bool on_npc_interaction_item(NPC& npc,
        const std::string &item_name, int variant);
    void on_npc_interaction_item_finished(NPC& npc,
        const std::string &item_name, int variant);
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
    void on_appear(Pickable& pickable);
    void on_movement_changed(Pickable& pickable);  // TODO remove
    void on_variant_changed(int variant);
    void on_amount_changed(int amount);
    void on_obtaining(const Treasure& treasure);
    void on_obtained(const Treasure& treasure);
    void on_use(InventoryItem& inventory_item);
    void on_ability_used(const std::string& ability_name);
    void on_appear();
    void on_enabled();
    void on_disabled();
    void on_restart();
    void on_pre_draw();
    void on_post_draw();
    void on_position_changed(const Rectangle& xy, Layer layer);
    void on_obstacle_reached();
    void on_movement_changed(Movement& movement);
    void on_movement_finished();
    void on_sprite_animation_finished(Sprite& sprite, const std::string& animation);
    void on_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame);
    void on_custom_attack_received(EnemyAttack attack, Sprite* sprite);
    void on_hurt(EnemyAttack attack, int life_lost);
    void on_dying();
    void on_dead();
    void on_immobilized();
    void on_message_received(Enemy& src_enemy, const std::string& message);

    /**
     * @brief Type of the functions that can be called by Lua.
     * */
    typedef int (FunctionExportedToLua) (lua_State* l);

    // Implementation of the API.
    // All functions named <type>_api_<name> can be called by Lua.
    static FunctionExportedToLua

      // Main API.
      main_api_load_file,
      main_api_do_file,
      main_api_reset,
      main_api_exit,
      main_api_start_screen,
      main_api_is_debug_enabled,
      main_api_get_distance,  // TODO remove
      main_api_get_angle,     // TODO remove

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
      video_api_get_mode,
      video_api_set_mode,
      video_api_switch_mode,
      video_api_is_mode_supported,
      video_api_get_modes,
      video_api_is_fullscreen,
      video_api_set_fullscreen,

      // Timer API.
      timer_api_start,
      timer_api_stop,
      timer_api_is_with_sound,
      timer_api_set_with_sound,

      // Language API.
      language_api_get_language,
      language_api_set_language,
      language_api_get_language_name,
      language_api_get_languages,
      language_api_get_default_language,
      language_api_get_string,
      language_api_get_dialog,

      // Equipment item API.
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
      text_surface_api_get_text_color,
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
      sprite_api_get_frame_delay,
      sprite_api_set_frame_delay,
      sprite_api_is_paused,
      sprite_api_set_paused,
      sprite_api_set_ignore_suspend,
      sprite_api_synchronize,

      // Movement API.
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

      // Game API.
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
      game_api_get_item,
      game_api_is_dungeon_finished,
      game_api_set_dungeon_finished,

      // Map API.
      map_api_get_game,
      map_api_start_dialog,
      map_api_set_dialog_variable,
      map_api_set_dialog_style,
      map_api_set_pause_enabled,  // TODO remove (do this in Lua only)
      map_api_get_light,
      map_api_set_light,
      map_api_move_camera,
      map_api_draw_sprite,
      map_api_get_tileset,
      map_api_set_tileset,
      map_api_get_hero,
      map_api_get_crystal_state,
      map_api_set_crystal_state,
      map_api_change_crystal_state,
      map_api_open_doors,
      map_api_close_doors,
      map_api_set_doors_open,
      map_api_create_pickable,  // TODO make creation functions uniform, all with a table as parameter
      map_api_create_destructible,
      map_api_create_block,
      map_api_create_bomb,
      map_api_create_explosion,
      map_api_create_fire,
      map_api_create_enemy,
      map_api_get_entity,
      map_api_has_entity,
      map_api_get_entities,
      map_api_get_entities_count,
      map_api_has_entities,
      map_api_set_entities_enabled,
      map_api_remove_entities,

      // Map entity API.
      entity_api_get_map,
      entity_api_get_name,
      entity_api_get_size,
      entity_api_get_origin,
      entity_api_get_position,
      entity_api_is_enabled,
      entity_api_set_enabled,
      entity_api_remove,
      entity_api_exists,
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
      hero_api_start_treasure,
      hero_api_start_victory,
      hero_api_start_boomerang,
      hero_api_start_bow,
      hero_api_start_hookshot,
      hero_api_start_running,
      hero_api_start_hurt,
      npc_api_start_movement,
      npc_api_stop_movement,
      npc_api_get_sprite,
      npc_api_set_position,
      chest_api_is_open,
      chest_api_set_open,
      block_api_reset,
      block_api_set_position,
      switch_api_is_activated,
      switch_api_set_activated,
      switch_api_set_locked,
      door_api_is_open,
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
      enemy_api_set_layer_independent_collisions,
      enemy_api_set_treasure,
      enemy_api_set_no_treasure,
      enemy_api_set_random_treasure,
      enemy_api_get_obstacle_behavior,
      enemy_api_set_obstacle_behavior,
      enemy_api_get_optimization_distance,
      enemy_api_set_optimization_distance,
      enemy_api_set_size,
      enemy_api_set_origin,
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

      // available to all userdata types
      userdata_meta_eq,
      userdata_meta_gc,
      userdata_meta_newindex_as_table,
      userdata_meta_index_as_table,

      // internal functions
      l_loader,
      l_get_map_entity_or_global,
      l_camera_do_callback,
      l_camera_restore;

    // Script data.
    lua_State* l;                   /**< The Lua state encapsulated. */
    MainLoop& main_loop;            /**< The Solarus main loop. */
    Game* current_game;             /**< The game currently running if any. */
    CustomScreen* current_screen;   /**< The current menu when no game is running. */

    std::map<Timer*, LuaTimerData>
        timers;                     /**< The timers currently running, with
                                     * their context and callback. */

    std::set<Drawable*>
      drawables;                 /**< All drawable objects created by
                                     * this script. */

    static std::map<InputEvent::KeyboardKey, std::string>
      input_key_names;              /**< Names of all existing keyboard keys in Lua. */
    static const char* enemy_attack_names[];
    static const char* enemy_hurt_style_names[];
    static const char* enemy_obstacle_behavior_names[];
    static const char* transition_style_names[];
};

#endif

