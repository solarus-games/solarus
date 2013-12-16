/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_BOOMERANG_H
#define SOLARUS_BOOMERANG_H

#include "Common.h"
#include "entities/MapEntity.h"

namespace solarus {

/**
 * \brief A boomerang thrown on the map.
 *
 * In the current implementation, only the hero is supposed to be able to
 * throw a boomerang.
 */
class Boomerang: public MapEntity {

  private:

    Hero& hero;                     /**< the hero */

    uint32_t next_sound_date;       /**< date when the boomerang sound is played next time */

    bool has_to_go_back;            /**< true if the boomerang is about to go back */
    bool going_back;                /**< indicates that the boomerang is going back towards the hero */

    Rectangle initial_coords;       /**< coordinates of the boomerang's initial position */
    int speed;                      /**< speed of the movement in pixels per second */

  public:

    Boomerang(Hero& hero, int max_distance, int speed, double angle,
        const std::string& sprite_name);
    ~Boomerang();

    EntityType get_type() const;
    bool can_be_obstacle() const;
    bool can_be_drawn() const;

    // obstacles
    bool is_teletransporter_obstacle(const Teletransporter& teletransporter) const;
    bool is_conveyor_belt_obstacle(const ConveyorBelt& conveyor_belt) const;
    bool is_stairs_obstacle(const Stairs& stairs) const;
    bool is_low_wall_obstacle() const;
    bool is_deep_water_obstacle() const;
    bool is_hole_obstacle() const;
    bool is_lava_obstacle() const;
    bool is_prickle_obstacle() const;
    bool is_ladder_obstacle() const;
    bool is_switch_obstacle(const Switch& sw) const;
    bool is_raised_block_obstacle(const CrystalBlock& raised_block) const;
    bool is_crystal_obstacle(const Crystal& crystal) const;
    bool is_npc_obstacle(const NPC& npc) const;
    bool is_jumper_obstacle(const Jumper& jumper) const;

    // state
    bool is_going_back() const;
    void go_back();
    void update();

    // movement
    void notify_obstacle_reached();
    void notify_movement_finished();

    // collisions
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode);
    void notify_collision_with_enemy(Enemy& enemy);
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim, EnemyReaction::Reaction& result, bool killed);
};

}

#endif

