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
#ifndef SOLARUS_SCRIPTS_H
#define SOLARUS_SCRIPTS_H

#include "Common.h"
#include <list>

/**
 * @brief Represents a list of scripts currently running.
 *
 * When an event is triggered (i.e. calling Lua from C++),
 * you can use this class to notify all current scripts.
 */
class Scripts {

  private:

    std::list<Script*> scripts;			/**< the scripts currently running */

  public:

    Scripts();
    ~Scripts();

    void add_script(Script *script);
    void remove_script(Script *script);

    // calling Lua from C++
    void event_dialog_started(const MessageId &message_id);
    void event_dialog_finished(const MessageId &first_message_id, int answer);
    void event_camera_reached_target();
    void event_camera_back();
    void event_treasure_obtaining(const std::string &item_name, int variant, int savegame_variable);
    void event_treasure_obtained(const std::string &item_name, int variant, int savegame_variable);
    void event_map_started(const std::string &destination_point_name);
    void event_map_opening_transition_finished(const std::string &destination_point_name);
    void event_switch_enabled(const std::string &switch_name);
    void event_switch_disabled(const std::string &switch_name);
    void event_switch_left(const std::string &switch_name);
    void event_hero_victory_sequence_finished();
    void event_hero_on_sensor(const std::string &sensor_name);
    void event_hero_interaction(const std::string &entity_name);
    bool event_hero_interaction_item(const std::string &entity_name, const std::string &item_name, int variant);
    void event_npc_dialog(const std::string &npc_name);
    void event_npc_movement_finished(const std::string &npc_name);
    bool event_chest_empty(const std::string &chest_name);
    void event_shop_item_bought(const std::string &shop_item_name);
    void event_enemy_dead(const std::string &enemy_name);
};

#endif

