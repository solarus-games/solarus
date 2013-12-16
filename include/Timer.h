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
#ifndef SOLARUS_TIMER_H
#define SOLARUS_TIMER_H

#include "Common.h"
#include "lua/ExportableToLua.h"

namespace solarus {

/**
 * \brief Represents a timer that any class can start.
 *
 * Timers are mostly used by the scripts.
 */
class Timer: public ExportableToLua {

  public:

    Timer(uint32_t duration);
    ~Timer();

    bool is_with_sound();
    void set_with_sound(bool with_sound);
    bool is_suspended();
    void set_suspended(bool suspended);
    bool is_suspended_with_map();
    void set_suspended_with_map(bool suspend_with_map);
    bool is_finished();

    void update();
    void notify_map_suspended(bool suspended);

    virtual const std::string& get_lua_type_name() const;

  private:

    // timer
    uint32_t expiration_date;        /**< date when the timer is finished */
    bool finished;                   /**< indicates that the timer is finished */

    bool suspended_with_map;         /**< whether the timer should be suspended when the map is */
    bool suspended;                  /**< indicates whether the timer is suspended */
    uint32_t when_suspended;         /**< date when the timer was suspended */

    // sound
    uint32_t next_sound_date;        /**< date when the next clock sound effect is played */
};

}

#endif

