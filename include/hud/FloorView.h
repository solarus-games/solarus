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
#ifndef SOLARUS_FLOOR_VIEW_H
#define SOLARUS_FLOOR_VIEW_H

#include "Common.h"
#include "hud/HudElement.h"

/**
 * Displays the floor of the current map.
 */
class FloorView: public HudElement {

  private:

    Map *current_map;
    Surface *img_floors;

    bool is_floor_displayed;
    uint32_t hide_floor_date;

  public:

    FloorView(Game *game, int x, int y);
    ~FloorView(void);

    void update(void);
    void rebuild(void);
};

#endif

