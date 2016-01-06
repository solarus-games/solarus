/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_HERO_JUMPING_STATE_H
#define SOLARUS_HERO_JUMPING_STATE_H

#include "solarus/hero/BaseState.h"
#include <memory>

namespace Solarus {

class JumpMovement;

/**
 * \brief The state "Jumping" of the hero.
 */
class Hero::JumpingState: public Hero::BaseState {

  public:

    JumpingState(
        Hero& hero,
        int direction8,
        int distance,
        bool ignore_obstacles,
        bool with_sound
    );

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void set_map(Map& map) override;
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;
    virtual void notify_layer_changed() override;
    virtual int get_wanted_movement_direction8() const override;
    virtual bool can_start_gameover_sequence() const override;
    virtual bool is_touching_ground() const override;
    virtual bool is_stairs_obstacle(const Stairs& stairs) const override;
    virtual bool is_sensor_obstacle(const Sensor& sensor) const override;
    virtual bool is_separator_obstacle(const Separator& separator) const override;
    virtual bool can_avoid_deep_water() const override;
    virtual bool can_avoid_hole() const override;
    virtual bool can_avoid_ice() const override;
    virtual bool can_avoid_lava() const override;
    virtual bool can_avoid_prickle() const override;
    virtual bool can_avoid_teletransporter() const override;
    virtual bool can_avoid_stream(const Stream& stream) const override;
    virtual bool can_avoid_sensor() const override;
    virtual bool can_avoid_switch() const override;
    virtual bool can_be_hurt(Entity* attacker) const override;
    virtual std::shared_ptr<CarriedItem> get_carried_item() const override;
    virtual CarriedItem::Behavior get_previous_carried_item_behavior() const override;

  private:

    std::shared_ptr<JumpMovement>
        movement;                 /**< the movement applied to the hero */
    int direction8;               /**< direction of the jump (0 to 7) */
    bool with_sound;              /**< indicates that a jump sound is played */
    std::shared_ptr<CarriedItem>
        carried_item;             /**< an item carried by the hero while making
                                   * this jump, or nullptr */

};

}

#endif

