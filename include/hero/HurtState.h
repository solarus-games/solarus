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
#ifndef SOLARUS_HERO_HURT_STATE_H
#define SOLARUS_HERO_HURT_STATE_H

#include "hero/State.h"

/**
 * @brief The state "Hurt" of the hero.
 */
class Hero::HurtState: public Hero::State {

  private:

    MapEntity &source;		/**< the entity that hurts the hero (usually an enemy) */
    int life_points;		/**< number of heart quarters to remove (this number may be reduced by the tunic) */
    int magic_points;		/**< number of magic points to remove */

  public:

    HurtState(Hero &hero, MapEntity &source, int life_points, int magic_points);
    ~HurtState();

    void start(State *previous_state);
    void stop(State *next_state);
    void update();
    bool can_start_gameover_sequence();
    bool is_touching_ground();
    bool is_teletransporter_obstacle(Teletransporter *teletransporter);
    bool is_conveyor_belt_obstacle(ConveyorBelt *conveyor_belt);
    bool is_sensor_obstacle(Sensor *sensor);
    bool can_be_hurt();
};

#endif

