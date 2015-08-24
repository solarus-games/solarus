/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include "solarus/entities/HeroPtr.h"
#include "solarus/entities/MapEntity.h"
#include <memory>
#include <string>

namespace Solarus {

/**
 * \brief A boomerang thrown on the map.
 *
 * In the current implementation, only the hero is supposed to be able to
 * throw a boomerang.
 */
class Boomerang: public Entity {

  public:

    Boomerang(
        const HeroPtr& hero,
        int max_distance,
        int speed,
        double angle,
        const std::string& sprite_name
    );

    EntityType get_type() const override;
    virtual bool can_be_obstacle() const override;
    virtual bool can_be_drawn() const override;

    // obstacles
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter) override;
    virtual bool is_stream_obstacle(Stream& stream) override;
    virtual bool is_stairs_obstacle(Stairs& stairs) override;
    virtual bool is_low_wall_obstacle() const override;
    virtual bool is_deep_water_obstacle() const override;
    virtual bool is_hole_obstacle() const override;
    virtual bool is_lava_obstacle() const override;
    virtual bool is_prickle_obstacle() const override;
    virtual bool is_ladder_obstacle() const override;
    virtual bool is_switch_obstacle(Switch& sw) override;
    virtual bool is_raised_block_obstacle(CrystalBlock& raised_block) override;
    virtual bool is_crystal_obstacle(Crystal& crystal) override;
    virtual bool is_npc_obstacle(Npc& npc) override;
    virtual bool is_jumper_obstacle(Jumper& jumper, const Rectangle& candidate_position) override;

    // state
    bool is_going_back() const;
    void go_back();
    virtual void update() override;

    // movement
    virtual void notify_obstacle_reached() override;
    virtual void notify_movement_finished() override;

    // collisions
    virtual void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) override;
    virtual void notify_collision_with_crystal(Crystal& crystal, CollisionMode collision_mode) override;
    virtual void notify_collision_with_enemy(Enemy& enemy) override;
    virtual void notify_attacked_enemy(
        EnemyAttack attack,
        Enemy& victim,
        const Sprite* victim_sprite,
        EnemyReaction::Reaction& result,
        bool killed
    ) override;

  private:

    HeroPtr hero;                   /**< The hero who shot this boomerang. */

    uint32_t next_sound_date;       /**< date when the boomerang sound is played next time */

    bool has_to_go_back;            /**< true if the boomerang is about to go back */
    bool going_back;                /**< indicates that the boomerang is going back towards the hero */

    int speed;                      /**< speed of the movement in pixels per second */

};

}

#endif

