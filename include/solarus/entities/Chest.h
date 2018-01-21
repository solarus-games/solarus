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
#ifndef SOLARUS_CHEST_H
#define SOLARUS_CHEST_H

#include "solarus/core/Common.h"
#include "solarus/core/Treasure.h"
#include "solarus/entities/Entity.h"
#include <map>
#include <string>

namespace Solarus {

/**
 * \brief Represents a chest on a map.
 *
 * It can be a normal chest or a big chest.
 */
class Chest: public Entity {

  public:

    static constexpr EntityType ThisType = EntityType::CHEST;

    /**
     * \brief The different possible ways of opening a chest.
     */
    enum class OpeningMethod {
      BY_INTERACTION,                       /**< Can be opened by pressing the action command. */
      BY_INTERACTION_IF_SAVEGAME_VARIABLE,  /**< Can be opened by pressing the action command, provided that a savegame variable has the correct value. */
      BY_INTERACTION_IF_ITEM,               /**< Can be opened by pressing the action command, provided that the player has a specific equipment item. */
    };

    Chest(
        const std::string& name,
        int layer,
        const Point& xy,
        const std::string& sprite_name,
        const Treasure& treasure);

    virtual EntityType get_type() const override;

    const Treasure& get_treasure() const;
    void set_treasure(const Treasure& treasure);

    bool is_open() const;
    void set_open(bool open);
    bool can_open();

    OpeningMethod get_opening_method() const;
    void set_opening_method(OpeningMethod opening_method);
    const std::string& get_opening_condition() const;
    void set_opening_condition(const std::string& opening_condition);
    bool is_opening_condition_consumed() const;
    void set_opening_condition_consumed(bool opening_condition_consumed);
    const std::string& get_cannot_open_dialog_id() const;
    void set_cannot_open_dialog_id(const std::string& cannot_open_dialog_id);

    virtual bool is_obstacle_for(Entity& other) override;
    virtual void notify_collision(Entity& entity_overlapping, CollisionMode collision_mode) override;
    virtual void notify_enabled(bool enabled) override;
    virtual void update() override;
    virtual bool notify_action_command_pressed() override;
    virtual void set_suspended(bool suspended) override;

    static const std::map<OpeningMethod, std::string> opening_method_names;

  private:

    Treasure treasure;                 /**< The treasure placed in this chest (possibly empty, possibly non-obtainable).
                                        * Remains unchanged when the chest is open.  */
    bool open;                         /**< whether the chest is open (but the treasure may not have
                                        * been given yet because there is a delay of 500 ms). */
    bool treasure_given;               /**< whether the chest is open and the treasure has been given to the player. */
    uint32_t treasure_date;            /**< Date when the treasure will be given to the player. */

    OpeningMethod opening_method;      /**< How this chest can be opened. */
    std::string opening_condition;     /**< Condition required to open the chest: a savegame variable if
                                        * the opening mode is \c OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE,
                                        * or an equipment item name if the opening mode is
                                        * \c OPENING_BY_INTERACTION_IF_ITEM. */
    bool opening_condition_consumed;   /**< Indicates that the required savegame variable
                                        * (in the case of \c OPENING_BY_INTERACTION_IF_SAVEGAME_VARIABLE)
                                        * or the required item
                                        * (in the case of \c OPENING_BY_INTERACTION_IF_ITEM)
                                        * should be consumed when opening the chesty. */
    std::string cannot_open_dialog_id; /**< Dialog to show if the chesty cannot be opened,
                                        * or an empty string. */

};

}

#endif

