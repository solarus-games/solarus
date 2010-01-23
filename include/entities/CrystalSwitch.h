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
#ifndef SOLARUS_CRYSTAL_SWITCH_H
#define SOLARUS_CRYSTAL_SWITCH_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * A crystal switch is a switch that the hero can activate to change the configuration of the
 * blue and orange blocks on the map.
 */
class CrystalSwitch: public Detector {

  private:

    bool state;                      /**< false if the orange blocks are lowered,
				      * true if the blue blocks are lowered */
    uint32_t next_possible_hit_date; /**< date when the crystal switch can be hit again */
    Sprite *star_sprite;             /**< sprite of the star twinkling on the crystal switch */
    Rectangle star_xy;               /**< position of the star */

    void activate();
    void twinkle();

  public:

    CrystalSwitch(Layer layer, int x, int y);
    ~CrystalSwitch(void);
    static CreationFunction parse;

    EntityType get_type(void);

    bool is_obstacle_for(MapEntity *other);
    void notify_collision(MapEntity *entity_overlapping, CollisionMode collision_mode);
    void notify_collision(MapEntity *other_entity, Sprite *other_sprite, Sprite *this_sprite);
    void action_key_pressed(void);

    void update(void);
    void display_on_map(void);
    void set_suspended(bool suspended);
};

#endif

