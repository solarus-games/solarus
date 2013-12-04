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
#ifndef SOLARUS_HERO_STAIRS_STATE_H
#define SOLARUS_HERO_STAIRS_STATE_H

#include "hero/State.h"
#include "entities/Stairs.h"

/**
 * \brief The state "Stairs" of the hero.
 */
class Hero::StairsState: public Hero::State {

  public:

    StairsState(
        Hero& hero,
        Stairs& stairs,
        Stairs::Way way);
    ~StairsState();

    void start(const State* previous_state);
    void stop(const State* next_state);
    void set_map(Map& map);
    void update();
    void set_suspended(bool suspended);

    bool is_touching_ground() const;
    bool can_come_from_bad_ground() const;
    bool is_teletransporter_delayed() const;
    int get_wanted_movement_direction8() const;
    CarriedItem* get_carried_item() const;
    CarriedItem::Behavior get_previous_carried_item_behavior() const;
    void notify_layer_changed();

  private:

    Stairs& stairs;			/**< the stairs the hero is currently taking */
    Stairs::Way way;			/**< indicates the way the hero is taking the stairs:
					 * - for stairs inside a single floor, NORMAL_WAY means that the hero is going upstairs
					 * and REVERSE_WAY means the contrary;
					 * - for stairs between two floors, NORMAL_WAY means that the hero is on the stairs
					 *   of the old map and REVERSE_WAY means that he is on the stairs on the new map */
    int phase;				/**< when walking on stairs between two floors, indicates the current
					 * phase of the animations (0: not started, 1: initial animation,
					 * 2: diagonal animation, 3: final animation) */
    uint32_t next_phase_date;	 	/**< date when the stairs phase changes */
    CarriedItem* carried_item;		/**< an item carried by the hero while taking the stairs, or NULL */

    void destroy_carried_item();
};

#endif

