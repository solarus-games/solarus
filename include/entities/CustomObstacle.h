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
#ifndef SOLARUS_CUSTOM_OBSTACLE_H
#define SOLARUS_CUSTOM_OBSTACLE_H

#include "Common.h"
#include "entities/MapEntity.h"
#include <map>

/**
 * @brief An invisible obstacle that stops other types of entities.
 *
 * The types of entities that are stopped can be specified.
 * This entity is typically used to prevent enemies from going to
 * a specific place.
 */
class CustomObstacle: public MapEntity {

  private:

    std::map<EntityType, bool> entity_types_stopped;

    bool enabled;              /**< indicates that the obstacle is enabled (default: yes) */
    bool waiting_enabled;      /**< indicates that the obstacle is waiting to be enabled */

  public:

    CustomObstacle(const std::string &name, Layer layer, int x, int y, int width, int height,
	bool stops_hero, bool stops_enemies, bool stops_npcs, bool stops_blocks);
    ~CustomObstacle();
    static CreationFunction parse;

    EntityType get_type();
    bool is_obstacle_for(MapEntity &other);
    void update();

    bool is_enabled();
    void set_enabled(bool enable);
};

#endif

