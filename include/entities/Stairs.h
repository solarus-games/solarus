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
#ifndef SOLARUS_STAIRS_H
#define SOLARUS_STAIRS_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * @brief Some stairs that trigger a special animation on the hero.
 *
 * A stairs entity is a special terrain where the hero walks
 * either before going to another floor
 * or to change its layer inside a single floor.
 *
 * For stairs inside a single floor,
 * only one stairs entity has to be placed and the collisions will be detected on both layers.
 * This entity is usually placed on the low layer even if there is no obligation.
 *
 * For stairs that send the hero to another floor (i.e. another map),
 * there must be three entities:
 * - a teletransporter (that make the hero go to the next map),
 * - a stairs entity (that make the hero take the stairs animation),
 * - a destination point on the next map.
 */
class Stairs: public Detector {

  public:

    /**
     * @brief Subtypes of stairs.
     */
    enum Subtype {
      SPIRAL_UPSTAIRS,          /**< going to the floor upstairs (spiral staircase) */
      SPIRAL_DOWNSTAIRS,        /**< going to the floor downstairs (spiral staircase) */
      STRAIGHT_UPSTAIRS,        /**< going to the floor upstairs (straight staircase) */
      STRAIGHT_DOWNSTAIRS,      /**< going to the floor downstairs (straight staircase) */
      INSIDE_FLOOR              /**< small stairs inside a single floor, changing the hero's layer */
    };

      
    /**
     * @brief Indicates the way an entity is walking on stairs.
     * - For stairs inside a single floor, 0 means that the hero is going upstairs
     *   and 1 means that he is going downstairs.
     * - For stairs between two different floors, 0 means that the hero is on the stairs
     *   of the old map and 1 means that he is on the stairs on the new map.
     */
    enum Way {
      NORMAL_WAY,    /**< for stairs inside a single floor, indicates that the entity is going upstairs;
		      * for stairs between two floors, indicates that the entity is leaving this floor */
      REVERSE_WAY    /**< for stairs inside a single floor, indicates that the entity is going downstairs;
                      * for stairs between two floors, indicates that the entity is arriving on this floor */
    };

  private:

    Subtype subtype;            /**< subtype of stairs */

  public:

    Stairs(Layer layer, int x, int y, int direction, Subtype subtype);
    ~Stairs();
    static CreationFunction parse;

    EntityType get_type();

    bool is_inside_floor();
    bool has_layer_independent_collisions();
    bool is_sword_ignored();
    bool is_obstacle_for(MapEntity *other);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);

    int get_movement_direction(Way way);
    int get_animation_direction(Way way);
    void play_sound(Way way);
    std::string get_path(Way way);
    Rectangle get_clipping_rectangle(Way way);
};

#endif

