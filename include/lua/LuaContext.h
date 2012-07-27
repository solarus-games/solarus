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
#include "lua/Script.h"  // TODO remove

/**
 * @brief This class represents the only Lua context that runs
 * dynamic scripts at runtime.
 *
 * Such scripts includes map scripts, enemy scripts, menu scripts, etc.
 * Data files are not managed by this class even if they are written in Lua.
 *
 * TODO: enemy scripts and item scripts currently
 * have still their own Lua world (they inherit Script).
 * Once they are all converted to this shared Lua context,
 * the Script class will be removed.
 */
class LuaContext: public Script {

  public:

    static const std::string video_module_name;     /**< sol.video */
    static const std::string menu_module_name;      /**< sol.menu */
    static const std::string language_module_name;  /**< sol.language */

    LuaContext(MainLoop& main_loop);
    ~LuaContext();

    // Main loop from C++.
    void initialize();
    void exit();
    void update();
    void notify_input(InputEvent& event);

    // Main Lua script (sol.main).
    void main_on_update();
    void main_on_input(InputEvent& event);
    void main_on_started();
    void main_on_finished();

    // Menu events.
    void menu_on_update(int menu_ref);
    void menu_on_display(int menu_ref, Surface& dst_surface);
    void menu_on_input(int menu_ref, InputEvent& event);
    void menu_on_started(int menu_ref);
    void menu_on_finished(int menu_ref);

    // Game events.
    void game_on_update(Game& game);
    void game_on_pre_display(Game& game, Surface& dst_surface);
    void game_on_post_display(Game& game, Surface& dst_surface);
    void game_on_started(Game& game);
    void game_on_finished(Game& game);

    // Map events.
    void map_on_update(Map& map);
    void map_on_suspended(Map& map, bool suspended);
    void map_on_started(Map& map, DestinationPoint& destination_point);
    void map_on_treasure_obtaining(Map& map, const Treasure& treasure);
    void map_on_treasure_obtained(Map& map, const Treasure& treasure);
    void map_on_opening_transition_finished(Map& map,
        DestinationPoint& destination_point);
    void map_on_switch_activated(Map& map, Switch& sw);
    void map_on_switch_inactivated(Map& map, Switch& sw);
    void map_on_switch_left(Map& map, Switch& sw);
    void map_on_hero_victory_sequence_finished(Map& map);
    void map_on_hero_on_sensor(Map& map, Sensor& sensor);
    void map_on_hero_still_on_sensor(Map& map, Sensor& sensor);
    void map_on_npc_movement_finished(Map& map, NPC& npc);
    void map_on_npc_interaction_finished(Map& map, NPC& npc);
    void map_on_npc_interaction_item_finished(Map& map, NPC& npc,
        const std::string& item_name, int variant);
    void map_on_sensor_collision_explosion(Map& map, Sensor& sensor);
    bool map_on_chest_empty(Map& map, Chest& chest);
    bool map_on_shop_item_buying(Map& map, ShopItem& shop_item);
    void map_on_shop_item_bought(Map& map, ShopItem& shop_item);
    void map_on_door_open(Map& map, Door& door);
    void map_on_door_closed(Map& map, Door& door);
    void map_on_block_moved(Map& map, Block& block);
    void map_on_enemy_dying(Map& map, Enemy& enemy);
    void map_on_enemy_dead(Map& map, Enemy& enemy);

  private:

    bool find_local_function(int index, const std::string& function_name);
    bool find_local_function(const std::string& function_name);
    bool find_method(int index, const std::string& function_name);
    bool find_method(const std::string& function_name);

    static int l_loader(lua_State* l);

    void register_video_module();
    void register_menu_module();
    void register_language_module();

  public:  // TODO make private once Script is removed

    void on_started();
    void on_finished();
    void on_update();
    void on_display(Surface& dst_surface);
    void on_pre_display(Surface& dst_surface);
    void on_post_display(Surface& dst_surface);
    void on_input(InputEvent& event);
    void on_key_pressed(InputEvent& event);
    void on_key_released(InputEvent& event);
    void on_joypad_button_pressed(InputEvent& event);
    void on_joypad_button_released(InputEvent& event);
    void on_joypad_axis_moved(InputEvent& event);
    void on_joypad_hat_moved(InputEvent& event);
    void on_direction_pressed(InputEvent& event);
    void on_suspended(bool suspended);
    void on_started(DestinationPoint& destination_point);
    void on_treasure_obtaining(const Treasure& treasure);
    void on_treasure_obtained(const Treasure& treasure);
    void on_opening_transition_finished(const std::string& destination_point_name);
    void on_switch_activated(Switch& sw);
    void on_switch_inactivated(Switch& sw);
    void on_switch_left(Switch& sw);
    void on_hero_victory_sequence_finished();
    void on_hero_on_sensor(Sensor& sensor);
    void on_hero_still_on_sensor(Sensor& sensor);
    void on_npc_movement_finished(NPC& npc);
    void on_npc_interaction_finished(NPC& npc);
    void on_npc_interaction_item_finished(NPC& npc,
        const std::string &item_name, int variant);
    void on_sensor_collision_explosion(Sensor& sensor);
    bool on_chest_empty(Chest& chest);
    bool on_shop_item_buying(ShopItem& shop_item);
    void on_shop_item_bought(ShopItem& shop_item);
    void on_door_open(Door& door);
    void on_door_closed(Door& door);
    void on_block_moved(Block& block);
    void on_enemy_dying(Enemy& enemy);
    void on_enemy_dead(Enemy& enemy);
};

#endif

