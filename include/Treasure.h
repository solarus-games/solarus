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
#ifndef SOLARUS_TREASURE_H
#define SOLARUS_TREASURE_H

#include "Common.h"

/**
 * @brief Represents an item that the hero brandishes when he receives it.
 *
 * Treasures include all items the hero can get in chest, buy in a shop,
 * pick on the ground or get from scripts (e.g. when talking to an NPC).
 *
 * A treasure is represented as the following values:
 * - the item name: a string identitying the nature of the treasure, according to the file items.dat
 * - the variant: indicates the variant of this item
 * - a savegame variable: index of the boolean variable that indicates whether
 *   the player has found this treasure (-1 if the treasure is not saved).
 */
class Treasure {

  private:

    Game &game;				/**< the current game */
    std::string item_name;		/**< content of the treasure (can be "_none") */
    int variant;			/**< variant of this content */
    int savegame_variable;		/**< index of the savegame boolean variable corresponding to this treasure,
					 * or -1 if the treasure state is not saved */
    Sprite *sprite;			/**< the sprite of the treasure */

  public:

    Treasure(Game &game, const std::string &item_name, int variant, int savegame_variable);
    Treasure(const Treasure &other);
    ~Treasure();

    ItemProperties& get_item_properties();
    const std::string & get_item_name() const;
    int get_variant() const;
    int get_savegame_variable() const;
    bool is_saved() const;
    bool is_empty() const;

    void give_to_player() const;
    bool is_found() const;

    void display(Surface *destination, int x, int y);
};

#endif

