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
#ifndef SOLARUS_TIMER_H
#define SOLARUS_TIMER_H

#include "solarus/Common.h"
#include "solarus/lua/ExportableToLua.h"
#include <string>

namespace Solarus {

/**
 * \brief Represents a timer that any class can start.
 *
 * Timers are mostly used by the scripts.
 */
class Timer: public ExportableToLua {

  public:

    Timer(uint32_t duration);

    bool is_with_sound() const;
    void set_with_sound(bool with_sound);
    bool is_suspended() const;
    void set_suspended(bool suspended);
    bool is_suspended_with_map() const;
    void set_suspended_with_map(bool suspend_with_map);
    bool is_finished() const;
    uint32_t get_initial_duration() const;
    uint32_t get_expiration_date() const;
    void set_expiration_date(uint32_t expiration_date);

    void update();
    void notify_map_suspended(bool suspended);

    virtual const std::string& get_lua_type_name() const override;

  private:

    uint32_t expiration_date;        /**< Date when the timer is finished. */
    uint32_t duration;               /**< Initial duration of this timer. */
    bool finished;                   /**< Whether the timer is finished. */
    bool suspended_with_map;         /**< Whether the timer should be suspended when the map is. */
    bool suspended;                  /**< Whether the timer is suspended. */
    uint32_t when_suspended;         /**< Date when the timer was suspended. */
    uint32_t next_sound_date;        /**< Date when the next clock sound effect is played. */

};

}

#endif

