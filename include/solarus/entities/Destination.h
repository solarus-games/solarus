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
#ifndef SOLARUS_DESTINATION_H
#define SOLARUS_DESTINATION_H

#include "solarus/Common.h"
#include "solarus/entities/MapEntity.h"

namespace Solarus {

/**
 * \brief A location of the map where the hero can arrive when using a teletransporter.
 */
class Destination: public MapEntity {

  public:

    Destination(
        const std::string& name,
        Layer layer,
        const Point& xy,
        int hero_direction,
        const std::string& sprite_name,
        bool is_default
    );

    virtual EntityType get_type() const override;
    virtual bool can_be_obstacle() const override;
    bool is_default() const;

  private:

    bool is_default_destination;

};

}

#endif

