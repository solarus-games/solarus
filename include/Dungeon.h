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
  
  // elements of each floor: chests and bosses
  struct DungeonElement {
    int savegame_variable;
    int floor;
    int x;
    int y;
    bool big; // to distinguish big chests from normal chests and big bosses from minibosses
  };

 private:

  int dungeon_number;                  /**< dungeon number, between 1 and 20 */

  // floors
  int lowest_floor;                    /**< lowest floor number, between -16 and 15 */
  std::vector<SDL_Rect> floor_sizes;   /**< size of each floor */

  std::vector<DungeonElement> *chests; /**< properties of each chest for each floor */
  std::vector<DungeonElement> *bosses; /**< properties of each miniboss and boss for each floor */
  int boss_floor;                      /**< floor of the boss */

  void load(void);

 public:

  // creation and destruction
  Dungeon(int dungeon_number);
  ~Dungeon(void);

  int get_number(void);
  std::string get_name(void);

  // floors
  int get_nb_floors(void);
  int get_lowest_floor(void);
  int get_highest_floor(void);
  const SDL_Rect *get_floor_size(int floor);

  // displaying floors (for the hud and the dungeon map submenu)
  int get_nb_floors_displayed(void);
  int get_highest_floor_displayed(int current_floor);

  // dungeon elements
  int get_boss_floor(void);
  const std::vector<DungeonElement> get_bosses(int floor);
  const std::vector<DungeonElement> get_chests(int floor);
};

#endif
