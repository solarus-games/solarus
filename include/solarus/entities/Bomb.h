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
#ifndef SOLARUS_BOMB_H
#define SOLARUS_BOMB_H

#include "solarus/Common.h"
#include "solarus/entities/Detector.h"

namespace Solarus {

/**
 * \brief A bomb that will explode soon.
 */
class Bomb: public Detector {

  public:

    Bomb(const std::string& name, Layer layer, const Point& xy);

    virtual EntityType get_type() const override;

    virtual bool can_be_obstacle() const override;
    virtual bool is_stream_obstacle(Stream& stream) override;
    virtual bool is_teletransporter_obstacle(Teletransporter& teletransporter) override;
    virtual bool is_deep_water_obstacle() const override;
    virtual bool is_hole_obstacle() const override;
    virtual bool is_lava_obstacle() const override;
    virtual bool is_prickle_obstacle() const override;
    virtual bool is_ladder_obstacle() const override;
    virtual void notify_collision(MapEntity& entity_overlapping, CollisionMode collision_mode) override;
    virtual void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping) override;
    virtual void notify_collision_with_stream(Stream& stream, int dx, int dy) override;
    virtual void notify_position_changed() override;
    virtual bool notify_action_command_pressed() override;
    virtual void set_suspended(bool suspended) override;
    virtual void update() override;

    void explode();

  private:

    uint32_t explosion_date;  /**< date when the bomb explodes */

};

}

#endif

