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
#ifndef SOLARUS_FIRE_H
#define SOLARUS_FIRE_H

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"

namespace Solarus {

/**
 * \brief Represents some fire on the map.
 *
 * Fire can interact with entities.
 */
class Fire: public Detector {

  public:

    Fire(const std::string& name, Layer layer, const Point& xy);

    virtual EntityType get_type() const override;
    virtual bool can_be_obstacle() const override;

    // state
    virtual void update() override;

    // collisions
    virtual void notify_collision(
        MapEntity& other_entity,
        Sprite& this_sprite,
        Sprite& other_sprite
    ) override;

};

}

#endif
