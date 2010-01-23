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
#ifndef SOLARUS_TELETRANSPORTER_H
#define SOLARUS_TELETRANSPORTER_H

#include "Common.h"
#include "Detector.h"
#include "Transition.h"

/**
 * A teletransporter is a detector placed on a map.
 * When the hero walks on this detector, he is placed on a destination point
 * on the same map or another one.
 */
class Teletransporter: public Detector {

  public:

    /**
     * Subtypes of teletransporters.
     */
    enum Subtype {
      INVISIBLE = 0, /**< an invisible detector, usually to move the hero to another map */
      YELLOW    = 1, /**< a classical teletransporter */
      BLUE      = 2, /**< a teletransporter with a different color and an immediate transition */
    };

  private:

    Subtype subtype;                      /**< subtype of teletransporter */
    Transition::Style transition_style;   /**< style of transition between the two maps */
    Sound *sound;                         /**< the sound played when this teletransporter is taken (can be NULL) */
    MapId destination_map_id;             /**< id of the destination map */
    std::string destination_point_name;        /**< destination point on that map, or "_same" to keep the hero's coordinates,
						* or "_side" to place the hero on the appropriate side of the map */
    int destination_side;                 /**< when the destination point is "_side", indicates which side
					   * of the destination map this teletransporters leads to
					   * (this depends on the teletransporter position on the map */
    int transition_direction;             /**< when the destination point is "_side", indicates the direction 
					   * of the transition between the two maps (this is the opposite
					   * direction of destination_side) */
    bool transporting_hero;               /**< true if the hero is currently being transported by this teletransporter */

  public:

    Teletransporter(const std::string &name, Layer layer, int x, int y, int width, int height,
	Subtype subtype, Transition::Style transition_style,
	MapId destination_map_id, std::string destination_point_name);
    ~Teletransporter(void);
    static CreationFunction parse;

    EntityType get_type(void);
    void set_map(Map *map);

    bool is_obstacle_for(MapEntity *other);
    bool test_collision_custom(MapEntity *entity);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    void transport_hero(Hero *hero);
};

#endif

