/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_TRANSITION_SCROLLING_H
#define SOLARUS_TRANSITION_SCROLLING_H

#include "solarus/core/Common.h"
#include "solarus/core/Rectangle.h"
#include "solarus/graphics/SurfacePtr.h"
#include "solarus/graphics/Transition.h"

namespace Solarus {

/**
 * \brief Scrolling transition between two maps.
 *
 * This kind of transition can only be used for the transition between two maps.
 * The new map is drawn gradually, with a scrolling effect, and the hero
 * is drawn between both maps.
 * Nothing happens during the out transition. During the in transition,
 * the previous map does not exist anymore but its surface was saved
 * and we can still draw it.
 */
class TransitionScrolling: public Transition {

  public:

    explicit TransitionScrolling(Direction direction);

    virtual bool needs_previous_surface() const override;

    virtual void start() override;
    virtual bool is_started() const override;
    virtual bool is_finished() const override;
    virtual void notify_suspended(bool suspended) override;
    virtual void update() override;
    virtual void draw(Surface& dst_surface, const Surface& src_surface, const DrawInfos& infos) const override;

  private:

    void scroll();
    Rectangle get_previous_map_dst_position(int scrolling_direction);

    int scrolling_direction;              /**< direction of the scrolling (0 to 3) */
    uint32_t next_scroll_date;            /**< date of the next scrolling step */

    int dx;                               /**< x change for each scrolling step */
    int dy;                               /**< y change for each scrolling step */

    Rectangle previous_map_dst_position;  /**< position where the previous map is blitted on both_maps_surface */
    Rectangle current_map_dst_position;   /**< position where the current map is blitted on both_maps_surface */
    Rectangle current_scrolling_position; /**< the rectangle from both_maps_surface that is currently visible */

};

}

#endif

