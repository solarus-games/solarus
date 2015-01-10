/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_BLOCK_H
#define SOLARUS_BLOCK_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/entities/Detector.h"

namespace Solarus {

/**
 * \brief A entity that the hero can push or pull.
 *
 * Some blocks can be moved only once or in only one direction.
 */
class Block: public Detector {

  public:

    Block(const std::string& name,
        Layer layer,
        const Point& xy,
        int direction,
        const std::string& sprite_name,
        bool can_be_pushed,
        bool can_be_pulled,
        int maximum_moves
    );

    virtual EntityType get_type() const override;
    virtual bool is_ground_observer() const override;

    virtual bool is_obstacle_for(MapEntity& other) override;
    virtual bool is_hole_obstacle() const override;
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter) override;
    virtual bool is_hero_obstacle(Hero& hero) override;
    virtual bool is_enemy_obstacle(Enemy& enemy) override;
    virtual bool is_destructible_obstacle(Destructible& destructible) override;
    virtual void notify_created() override;
    virtual void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) override;
    virtual void notify_collision_with_switch(Switch& sw, CollisionMode collision_mode) override;
    virtual bool notify_action_command_pressed() override;
    virtual bool start_movement_by_hero() override;
    virtual void stop_movement_by_hero() override;
    virtual void notify_moving_by(MapEntity& entity) override;
    virtual void notify_moved_by(MapEntity& entity) override;
    virtual void notify_position_changed() override;
    virtual void notify_obstacle_reached() override;
    virtual void notify_ground_below_changed() override;

    void reset();
    bool is_pushable() const;
    void set_pushable(bool pushable);
    bool is_pullable() const;
    void set_pullable(bool pullable);
    int get_maximum_moves() const;
    void set_maximum_moves(int maximum_moves);

  private:

    void movement_by_hero_finished();

    int maximum_moves;          /**< indicates whether the block can be pushed
                                 * (0: none, 1: once: 2: infinite) */
    bool sound_played;          /**< true if the block sound was played while pulling it */
    uint32_t when_can_move;     /**< date when the hero can move the block again */

    Point last_position;        /**< last position of the block before moving */
    Point initial_position;     /**< position of the block when created */
    int initial_maximum_moves;  /**< value of maximum_moves when the block was created */
    bool can_be_pushed;         /**< indicates that the hero can push this block */
    bool can_be_pulled;         /**< indicates that the hero can pull this block */

    static constexpr uint32_t moving_delay = 500; /**< delay between two successive moves of a block */

};

}

#endif

