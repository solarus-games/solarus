/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_EXPLOSION_H
#define ZSDX_EXPLOSION_H

#include "Common.h"
#include "entities/Detector.h"

/**
 * Represents an explosion on the map.
 * An explosion can be caused by several entities such as bombs, bomb flowers and enemies.
 * An explosion can hurt the hero, the enemies and open weak walls.
 */
class Explosion: public Detector {

  public:

    Explosion(Layer layer, const SDL_Rect &xy);
    ~Explosion(void);

    EntityType get_type(void);

    // features
    bool can_be_obstacle(void);
    bool can_detect_entities(void);
    bool can_be_displayed(void);
    bool is_displayed_in_y_order(void);

    // state
    void update(void);
};

#endif

