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
#ifndef SOLARUS_RELATIVE_MOVEMENT_H
#define SOLARUS_RELATIVE_MOVEMENT_H

#include "solarus/core/Common.h"
#include "solarus/entities/EntityPtr.h"
#include "solarus/movements/Movement.h"

namespace Solarus {

/**
 * \brief Movement of an entity attached to another entity.
 *
 * At each moment, the entity is placed at a position
 * relative to the entity followed.
 * If the movement is sensible to obstacles, it becomes finished as
 * soon as the entity cannot be placed at the requested position.
 */
class RelativeMovement: public Movement {

  public:

    RelativeMovement(
        const EntityPtr& entity_followed,
        int x,
        int y,
        bool ignore_obstacles
    );

    RelativeMovement(
        const EntityPtr& entity_followed,
        const Point& entity_offset,
        bool ignore_obstacles
    );

    virtual bool is_finished() const override;
    virtual Point get_displayed_xy() const override;

  protected:

    virtual void update() override;

  private:

    EntityPtr entity_followed;      /**< The entity followed by this movement or nullptr. */
    const Point entity_offset;         /**< coordinate of where this entity should be placed,
                                        * relative to the entity followed */

    bool finished;                     /**< indicates that the movement is stopped because of obstacles */

};

}

#endif

