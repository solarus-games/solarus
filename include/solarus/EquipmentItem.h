/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_EQUIPMENT_ITEM_H
#define SOLARUS_EQUIPMENT_ITEM_H

#include "solarus/Common.h"
#include "solarus/lua/ExportableToLua.h"
#include "solarus/Ability.h"
#include <string>

namespace Solarus {

class Equipment;
class Game;
class LuaContext;
class Map;
class Pickable;
class Savegame;

/**
 * \brief An item possibly possessed by the player.
 *
 * Its properties are controlled by the script of the item.
 */
class EquipmentItem: public ExportableToLua {

  public:

    EquipmentItem(Equipment& equipment);

    Equipment& get_equipment();
    Game* get_game();
    Savegame& get_savegame();
    const Savegame& get_savegame() const;
    LuaContext& get_lua_context();

    // Properties.
    const std::string& get_name() const;
    void set_name(const std::string& name);
    bool is_saved() const;
    const std::string& get_savegame_variable() const;
    void set_savegame_variable(const std::string& savegame_variable);
    bool has_amount() const;
    const std::string& get_amount_savegame_variable() const;
    void set_amount_savegame_variable(const std::string& amount_savegame_variable);
    bool is_obtainable() const;
    void set_obtainable(bool obtainable);
    bool is_assignable() const;
    void set_assignable(bool assignable);
    bool get_can_disappear() const;
    void set_can_disappear(bool can_disappear);
    bool get_brandish_when_picked() const;
    void set_brandish_when_picked(bool brandish_when_picked);
    const std::string& get_sound_when_picked() const;
    void set_sound_when_picked(const std::string& sound_when_picked);
    const std::string& get_sound_when_brandished() const;
    void set_sound_when_brandished(const std::string& sound_when_brandished);
    const std::string& get_shadow() const;
    void set_shadow(const std::string& shadow);

    // Current state.
    void initialize();
    void exit();
    void update();
    void set_suspended(bool suspended);
    void start();
    void notify_map_changed(Map& map);
    void notify_amount_changed(int amount);
    void notify_variant_changed(int variant);
    void notify_using();
    void notify_ability_used(Ability ability);
    void notify_pickable_appeared(Pickable& pickable);

    int get_variant() const;
    void set_variant(int variant);

    int get_amount() const;
    void set_amount(int amount);

    int get_max_amount() const;
    void set_max_amount(int max_amount);

    virtual const std::string& get_lua_type_name() const override;

  private:

    Equipment& equipment;                /**< the equipment object that manages all items */
    std::string name;                    /**< name that identifies this item */
    std::string savegame_variable;       /**< savegame variable that stores the possession state */
    std::string amount_savegame_variable; /**< savegame variable that stores the amount associated to this item
                                          * or an empty string if there is no amount */
    int max_amount;                      /**< limit of the amount associated to this item, or 0 */
    bool obtainable;                     /**< whether the player can receive this item */
    bool assignable;                     /**< indicates that this item can be assigned to an item key an then
                                          * be used explicitly */
    bool can_disappear;                  /**< when the item is dropped on the ground, indicates that it will disappear
                                          * after some time */
    bool brandish_when_picked;           /**< when the item is picked on the ground, indicates that the hero
                                          * brandishes it */
    std::string sound_when_picked;       /**< sound to play when the item is picked on the ground (or an empty string) */
    std::string sound_when_brandished;   /**< sound to play when the item is brandished (or an empty string) */
    std::string shadow;                  /**< animation of the shadow sprite when the item is on the ground (or an empty string) */

};

}

#endif

