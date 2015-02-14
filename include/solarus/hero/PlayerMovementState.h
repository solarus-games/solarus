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
#ifndef SOLARUS_HERO_PLAYER_MOVEMENT_STATE_H
#define SOLARUS_HERO_PLAYER_MOVEMENT_STATE_H

#include "solarus/Common.h"
#include "solarus/hero/State.h"
#include <cstdint>
#include <memory>
#include <string>

namespace Solarus {

class PlayerMovement;

/**
 * \brief State where the hero is controlled by the player.
 *
 * Abstract class for all hero states such that the movement is controlled by the player.
 * This means that the hero can move in the eight directions and the movement
 * is an instance of PlayerMovement.
 * These states include FreeState (the normal walk), SwordLoadingState (the sword is
 * loading), CarryingState (the hero is carrying something) and SwimmingState.
 */
class Hero::PlayerMovementState: public Hero::State {

  public:

    virtual ~PlayerMovementState();

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void set_map(Map& map) override;

    virtual void update() override;
    virtual void set_suspended(bool suspended) override;
    virtual void set_animation_stopped();
    virtual void set_animation_walking();
    virtual bool can_control_movement() const override;
    virtual int get_wanted_movement_direction8() const override;
    virtual void notify_walking_speed_changed() override;
    virtual void notify_movement_changed() override;
    virtual void notify_position_changed() override;
    virtual void notify_layer_changed() override;
    virtual bool can_be_hurt(MapEntity* attacker) const override;
    virtual bool can_pick_treasure(EquipmentItem& item) const override;
    virtual bool can_take_jumper() const override;
    virtual void notify_jumper_activated(Jumper& jumper) override;

  protected:

    PlayerMovementState(Hero& hero, const std::string& state_name);

    const std::shared_ptr<PlayerMovement>& get_player_movement();
    std::shared_ptr<const PlayerMovement> get_player_movement() const;

  private:

    void cancel_jumper();

    std::shared_ptr<PlayerMovement>
        player_movement;               /**< The movement created by this state.
                                        * The movement of the hero is also this object,
                                        * unless a script decided to change it. */
    std::shared_ptr<Jumper>
        current_jumper;                /**< The jumper about to be triggered or nullptr */
    uint32_t jumper_start_date;        /**< Date to trigger the jumper
                                        * (because a small delay is necessary) */

};

}

#endif

