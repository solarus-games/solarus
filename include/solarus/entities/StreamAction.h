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
#ifndef SOLARUS_STREAM_ACTION_H
#define SOLARUS_STREAM_ACTION_H

#include "solarus/Common.h"
#include "solarus/entities/MapEntityPtr.h"
#include "solarus/lowlevel/Point.h"
#include <cstdint>
#include <memory>

namespace Solarus {

class Entity;
class Stream;

/**
 * \brief Manages a map entity being moved by a stream.
 *
 * An object of this class should be created when an entity has to be moved
 * by a stream, and can be destroyed when the effect of this stream on
 * this entity is finished. You can call is_active() to know if this happens.
 */
class StreamAction {

  public:

    StreamAction(Stream& stream, Entity& entity_moved);

    const Stream& get_stream() const;
    const Entity& get_entity_moved() const;
    bool is_active() const;

    void update();
    bool is_suspended() const;
    void set_suspended(bool suspended);

  private:

    void recompute_movement();
    bool test_obstacles(int dx, int dy);
    bool has_reached_target() const;

    std::shared_ptr<Stream>
        stream;                   /**< The stream applied,
                                   * or nullptr if it was destroyed. */
    EntityPtr entity_moved;    /**< The entity the stream is applied to,
                                   * or nullptr if it was destroyed. */
    bool active;                  /**< Whether the stream effect is currently
                                   * applied. */
    bool suspended;               /**< Whether the stream effect is currently
                                   * suspended. */
    uint32_t when_suspended;      /**< Date when the stream effect was suspended. */

    Point target;                 /**< Position where to stop. */
    uint32_t next_move_date;      /**< Date of the next one-pixel move. */
    uint32_t delay;               /**< Interval between two one-pixel moves. */

};

}

#endif

