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
#ifndef SOLARUS_DETECTOR_H
#define SOLARUS_DETECTOR_H

#include "solarus/Common.h"
#include "solarus/entities/MapEntity.h"
#include "solarus/entities/CollisionMode.h"

namespace Solarus {

class EquipmentItem;

/**
 * \brief Abstract class for an entity able to detect the presence of other
 * entities.
 *
 * A detector is an object placed somewhere on the map
 * to detect the presence of the hero or other moving entities.
 * Examples of detectors include teletransporters, switches and enemies.
 */
class Detector: public MapEntity {

  public:

    // destruction
    virtual ~Detector();
    virtual void notify_being_removed() override;

    // properties
    virtual bool has_layer_independent_collisions() const override;
    void set_layer_independent_collisions(bool independent);

    // position
    virtual void notify_position_changed() override;
    virtual void notify_layer_changed() override;

    // general collision checking functions
    void check_collision(MapEntity& entity);
    void check_collision(MapEntity& entity, Sprite& sprite);

    virtual bool notify_action_command_pressed();
    virtual bool interaction_with_item(EquipmentItem& item);  // TODO rename to notify_interaction_with_item
    virtual bool start_movement_by_hero();
    virtual void stop_movement_by_hero();
    virtual std::string get_sword_tapping_sound();

  protected:

    // construction
    Detector(int collision_mode, const std::string& name, Layer layer,
        const Point& xy, const Size& size);
    virtual bool is_detector() const override;

    // properties of the detector
    void set_collision_modes(int collision_modes);
    void add_collision_mode(CollisionMode collision_mode);
    bool has_collision_mode(CollisionMode collision_mode);
    void enable_pixel_collisions();

    // specialized collision checking functions
    bool test_collision_rectangle(MapEntity& entity);
    bool test_collision_inside(MapEntity& entity);
    bool test_collision_origin_point(MapEntity& entity);
    bool test_collision_facing_point(MapEntity& entity);
    bool test_collision_touching(MapEntity& entity);
    bool test_collision_center(MapEntity& entity);
    virtual bool test_collision_custom(MapEntity& entity);

    // functions called when a collision is detected
    virtual void notify_collision(
        MapEntity& entity_overlapping, CollisionMode collision_mode);
    virtual void notify_collision(
        MapEntity& other_entity, Sprite& this_sprite, Sprite& other_sprite);

  private:

    int collision_modes;                    /**< collision modes of the detector
                                             * (can be an OR combination of CollisionMode values) */
    bool layer_independent_collisions;      /**< indicates that the detector detects collisions on every layer */

};

}

#endif

