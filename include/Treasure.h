/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_TREASURE_H
#define ZSDX_TREASURE_H

#include "Common.h"

/**
 * A treasure is any item that the hero brandishes when he receives it.
 * It includes all items he can get in chest, buy a shop or get from the script
 * (e.g. when talking to an NPC).
 * It also includes items picked on the ground if they are saved
 * (heart pieces, keys, etc.).
 * Picked items that are not saved are not treasures since they are not brandished.
 *
 * A treasure is represented as the following three values:
 * - the treasure content: indicates the nature of the treasure: 20 rupees, a heart container,
 *   the map, a red potion, the bow, some bombs, etc;
 * - the amount: an optional integer indicating the number of items of this treasure
 *   (for bombs, rupees, croissants, etc.);
 * - a savegame variable: index of the boolean variable saved indicating whether
 *   the player has found this treasure (-1 if the treasure is not saved).
 */
class Treasure {

  public:

    /**
     * Identifies each possible treasure.
     * The values correspond to the icons in hud/message_and_treasure_icons.png
     * and the maps refer to these numbers for each chest or shop item.
     */
    enum Content {
      NONE              = -1,

      FEATHER           = 0,
      // 1: no bombs here because the first time we give the small bomb bag,
      // and other times we give pickable bombs (below)
      BOW               = 2,
      BOW_AND_ARROWS    = 3, // note: for arrows only, see the pickable arrows below
      BOOMERANG         = 4,
      LAMP              = 5,
      HOOK_SHOT         = 6,
      BOTTLE            = 7,
      WATER             = 8,
      RED_POTION        = 9,

      GREEN_POTION      = 10,
      BLUE_POTION       = 11,
      FAIRY_IN_BOTTLE   = 12,
      PEGASUS_SHOES     = 13,
      MYSTIC_MIRROR     = 14,
      CANE_OF_SOMARIA   = 15,
      APPLES            = 16,
      PAINS_AU_CHOCOLAT = 17,
      CROISSANTS        = 18,
      ROCK_KEY          = 19,

      RED_KEY           = 20,
      CLAY_KEY          = 21,
      APPLE_PIE         = 22,
      GOLDEN_BARS       = 23,
      EDELWEISS         = 24,
      BONE_KEY          = 25,
      FLIPPERS          = 26,
      MAGIC_CAPE        = 27,
      IRON_KEY          = 28,
      STONE_KEY         = 29,

      WOODEN_KEY        = 30,
      ICE_KEY           = 31,
      IRON_GLOVE        = 32,
      GOLDEN_GLOVE      = 33,
      FIRE_STONE        = 34,

      // 35 to 39: unused

      // 40 to 49: unused, reserved for the icons of the dungeons finished in quest status

      WORLD_MAP         = 50,
      // 51: the small rupee bag is not a treasure since the player has it from the beginning
      LARGE_RUPEE_BAG   = 52,
      HUGE_RUPEE_BAG    = 53,
      SMALL_BOMB_BAG    = 54,
      LARGE_BOMB_BAG    = 55,
      HUGE_BOMB_BAG     = 56,
      // 57: the small quiver is not a treasure since the player has it automatically when he receives the bow
      LARGE_QUIVER      = 58,
      HUGE_QUIVER       = 59,

      BLUE_TUNIC        = 61,
      RED_TUNIC         = 62,
      SHIELD_1          = 63,
      SHIELD_2          = 64,
      SHIELD_3          = 65,
      SWORD_1           = 66,
      SWORD_2           = 67,
      SWORD_3           = 68,
      SWORD_4           = 69,

      MAP               = 70,
      COMPASS           = 71,
      SMALL_KEY         = 72,
      BIG_KEY           = 73,
      BOSS_KEY          = 74,
      // 75 to 78: icons for pieces of hearts (message icons only, no treasure)
      PIECE_OF_HEART    = 79,

      HEART_CONTAINER   = 80,
      // 81: a fairy is not a treasure
      BOMBS             = 82,
      ARROWS            = 83,
      HEARTS            = 84,
      SMALL_MAGIC       = 85,
      BIG_MAGIC         = 86,
      GREEN_RUPEES      = 87,
      BLUE_RUPEES       = 88,
      RED_RUPEES        = 89,
    };

  private:

    Surface *treasures_img;

    Content content;         /**< content of the treasure */
    int amount;              /**< amount of item (for some kinds of treasures only) */
    int savegame_variable;   /**< index of the savegame boolean variable corresponding to this treasure,
			      * or -1 if the treasure state is not saved */

    Counter *counter;

    void play_treasure_sound(void);
    void show_message(void);
    void add_item_to_equipment(void);

  public:

    Treasure(Content content, int savegame_variable);
    Treasure(Content content, int amount, int savegame_variable);
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

