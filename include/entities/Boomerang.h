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
#ifndef SOLARUS_BOOMERANG_H
#define SOLARUS_BOOMERANG_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * @brief A boomerang thrown on the map.
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
    int max_distance;               /**< maximum distance to traverse in pixel */
    int speed;                      /**< speed of the movement in pixels per second */

  public:

    Boomerang(Hero& hero, int max_distance, int speed, double angle,
        const SpriteAnimationSetId& sprite_name);
    ~Boomerang();

    EntityType get_type();

    // features
    bool can_be_obstacle();
    bool can_detect_entities();
    bool can_be_displayed();
    bool is_displayed_in_y_order();

    bool is_teletransporter_obstacle(Teletransporter& teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt& conveyor_belt);
    bool is_stairs_obstacle(Stairs& stairs);
    bool is_deep_water_obstacle();
    bool is_hole_obstacle();
    bool is_lava_obstacle();
    bool is_prickle_obstacle();
    bool is_ladder_obstacle();
    bool is_switch_obstacle(Switch& sw);
    bool is_raised_block_obstacle(CrystalBlock& raised_block);
    bool is_crystal_obstacle(Crystal& crystal);
    bool is_npc_obstacle(NPC& npc);
    bool is_jumper_obstacle(Jumper& jumper);

    // state
    bool is_going_back();
    void go_back();

    void update();

    // collisions
    void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode);
    void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode);
    void notify_collision_with_enemy(Enemy& enemy);
    void notify_attacked_enemy(EnemyAttack attack, Enemy& victim, EnemyReaction::Reaction& result, bool killed);
};

#endif

