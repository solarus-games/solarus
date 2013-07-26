/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_SEPARATOR_H
#define SOLARUS_SEPARATOR_H

#include "Common.h"
#include "entities/Detector.h"
#include <list>

/**
 * \brief An horizontal or vertical separation between two parts of the map.
 *
 * The camera cannot cross this separation except when the hero touches it:
 * in this case, scrolling is performed.
 * This entity allows to separate parts of the map like if there was several
 * maps.
 */
class Separator: public Detector {

  public:

    Separator(
        const std::string& name,
        Layer layer,
        int x,
        int y,
        int width,
        int height);
    ~Separator();

    EntityType get_type();

    bool is_horizontal() const;
    bool is_vertical() const;

    bool is_obstacle_for(MapEntity& other);
    bool test_collision_custom(MapEntity& entity);
    void notify_collision(
        MapEntity& entity_overlapping, CollisionMode collision_mode);
};

#endif

