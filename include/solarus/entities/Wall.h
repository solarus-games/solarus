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
#ifndef SOLARUS_WALL_H
#define SOLARUS_WALL_H

#include "solarus/Common.h"
#include "solarus/entities/MapEntity.h"
#include <set>

namespace Solarus {

/**
 * \brief An invisible obstacle that stops other types of entities.
 *
 * The types of entities that are stopped can be specified.
 * This entity is typically used to prevent enemies from going to
 * a specific place.
 */
class Wall: public MapEntity {

  public:

    Wall(
        const std::string& name,
        Layer layer,
        const Point& xy,
        const Size& size,
        bool stops_hero,
        bool stops_enemies,
        bool stops_npcs,
        bool stops_blocks,
        bool stops_projectiles
    );

    virtual EntityType get_type() const override;
    virtual bool can_be_drawn() const override;
    virtual bool is_obstacle_for(MapEntity& other) override;

  private:

    std::set<EntityType> entity_types_stopped;

};

}

#endif

