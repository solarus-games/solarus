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
 * Treasures include all items the hero can get in chest, buy in a shop
 * or get from the script (e.g. when talking to an NPC).
 * It also includes items picked on the ground if they are saved
 * (heart pieces, keys, etc.).
 * Picked items that are not saved (rupees, bombs, etc.) are not treasures since they are not brandished
 * when they are picked on the ground. When they come from a chest, they are
 * treasures.
 *
 * A treasure is represented as the following three values:
 * - the treasure content: indicates the nature of the treasure: a red rupee, a heart container,
 *   the map, a red potion, the bow, some bombs, etc;
 * - the amount: an optional integer indicating the number of items of this treasure
 *   (for bombs, rupees, croissants, etc.);
 * - a savegame variable: index of the boolean variable that indicates whether
 *   the player has found this treasure (-1 if the treasure is not saved).
 */
class Treasure {

  public:

    /**
     * @brief Identification of each possible treasure.
     *
     * The values correspond to the icons in hud/message_and_treasure_icons.png
     * and the maps refer to these numbers for each chest or shop item.
     */
    enum Content {

      // TODO separate message icons from treasure contents
      // TODO provide constants to the scripts or change the Lua API to specify treasures with strings
      NONE,
      INVENTORY_ITEM,
      WORLD_MAP,
      LARGE_RUPEE_BAG,
      HUGE_RUPEE_BAG,
      SMALL_BOMB_BAG,
      LARGE_BOMB_BAG,
      HUGE_BOMB_BAG,
      LARGE_QUIVER,
      HUGE_QUIVER,
      BLUE_TUNIC,
      RED_TUNIC,
      SHIELD_1,
      SHIELD_2,
      SHIELD_3,
      SWORD_1,
      SWORD_2,
      SWORD_3,
      SWORD_4,
      MAP,
      COMPASS,
      SMALL_KEY,
      BIG_KEY,
      BOSS_KEY,
      PIECE_OF_HEART,
      HEART_CONTAINER,
      BOMBS,
      ARROWS,
      BOW_AND_ARROWS,
      HEARTS,
      SMALL_MAGIC,
      BIG_MAGIC,
      GREEN_RUPEES,
      BLUE_RUPEES,
      RED_RUPEES
    };

  private:

    Surface *treasures_img;  /**< image of all treasures */

    Game *game;              /**< the current game */
    Content content;         /**< content of the treasure */
    int amount;              /**< amount of item (for some kinds of treasures only) */
    int savegame_variable;   /**< index of the savegame boolean variable corresponding to this treasure,
			      * or -1 if the treasure state is not saved */

    Counter *counter;

    void play_treasure_sound(void);
    void show_message(void);
    void add_item_to_equipment(void);

  public:

    Treasure(Game *game, Content content, int savegame_variable);
    Treasure(Game *game, Content content, int amount, int savegame_variable);
    ~Treasure(void);

    Content get_content(void);
    bool has_amount(void);
    int get_amount(void);
    int get_savegame_variable(void);

    bool is_amount_full(void);
    void give_to_player(void);
    bool is_found(void);

    void display(Surface *destination, int x, int y);
};

#endif

