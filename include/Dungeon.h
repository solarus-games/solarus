#ifndef ZSDX_DUNGEON_H
#define ZSDX_DUNGEON_H

#include "Common.h"
#include <vector>

/**
 * Represents the properties of a dungeon, like the floors
 * and the chests it contains.
 * This class is used by the map submenu when the player is in a dungeon.
 * These properties are loaded from file dungeons.zsd.
 */
class Dungeon {

 private:

  int dungeon_number;             /**< dungeon number, between 1 and 20 */

  // floors
  int lowest_floor;               /**< lowest floor number, between -16 and 15 */
  vector<SDL_Rect> floor_sizes;   /**< size of each floor */

  // elements of each floor: chests, minibosses, boss
  struct DungeonElement {
    int savegame_variable;
    int floor;
    int x;
    int y;
  };

  vector<DungeonElement> *chests;      /**< properties of each chest for each floor */

  vector<DungeonElement> *minibosses;  /**< properties of each miniboss for each floor */
  DungeonElement boss;                 /**< properties of the boss */

  void load(void);

 public:

  Dungeon(int dungeon_number);
  ~Dungeon(void);

  int get_nb_floors(void);
  int get_lowest_floor(void);
  int get_highest_floor(void);
};

#endif
