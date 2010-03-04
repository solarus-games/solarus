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
#ifndef SOLARUS_INTERNAL_STAIRS_H
#define SOLARUS_INTERNAL_STAIRS_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * A special terrain where the hero walks from 
 * the low layer to the intermediate layer (or the opposite)
 * in the same map by taking stairs.
 *
 * Only one entity has to be placed and the collisions will be detected on both layers.
 * This entity is usually placed on the intermediate layer
 * even if there is no obligation.
 */
class InternalStairs: public Detector {

  public:

    InternalStairs(Layer layer, int x, int y, int direction);
    ~InternalStairs(void);
    static CreationFunction parse;

    EntityType get_type(void);

    bool is_sword_ignored(void);
    bool is_obstacle_for(MapEntity *other);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
};

#endif

