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
#ifndef SOLARUS_EXPLOSION_H
#define SOLARUS_EXPLOSION_H

#include "Common.h"
#include "entities/Detector.h"
#include <list>

/**
 * @brief Represents an explosion on the map.
 *
 * An explosion can hurt the hero, the enemies and open weak walls.
 */
class Explosion: public Detector {

  private:

    std::list<Enemy*> victims; /**< list of enemies successfully hurt by this explosion */

  public:

    Explosion(Layer layer, const Rectangle &xy, bool with_damages);
    ~Explosion();

    EntityType get_type();

    // features
    bool can_be_obstacle();
    bool can_detect_entities();
    bool can_be_displayed();
    bool is_displayed_in_y_order();

    // state
    void update();
    void notify_sprite_frame_changed(Sprite& sprite, const std::string& animation, int frame);

    // collisions
    void notify_collision(MapEntity &other_entity, Sprite &other_sprite, Sprite &this_sprite);
    void notify_collision_with_switch(Switch& sw, Sprite& sprite_overlapping);
    void notify_collision_with_crystal(Crystal &crystal, Sprite &sprite_overlapping);
    void notify_collision_with_sensor(Sensor& sensor, CollisionMode collision_mode);
    void notify_collision_with_enemy(Enemy &enemy, Sprite &enemy_sprite, Sprite &this_sprite);
    void try_attack_enemy(Enemy &enemy, Sprite &enemy_sprite);
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim, EnemyReaction::Reaction& result, bool killed);
};

#endif

