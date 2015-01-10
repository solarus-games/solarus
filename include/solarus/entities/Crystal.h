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
#ifndef SOLARUS_CRYSTAL_H
#define SOLARUS_CRYSTAL_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/entities/Detector.h"
#include <list>

namespace Solarus {

/**
 * \brief A switch that can be activated to change the state of the
 * crystal blocks.
 */
class Crystal: public Detector {

  public:

    Crystal(const std::string& name, Layer layer, const Point& xy);

    virtual EntityType get_type() const override;

    virtual void notify_creating() override;
    virtual bool is_obstacle_for(MapEntity& other) override;
    virtual void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) override;
    virtual void notify_collision(MapEntity& other_entity, Sprite& this_sprite, Sprite& other_sprite) override;
    virtual bool notify_action_command_pressed() override;
    void activate(MapEntity& entity_activating);

    virtual void update() override;
    virtual void draw_on_map() override;
    virtual void set_suspended(bool suspended) override;

  private:

    void twinkle();

    bool state;                                    /**< false if the orange blocks are lowered,
                                                    * true if the blue blocks are lowered */
    uint32_t next_possible_hit_date;               /**< date when the crystal can be hit again */
    std::list<MapEntity*> entities_activating;     /**< list of entities that recently activated this crystal */
    SpritePtr star_sprite;           /**< sprite of the star twinkling on the crystal */
    Point star_xy;                                 /**< position of the star */

};

}

#endif

