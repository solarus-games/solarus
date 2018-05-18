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
#ifndef SOLARUS_TRANSITION_FADE_H
#define SOLARUS_TRANSITION_FADE_H

#include "solarus/core/Common.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/Transition.h"

namespace Solarus {

/**
 * \brief Fade in or fade out transition effect.
 *
 * TODO separate both behaviors in two classes (color and opacity).
 */
class TransitionFade: public Transition {

  public:

    TransitionFade(Direction direction);

    void set_delay(uint32_t delay);

    virtual void start() override;
    bool is_colored() const;
    const Color& get_color() const;
    void set_color(const Color& color);
    void clear_color();
    virtual bool is_started() const override;
    virtual bool is_finished() const override;
    virtual void notify_suspended(bool suspended) override;
    virtual void update() override;
    virtual void draw(Surface& dst_surface, const Surface& src_surface, const DrawInfos& infos) const override;
    virtual void finish(Drawable& target) const override;

  private:

    bool finished;
    int alpha_start;
    int alpha_limit;
    int alpha_increment;
    int alpha; // current alpha value of the surface

    uint32_t next_frame_date;
    uint32_t delay;

    bool colored;
    Color transition_color;

};

}

#endif

