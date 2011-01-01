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
#ifndef SOLARUS_BOMB_H
#define SOLARUS_BOMB_H

#include "Common.h"
#include "entities/MapEntity.h"

/**
 * @brief A bomb that will explode soon.
 */
class Bomb: public MapEntity {

  private:

    uint32_t explosion_date;		/**< date when the bomb explodes */

  public:

    Bomb(Layer layer, int x, int y);
    ~Bomb();

    EntityType get_type();

    bool can_be_obstacle();
    bool can_detect_entities();
    bool can_be_displayed();
    bool is_displayed_in_y_order();
    void notify_collision_with_explosion(Explosion &explosion, Sprite &sprite_overlapping);
    void set_suspended(bool suspended);
    void update();

    void explode();
};

#endif

