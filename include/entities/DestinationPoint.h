/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_DESTINATION_POINT_H
#define SOLARUS_DESTINATION_POINT_H

#include "Common.h"
#include "MapEntity.h"

/**
 * @brief A location of the map where the hero can arrive when using a teletransporter.
 */
class DestinationPoint: public MapEntity {

  private:

    bool is_visible;
    bool change_direction;

  public:

    DestinationPoint(const std::string &name, Layer layer, int x, int y, int hero_direction, bool is_visible);
    ~DestinationPoint();
    static CreationFunction parse;

    EntityType get_type();
};

#endif

