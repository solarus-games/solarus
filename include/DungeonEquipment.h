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
#ifndef SOLARUS_DUNGEON_EQUIPMENT_H
#define SOLARUS_DUNGEON_EQUIPMENT_H

#include "Common.h"

/**
 * @brief Represents the hero's equipment in a dungeon.
 *
 * This class represents the hero's equipment in a dungeon
 * (the map, the compass, the boss keys...).
 * Like the class Equipment, it makes the interface between
 * the savegame storage and other engine classes.
 * This class can be used only when the player is in a dungeon.
 * Note that the small keys are not handled here because small keys
 * can exist outside a dungeon. They are handled in class Equipment.
 */
class DungeonEquipment {

  private:

    Savegame *savegame;
    Game *game;                                /**< the current game */
    int get_current_dungeon(void);

  public:

    DungeonEquipment(Savegame *savegame);
    ~DungeonEquipment(void);
    void set_game(Game *game);

    bool has_map(void);
    void add_map(void);

    bool has_compass(void);
    void add_compass(void);

    bool has_big_key(void);
    void add_big_key(void);

    bool has_boss_key(void);
    void add_boss_key(void);
};

#endif

