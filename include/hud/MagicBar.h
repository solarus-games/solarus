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
#ifndef SOLARUS_MAGIC_BAR_H
#define SOLARUS_MAGIC_BAR_H

#include "Common.h"
#include "hud/HudElement.h"

/**
 * This class displays the magic bar on the screen.
 */
class MagicBar: public HudElement {

  private:

    // static image representing the current level of the magic bar
    int current_magic_displayed;
    uint32_t next_magic_update_date;
    bool is_magic_decreasing;
    Surface *img_magic_bar;
    Sound *sound_magic_bar;

    // sprite representing the magic bar container
    int max_magic_displayed;
    Sprite *sprite_magic_bar_container;

  public:

    MagicBar(Game *game, int x, int y);
    ~MagicBar(void);

    void update(void);
    void rebuild(void);
    bool is_visible(void);  
};

#endif

