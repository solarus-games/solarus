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
#define SOLARUS_ENEMY_SCRIPT_H

#include "Common.h"
#include "lua/Script.h"
#include "entities/EnemyAttack.h"
#include "entities/Layer.h"

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
    void event_appear();
    void event_enabled();
    void event_disabled();
    void event_restart();
    void event_pre_display();
    void event_post_display();
    void event_position_changed(const Rectangle& xy);
    void event_layer_changed(Layer layer);
    void event_obstacle_reached();
    void event_movement_changed(Movement& movement);
    void event_movement_finished(Movement& movement);
    void event_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame);
    void event_sprite_animation_finished(Sprite& sprite, const std::string& animation);
    int event_custom_attack_received(EnemyAttack attack, Sprite* sprite);
    void event_hurt(EnemyAttack attack, int life_lost);
    void event_dead();
};

#endif

