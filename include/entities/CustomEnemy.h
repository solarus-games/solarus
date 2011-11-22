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
#ifndef SOLARUS_CUSTOM_ENEMY_H
#define SOLARUS_CUSTOM_ENEMY_H

#include "Common.h"
#include "entities/Enemy.h"

/**
 * @brief An enemy controlled by a Lua script.
 * TODO: merge with Enemy since all enemies are custom enemies now
 */
class CustomEnemy: public Enemy {

  private:

    std::string breed;             /**< breed of the enemy (determines its script file) */
    EnemyScript* script;           /**< the script that controls this enemy */

  protected:

    void initialize();

  public:

    CustomEnemy(const ConstructionParameters& params, const std::string& breed);
    ~CustomEnemy();

    const std::string& get_breed();

    void set_map(Map& map);
    void update();
    void set_suspended(bool suspended);
    void display_on_map();

    void restart();
    void notify_enabled(bool enabled);
    void notify_obstacle_reached();
    void notify_position_changed();
    void notify_layer_changed();
    void notify_movement_changed();
    void notify_movement_finished();
    void notify_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame);
    void notify_sprite_animation_finished(Sprite& sprite, const std::string& animation);
    void notify_collision_with_enemy(Enemy& other, Sprite& other_sprite, Sprite& this_sprite);
    void custom_attack(EnemyAttack attack, Sprite* this_sprite);
    void notify_hurt(MapEntity& source, EnemyAttack attack, int life_points);
    void notify_dead();
    void notify_immobilized();

    void notify_message_received(Enemy& sender, const std::string& message);
};

#endif

