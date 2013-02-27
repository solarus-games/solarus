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
#ifndef SOLARUS_EQUIPMENT_H
#define SOLARUS_EQUIPMENT_H

#include "Common.h"
#include <string>
#include <map>

struct lua_State;

/**
 * @brief Represents the hero's equipment.
 *
 * This class gives access to the equipment data saved and the properties of items
 * as defined in items.dat.
 * You should call this class to get information about the current equipment
 * (sword, money, inventory...) and to modify it.
 */
class Equipment {

  private:

    Savegame& savegame;                          /**< the savegame encapsulated by this equipment object */
    bool suspended;                              /**< indicates that the game is suspended */

    // items
    std::map<std::string, EquipmentItem*> items;  /**< each item (properties loaded from item scripts) */

    const std::string get_ability_savegame_variable(const std::string& ability_name);

  public:

    // creation and destruction
    Equipment(Savegame& savegame);
    ~Equipment();

    Savegame& get_savegame();
    Game* get_game();
    void notify_game_started();
    void notify_game_finished();
    void notify_map_changed(Map& map);

    void update();
    void set_suspended(bool suspended);

    // money
    int get_max_money();
    void set_max_money(int max_money);

    int get_money();
    void set_money(int money);
    void add_money(int money_to_add);
    void remove_money(int money_to_remove);

    // life
    int get_max_life();
    void set_max_life(int max_life);

    int get_life();
    void set_life(int life);
    void add_life(int life_to_add);
    void remove_life(int life_to_remove);
    void restore_all_life();

    // magic
    int get_max_magic();
    void set_max_magic(int max_magic);

    int get_magic();
    void set_magic(int magic);
    void add_magic(int magic_to_add);
    void remove_magic(int magic_to_remove);
    void restore_all_magic();

    // equipment items
    void load_items();
    bool item_exists(const std::string& item_name);
    EquipmentItem& get_item(const std::string& item_name);
    EquipmentItem* get_item_assigned(int slot);
    void set_item_assigned(int slot, EquipmentItem* item);
    int get_item_slot(EquipmentItem& item);

    // built-in abilities
    // TODO rename abilities: sword -> attack, tunic -> defense, shield -> protection.
    // TODO make notify_ability_changed
    bool has_ability(const std::string& ability_name, int level = 1);
    int get_ability(const std::string& ability_name);
    void set_ability(const std::string& ability_name, int level);
    void notify_ability_used(const std::string& ability_name);
};

#endif

