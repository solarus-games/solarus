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
#ifndef SOLARUS_TRANSITION_IMMEDIATE_H
#define SOLARUS_TRANSITION_IMMEDIATE_H

#include "solarus/core/Common.h"
#include "solarus/graphics/Transition.h"

namespace Solarus {

/**
 * \brief Immediate transition effect.
 *
 * This transition just acts as if there was no transition:
 * the next surface is immediately displayed.
 */
class TransitionImmediate: public Transition {

  public:

    explicit TransitionImmediate(Direction direction);

    virtual void start() override;
    virtual bool is_started() const override;
    virtual bool is_finished() const override;
    virtual void notify_suspended(bool suspended) override;
    virtual void update() override;
    virtual void draw(Surface& dst_surface, const Surface& src_surface, const DrawInfos& infos) const override;

};

}

#endif

