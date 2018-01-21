/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/core/Ability.h"
#include <map>
#include <memory>
#include <string>

struct lua_State;

namespace Solarus {

class EquipmentItem;
class Game;
class Map;
class Savegame;

/**
 * \brief Represents the hero's equipment.
 *
 * This class gives access to the equipment data saved and the properties of
 * items.
 * You should call this class to get information about the current equipment
 * (sword, money, items...) and to modify it.
 */
class SOLARUS_API Equipment {

  public:

    // creation and destruction
    explicit Equipment(Savegame& savegame);

    Savegame& get_savegame();
    Game* get_game();
    void notify_game_started();
    void notify_game_finished();
    void notify_map_changed(Map& map);

    void update();
    void set_suspended(bool suspended);

    // money
    int get_max_money() const;
    void set_max_money(int max_money);

    int get_money() const;
    void set_money(int money);
    void add_money(int money_to_add);
    void remove_money(int money_to_remove);

    // life
    int get_max_life() const;
    void set_max_life(int max_life);

    int get_life() const;
    void set_life(int life);
    void add_life(int life_to_add);
    void remove_life(int life_to_remove);
    void restore_all_life();

    // magic
    int get_max_magic() const;
    void set_max_magic(int max_magic);

    int get_magic() const;
    void set_magic(int magic);
    void add_magic(int magic_to_add);
    void remove_magic(int magic_to_remove);
    void restore_all_magic();

    // equipment items
    void load_items();
    bool item_exists(const std::string& item_name) const;
    EquipmentItem& get_item(const std::string& item_name);
    const EquipmentItem& get_item(const std::string& item_name) const;
    EquipmentItem* get_item_assigned(int slot);
    const EquipmentItem* get_item_assigned(int slot) const;
    void set_item_assigned(int slot, EquipmentItem* item);
    int get_item_slot(const EquipmentItem& item) const;

    // built-in abilities
    // TODO make notify_ability_changed
    bool has_ability(Ability ability, int level = 1) const;
    int get_ability(Ability ability) const;
    void set_ability(Ability ability, int level);
    void notify_ability_used(Ability ability);

  private:

    Savegame& savegame;                          /**< The savegame encapsulated by this equipment object. */
    bool suspended;                              /**< Indicates that the game is suspended. */

    // items
    std::map<std::string, std::shared_ptr<EquipmentItem>>
        items;                                   /**< Each item (properties loaded from item scripts). */

    std::string get_ability_savegame_variable(Ability ability) const;

};

}

#endif

