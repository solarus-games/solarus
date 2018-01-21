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
#ifndef SOLARUS_TREASURE_H
#define SOLARUS_TREASURE_H

#include "solarus/core/Common.h"
#include "solarus/graphics/SurfacePtr.h"
#include "solarus/graphics/SpritePtr.h"
#include <string>

namespace Solarus {

class EquipmentItem;
class Game;
class Sprite;

/**
 * \brief Represents an item that the hero brandishes when he receives it.
 *
 * Treasures include all items the hero can get in chest, buy in a shop,
 * pick on the ground or get from scripts (e.g. when talking to an NPC).
 *
 * A treasure is represented as the following values:
 * - the item name: a string identifying the nature of the treasure,
 * - the variant: an integer indicating the variant of this item (some items
 * may have several variants),
 * - a savegame variable: name of the saved boolean variable that indicates
 * whether the player has found this treasure (or an empty string if the treasure
 * is not saved).
 */
class Treasure {

  public:

    Treasure(Game& game, const std::string& item_name, int variant,
        const std::string& savegame_variable);

    Game& get_game() const;
    EquipmentItem& get_item() const;
    const std::string& get_item_name() const;
    int get_variant() const;
    const std::string& get_savegame_variable() const;
    bool is_saved() const;
    bool is_empty() const;

    bool is_obtainable() const;
    void ensure_obtainable();
    void check_obtainable() const;
    void give_to_player() const;
    bool is_found() const;

    SpritePtr create_sprite() const;

  private:

    Game* game;                        /**< the current game */
    std::string item_name;             /**< content of the treasure (or an empty string) */
    int variant;                       /**< variant of this content */
    std::string savegame_variable;     /**< name of the savegame boolean variable corresponding to this treasure,
                                        * or an empty string if the treasure state is not saved */

};

}

#endif

