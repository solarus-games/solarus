/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_CAMERA_H
#define SOLARUS_CAMERA_H

#include "solarus/Common.h"
#include "solarus/entities/Entity.h"
#include "solarus/entities/EntityPtr.h"
#include "solarus/lowlevel/Rectangle.h"
#include <memory>

namespace Solarus {

class Map;
class Separator;
class TargetMovement;

/**
 * \brief Manages the visible area of the map.
 *
 * The camera determines the visible area of the map.
 * The camera may either be tracking an entity
 * (usually the hero)
 * or be controlled manually by a script.
 */
class Camera : public Entity {

  public:

    static constexpr EntityType ThisType = EntityType::CAMERA;

    explicit Camera(Map& map);

    EntityType get_type() const override;

    bool can_be_drawn() const override;
    void set_suspended(bool suspended) override;

    Point get_position_on_screen() const;
    void set_position_on_screen(const Point& position_on_screen);
    Point get_position_to_track(const Point& tracked_xy) const;

    void start_tracking(const EntityPtr& entity);
    void start_manual();

    EntityPtr get_tracked_entity() const;
    void notify_tracked_entity_traversing_separator(Separator& separator);
    bool is_traversing_separator() const;

    Rectangle apply_map_bounds(const Rectangle& area) const;
    Rectangle apply_separators(const Rectangle& area) const;
    Rectangle apply_separators_and_map_bounds(const Rectangle& area) const;

private:

    Point position_on_screen;               /**< Where to draw this camera on the screen. */

};

}

#endif

