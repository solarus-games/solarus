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
#ifndef SOLARUS_HERO_STAIRS_STATE_H
#define SOLARUS_HERO_STAIRS_STATE_H

#include "solarus/hero/State.h"
#include "solarus/entities/Stairs.h"

namespace Solarus {

/**
 * \brief The state "Stairs" of the hero.
 */
class Hero::StairsState: public Hero::State {

  public:

    StairsState(
        Hero& hero,
        Stairs& stairs,
        Stairs::Way way
    );

    virtual void start(const State* previous_state) override;
    virtual void stop(const State* next_state) override;
    virtual void set_map(Map& map) override;
    virtual void update() override;
    virtual void set_suspended(bool suspended) override;

    virtual bool is_touching_ground() const override;
    virtual bool can_come_from_bad_ground() const override;
    virtual bool is_teletransporter_delayed() const override;
    virtual int get_wanted_movement_direction8() const override;
    virtual std::shared_ptr<CarriedItem> get_carried_item() const override;
    virtual CarriedItem::Behavior get_previous_carried_item_behavior() const override;
    virtual void notify_layer_changed() override;

  private:

    Stairs& stairs;                    /**< the stairs the hero is currently taking */
    Stairs::Way way;                   /**< indicates the way the hero is taking the stairs:
                                        * - for stairs inside a single floor, NORMAL_WAY means that the hero is going upstairs
                                        * and REVERSE_WAY means the contrary;
                                        * - for stairs between two floors, NORMAL_WAY means that the hero is on the stairs
                                        *   of the old map and REVERSE_WAY means that he is on the stairs on the new map */
    int phase;                         /**< when walking on stairs between two floors, indicates the current
                                        * phase of the animations (0: not started, 1: initial animation,
                                        * 2: diagonal animation, 3: final animation) */
    uint32_t next_phase_date;          /**< date when the stairs phase changes */
    std::shared_ptr<CarriedItem>
        carried_item;                  /**< an item carried by the hero while taking the stairs, or nullptr */

};

}

#endif

