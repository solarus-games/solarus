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
#ifndef SOLARUS_HERO_RUNNING_STATE_H
#define SOLARUS_HERO_RUNNING_STATE_H

#include "solarus/hero/State.h"

namespace Solarus {

/**
 * \brief The state "Running" of the hero.
 */
class Hero::RunningState: public Hero::State {

  public:

    RunningState(Hero& hero, GameCommand command);

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;
    bool is_pressing_running_key() const;
    virtual void notify_direction_command_pressed(int direction4) override;
    virtual void notify_obstacle_reached() override;
    virtual int get_wanted_movement_direction8() const override;

    virtual bool can_take_jumper() const override;
    virtual void notify_jumper_activated(Jumper& jumper) override;
    virtual bool can_take_stairs() const override;
    virtual bool can_be_hurt(MapEntity* attacker) const override;
    virtual bool can_pick_treasure(EquipmentItem& item) const override;
    virtual bool can_sword_hit_crystal() const override;
    virtual bool can_start_gameover_sequence() const override;
    virtual bool is_touching_ground() const override;
    virtual bool can_avoid_deep_water() const override;
    virtual bool can_avoid_hole() const override;
    virtual bool can_avoid_lava() const override;
    virtual bool can_avoid_prickle() const override;
    virtual bool can_avoid_teletransporter() const override;
    virtual bool can_avoid_stream(const Stream& stream) const override;
    virtual bool can_persist_on_stream(const Stream& stream) const override;
    virtual bool is_sensor_obstacle(const Sensor& sensor) const override;
    virtual bool is_cutting_with_sword(Detector& detector) override;
    virtual int get_sword_damage_factor() const override;

  private:

    bool is_bouncing() const;

    int phase;                      /**< current phase of the run */
    uint32_t next_phase_date;       /**< date of the next phase */
    uint32_t next_sound_date;       /**< date of the next time a sound is played during the run */
    GameCommand command;            /**< the command pressed to make the hero run */

};

}

#endif

