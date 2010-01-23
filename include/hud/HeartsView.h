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
#ifndef SOLARUS_HEARTS_VIEW_H
#define SOLARUS_HEARTS_VIEW_H

#include "Common.h"
#include "hud/HudElement.h"

/**
 * This class handles the player's hearts displaying.
 */
class HeartsView: public HudElement {

  private:

    // rectangles in the image
    static const Rectangle full_heart_position;
    static const Rectangle fraction_heart_positions[3];

    Sprite *empty_heart_sprite;

    Surface *img_hearts;
    int nb_max_hearts_displayed;
    int nb_current_hearts_displayed;

    uint32_t next_heart_update_date;
    uint32_t next_danger_sound_date;

  public:

    HeartsView(Game *game, int x, int y);
    HeartsView(Equipment *equipment, int x, int y);
    ~HeartsView(void);
    void create(void);

    void update(void);
    void rebuild(void);
    bool is_visible(void);
};

#endif

