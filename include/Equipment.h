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
#ifndef SOLARUS_EQUIPMENT_H
#define SOLARUS_EQUIPMENT_H

#include "Common.h"

/**
 * @brief Represents the hero's equipment.
 *
 * This class makes the interface between the savegame and other classes.
 * You should call this class to get information about the current equipment
 * (sword, rupees, inventory…) and to modify it.
 */
class Equipment {

  private:

    Savegame *savegame;                        /**< the savegame encapsulated by this equipment object */
    Game *game;                                /**< the current game (may be NULL when the savegame is loaded outside a game) */

    // magic bar decrease handling
    uint32_t magic_decrease_delay;             /**< when the magic bar decreases with time,
						* delay between two decreases of 1 magic point */
    uint32_t next_magic_decrease_date;         /**< date of the next decrease of 1 magic point */

  public:

    // creation and destruction
    Equipment(Savegame *savegame);
    ~Equipment(void);
    void set_game(Game *game);

    void update(void);

    // money
    int get_max_money(void);
    void set_max_money(int max_money);

    int get_money(void);
    void set_money(int money);
    void add_money(int money_to_add);
    void remove_money(int money_to_remove);

    // life
    int get_max_life(void);
    void set_max_life(int max_life);

    int get_life(void);
    void set_life(int life);
    void add_life(int life_to_add);
    void remove_life(int life_to_remove);
    void restore_all_life(void);

    // magic
    int get_max_magic(void);
    void set_max_magic(int max_magic);

    int get_magic(void);
    void set_magic(int magic);
    void add_magic(int magic_to_add);
    void remove_magic(int magic_to_remove);
    void restore_all_magic(void);
    bool needs_magic(void);

    bool is_magic_decreasing(void);
    void start_removing_magic(uint32_t delay);
    void stop_removing_magic(void);

    // equipment items saved
    ItemProperties * get_item_properties(const std::string &item_name);
    bool has_item(const std::string &item_name);
    int get_item_variant(const std::string &item_name);
    void give_item(const std::string &item_name);
    void set_item_variant(const std::string &item_name, int variant);
    void remove_item(const std::string &item_name);

    int get_item_amount(const std::string &item_name);
    void set_item_amount(const std::string &item_name, int amount);
    void add_item_amount(const std::string &item_name, int amount_to_add);
    void remove_item_amount(const std::string &item_name, int amount_to_remove);

    int get_item_maximum(const std::string &item_name);
    bool has_item_maximum(const std::string &item_name);
    void set_item_maximum(const std::string &item_name, int maximum);

    // item assignments
    const std::string & get_item_assigned(int slot);
    void set_item_assigned(int slot, const std::string &item_name);
    int get_item_slot(const std::string &item_name);

    // small keys
    bool are_small_keys_enabled(void);
    int get_small_keys_variable(void);

    bool has_small_key(void);
    int get_small_keys(void);
    void add_small_key(void);
    void remove_small_key(void);

    // abilities
    bool has_ability(const std::string &ability_name);
    bool has_ability(const std::string &ability_name, int level);
    int get_ability(const std::string &ability_name);
    void set_ability(const std::string &ability_name, int level);
    bool use_ability(const std::string &ability_name);

    // dungeons
    bool is_dungeon_finished(int dungeon);
    bool is_dungeon_finished(void);
    void set_dungeon_finished(void);
};

#endif

