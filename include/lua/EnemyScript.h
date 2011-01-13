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
#ifndef SOLARUS_ENEMY_SCRIPT_H
#define SOLARUS_ITEM_SCRIPT_H

#include "Common.h"
#include "lua/MapScript.h"

/**
 * @brief Represents the Lua script of an instance of enemy.
 *
 * This class makes the interface between the engine C++ code and the Lua script of an enemy.
 * If several instances of the same enemy exist in the map, each instance has its own script.
 */
class EnemyScript: public Script {

  private:

    Game& game;                                      /**< the game */
    CustomEnemy& enemy;                              /**< the enemy controlled by this script */

  protected:

    Game& get_game();
    Map& get_map();
    Enemy& get_enemy();

  public:

    EnemyScript(CustomEnemy& enemy);
    ~EnemyScript();

    void update();
    void set_suspended(bool suspended);

    void event_update();
    void event_set_suspended(bool suspended);
};

#endif

