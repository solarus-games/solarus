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
#ifndef SOLARUS_JUMPER_H
#define SOLARUS_JUMPER_H

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"

namespace Solarus {

/**
 * \brief A sensor that makes the hero jump.
 *
 * A jumper is a detector placed on a map.
 * When the hero touches this detector, he makes a jump with a specified length.
 */
class Jumper: public Detector {

  public:

    Jumper(
        const std::string& name,
        Layer layer,
        const Point& xy,
        const Size& size,
        int direction,
        int jump_length
    );

    virtual EntityType get_type() const override;
    virtual bool can_be_drawn() const override;
    virtual bool is_sword_ignored() const override;
    virtual bool is_obstacle_for(MapEntity& other, const Rectangle& candidate_position) override;
    virtual bool test_collision_custom(MapEntity& entity) override;
    virtual void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) override;

    int get_jump_length() const;
    bool is_jump_horizontal() const;
    bool is_jump_vertical() const;
    bool is_jump_diagonal() const;
    bool is_in_jump_position(
        const Hero& hero,
        const Rectangle& candidate_position,
        bool extended_region
    ) const;

    bool overlaps_jumping_region(
        const Rectangle& rectangle,
        bool extended_region
    ) const;

  private:

    bool is_point_in_diagonal(const Point& point) const;
    bool is_point_in_extended_diagonal(const Point& point) const;

    int jump_length; /**< length of the jump in pixels (should be a multiple of 8) */

};

}

#endif

