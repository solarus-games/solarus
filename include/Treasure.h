#ifndef ZSDX_TREASURE_H
#define ZSDX_TREASURE_H

#include "Common.h"

/**
 * A treasure is any item that Link can receive.
 * It can be for example in a chest (class Chest) or in a shop (class ShopItem).
 * A treasure is represented as the following three values:
 * - the treasure content: indicates the nature of the treasure: 20 rupees, a heart container,
 *   the map, a red potion, the bow, some bombs, etc;
 * - the amount: an optional integer indicating the number of items of this treasure
 *   (for bombs, rupees, croissants, etc.);
 * - a savegame index: index of the boolean value indicating whether Link has found this item.
 */
class Treasure {

 public:

  /**
   * Identifies each possible treasure.
   * The values correspond to the icons in hud/message_and_treasure_icons.png
   */
  enum TreasureContent {
    FEATHER = 0,
    BOW = 2,
    BOW_AND_ARROWS,
    BOOMERANG,
    LAMP,
    HOOK_SHOT,
    BOTTLE,
    WATER,
    RED_POTION,

    GREEN_POTION = 10,
    BLUE_POTION,
    FAIRY_IN_BOTTLE,
    PEGASUS_SHOES,
    MYSTIC_MIRROR,
    CANE_OF_SOMARIA,
    MAGIC_CAPE,
    IRON_GLOVE,
    GOLDEN_GLOVE,
    FIRE_STONE,

    APPLES = 20,
    PAINS_AU_CHOCOLAT,
    CROISSANTS,
    APPLE_PIE,
    GOLDEN_BARS,
    EDELWEISS,
    BONE_KEY,
    FLIPPERS,
    RED_KEY,

    CLAY_KEY = 30,
    ROCK_KEY,
    IRON_KEY,
    STONE_KEY,
    WOODEN_KEY,
    ICE_KEY,

    WORLD_MAP = 50,
    LARGE_RUPEE_BAG = 52,
    HUGE_RUPEE_BAG,
    SMALL_BOMB_BAG,
    LARGE_BOMB_BAG,
    HUGE_BOMB_BAG,
    LARGE_QUIVER = 58,
    HUGE_QUIVER,

    BLUE_TUNIC = 61,
    RED_TUNIC,
    SHIELD_1,
    SHIELD_2,
    SHIELD_3,
    SWORD_1,
    SWORD_2,
    SWORD_3,
    SWORD_4,

    MAP = 70,
    COMPASS,
    SMALL_KEY,
    BIG_KEY,
    BOSS_KEY,
    PIECE_OF_HEART = 79,

    HEART_CONTAINER = 80,
    BOMBS = 82,
    ARROWS,
    HEARTS,
    SMALL_MAGIC,
    BIG_MAGIC,
    GREEN_RUPEES,
    BLUE_RUPEES,
    RED_RUPEES,
  };

 private:

  TreasureContent content; /**< content of the treasure */
  int amount;              /**< amount of item (for some kinds of treasures only) */
  int savegame_index;      /**< index of the savegame variable corresponding to this treasure,
			    * or -1 if the treasure state is not saved */

  void play_treasure_sound(void);
  void start_brandish_item(void);
  void stop_brandish_item(void);
  void show_message(void);
  void add_item_to_equipment(void);

  bool has_amount(void);

 public:

  Treasure(TreasureContent content, int amount, int savegame_index);
  ~Treasure(void);

  void give_to_player(void);
};

#endif
