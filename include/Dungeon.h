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
#ifndef SOLARUS_DUNGEON_H
#define SOLARUS_DUNGEON_H

#include "Common.h"
#include <vector>

/**
 * @brief Represents the properties of a dungeon.
 *
 * Represents the properties of a dungeon, like the floors
 * and the chests it contains.
 * This class is used by the map submenu when the player is in a dungeon.
 * These properties are loaded from file dungeons.dat.
 */
class Dungeon {

  public:

    /**
     * @brief A dynamic element that may appear on the minimap.
     *
     * This structre represents a dynamic element that appears on the minimap of a floor
     * (a chest or a boss).
     */
    struct DungeonElement {
      int savegame_variable;    /**< index of the savegame variable that memorizes this element's state (chest open or boss killed) */
      int floor;                /**< the floor where this element appears */
      int x;                    /**< x coordinate of the element on the floor */
      int y;                    /**< y coordinate of the element on the floor */
      bool big;                 /**< to distinguish big chests from normal chests and big bosses from minibosses */
    };

  private:

    int dungeon_number;                  /**< dungeon number, between 1 and 20 */
    std::string name;                    /**< name of the dungeon in the current language */

    // floors
    int lowest_floor;                    /**< lowest floor number, between -16 and 15 */
    std::vector<Rectangle> floor_sizes;  /**< size of each floor */

    std::vector<DungeonElement> *chests; /**< properties of each chest for each floor */
    std::vector<DungeonElement> *bosses; /**< properties of each miniboss and boss for each floor */
    int boss_floor;                      /**< floor of the boss (-100 if there is no boss) */

    void load();

  public:

    // creation and destruction
    Dungeon(int dungeon_number);
    ~Dungeon();

    int get_number();
    const std::string &get_name();

    // floors
    int get_nb_floors();
    int get_lowest_floor();
    int get_highest_floor();
    const Rectangle & get_floor_size(int floor);

    // displaying floors (for the hud and the dungeon map submenu)
    int get_nb_floors_displayed();
    int get_highest_floor_displayed(int current_floor);

    // dungeon elements
    int get_boss_floor();
    const std::vector<DungeonElement> get_bosses(int floor);
    const std::vector<DungeonElement> get_chests(int floor);
};

#endif

