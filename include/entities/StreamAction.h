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
#ifndef SOLARUS_STREAM_ACTION_H
#define SOLARUS_STREAM_ACTION_H

#include "Common.h"

namespace solarus {

/**
 * \brief Manages a map entity being moved by a stream.
 *
 * An object of this class should be created when an entity has to be moved
 * by a stream, and can be destroyed when the effect of this stream on
 * this entity is finished. You can call is_active() to know if this happens.
 */
class StreamAction {

  public:

    StreamAction(Stream& stream, MapEntity& entity_moved);
    ~StreamAction();

    const Stream& get_stream() const;
    const MapEntity& get_entity_moved() const;
    bool is_active() const;

    void update();
    bool is_suspended() const;
    void set_suspended(bool suspended);

  private:

    bool test_obstacles();

    Stream* stream;               /**< The stream applied,
                                   * or NULL if it was destroyed. */
    MapEntity* entity_moved;      /**< The entity the stream is applied to,
                                   * or NULL if it was destroyed. */
    bool active;                  /**< Whether the stream effect is currently
                                   * applied. */
    bool suspended;               /**< Whether the stream effect is currently
                                   * suspended. */
    uint32_t when_suspended;      /**< Date when the stream effect was suspended. */

    int dx;                       /**< X change of the movement. */
    int dy;                       /**< Y change of the movement. */
    int target_x;                 /**< X coordinate where to stop. */
    int target_y;                 /**< Y coordinate where to stop. The stream stops
                                   * when one of target_x or target_y is reached. */
    uint32_t next_move_date;      /**< Date of the next one-pixel move. */
    uint32_t delay;               /**< Interval between two one-pixel moves. */
};

}

#endif

