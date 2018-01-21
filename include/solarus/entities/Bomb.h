/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/entities/Entity.h"
#include <string>

namespace Solarus {

/**
 * \brief A bomb that will explode soon.
 */
class Bomb: public Entity {

  public:

    static constexpr EntityType ThisType = EntityType::BOMB;

    Bomb(const std::string& name, int layer, const Point& xy);

    EntityType get_type() const override;

    bool is_stream_obstacle(Stream& stream) override;
    bool is_teletransporter_obstacle(Teletransporter& teletransporter) override;
    bool is_deep_water_obstacle() const override;
    bool is_hole_obstacle() const override;
    bool is_lava_obstacle() const override;
    bool is_prickle_obstacle() const override;
    bool is_ladder_obstacle() const override;
    void notify_collision(Entity& entity_overlapping, CollisionMode collision_mode) override;
    void notify_collision_with_explosion(Explosion& explosion, Sprite& sprite_overlapping) override;
    void notify_collision_with_stream(Stream& stream, int dx, int dy) override;
    void notify_position_changed() override;
    bool notify_action_command_pressed() override;
    void set_suspended(bool suspended) override;
    void update() override;

    void explode();

  private:

    uint32_t explosion_date;  /**< Date when the bomb explodes. */

};

}

#endif

