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
#ifndef SOLARUS_TELETRANSPORTER_H
#define SOLARUS_TELETRANSPORTER_H

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"
#include "solarus/Transition.h"

namespace Solarus {

/**
 * \brief A detector that sends the hero to a destination.
 *
 * A teletransporter is a detector placed on a map.
 * When the hero walks on this detector, he is placed on a destination
 * on the same map or another one.
 */
class Teletransporter: public Detector {

  public:

    Teletransporter(
        const std::string& name,
        Layer layer,
        const Point& xy,
        const Size& size,
        const std::string& sprite_name,
        const std::string& sound_id,
        Transition::Style transition_style,
        const std::string& destination_map_id,
        const std::string& destination_name
    );

    virtual EntityType get_type() const override;
    virtual void notify_creating() override;

    const std::string& get_sound_id() const;
    void set_sound_id(const std::string& sound_id);
    Transition::Style get_transition_style() const;
    void set_transition_style(Transition::Style transition_style);
    const std::string& get_destination_map_id() const;
    void set_destination_map_id(const std::string& map_id);
    const std::string& get_destination_name() const;
    void set_destination_name(const std::string& destination_name);
    bool is_on_map_side() const;

    virtual bool is_obstacle_for(MapEntity& other) override;
    virtual bool test_collision_custom(MapEntity& entity) override;
    virtual void notify_collision(
        MapEntity& entity_overlapping,
        CollisionMode collision_mode
    ) override;
    void transport_hero(Hero& hero);

  private:

    std::string sound_id;                 /**< Sound played when this teletransporter is used
                                           * (an empty string means no sound). */
    Transition::Style transition_style;   /**< Style of transition between the two maps. */
    std::string destination_map_id;       /**< Id of the destination map. */
    std::string destination_name;         /**< Destination on that map, or "_same" to keep the hero's coordinates,
                                           * or "_side" to place the hero on the appropriate side of the map.
                                           * An empty string means the default destination entity of the map. */
    int destination_side;                 /**< When the destination is "_side", indicates which side
                                           * of the destination map this teletransporters leads to.
                                           * This depends on the teletransporter position on the original map.
                                           * -1 means a teletransporter other than "_side". */
    int transition_direction;             /**< When the destination is "_side", indicates the direction
                                           * of the transition between the two maps (this is the opposite
                                           * direction of destination_side). */
    bool transporting_hero;               /**< Whether the hero is currently being transported
                                           * by this teletransporter. */

};

}

#endif

