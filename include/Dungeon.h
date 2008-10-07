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

 public:
  
  // elements of each floor: chests, minibosses, boss
  struct DungeonElement {
    int savegame_variable;
    int floor;
    int x;
    int y;
  };


 private:

  int dungeon_number;             /**< dungeon number, between 1 and 20 */

  // floors
  int lowest_floor;               /**< lowest floor number, between -16 and 15 */
  vector<SDL_Rect> floor_sizes;   /**< size of each floor */

  vector<DungeonElement> *chests;      /**< properties of each chest for each floor */
  vector<DungeonElement> *minibosses;  /**< properties of each miniboss for each floor */
  DungeonElement boss;                 /**< properties of the boss */

  void load(void);

 public:

  // creation and destruction
  Dungeon(int dungeon_number);
  ~Dungeon(void);

  string get_name(void);

  // floors
  int get_nb_floors(void);
  int get_lowest_floor(void);
  int get_highest_floor(void);

  // displaying floors (for the hud and the dungeon map submenu)
  int get_nb_floors_displayed(void);
  int get_highest_floor_displayed(int current_floor);

  // dungeon elements
  DungeonElement *get_boss(void);
};

#endif
