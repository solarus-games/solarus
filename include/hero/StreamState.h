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
#ifndef SOLARUS_HERO_STREAM_STATE_H
#define SOLARUS_HERO_STREAM_STATE_H

#include "hero/State.h"

namespace solarus {

/**
 * \brief The state "stream" of the hero.
 */
class Hero::StreamState: public Hero::State {

  public:

    StreamState(Hero& hero, Stream& stream);
    ~StreamState();

    void start(const State* previous_state);
    void stop(const State* next_state);
    void update();

    bool can_avoid_teletransporter() const;
    bool can_avoid_stream(const Stream& stream) const;
    bool can_start_sword() const;
    bool can_start_item(EquipmentItem& item) const;

  private:

    Stream& stream;          /**< The stream to take. */
    bool snapping;           /**< Whether the hero is currently moving towards the
                              * center of the stream, before following its real direction. */

};

}

#endif

