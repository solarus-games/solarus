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
#ifndef SOLARUS_PAUSE_ICON_H
#define SOLARUS_PAUSE_ICON_H

#include "Common.h"
#include "hud/HudElement.h"
#include "KeysEffect.h"

/**
 * This class displays the pause icon.
 */
class PauseIcon: public HudElement {

  private:

    // static icon
    KeysEffect::PauseKeyEffect pause_key_effect_displayed;
    Surface *img_pause_icon;

    // icon flipping
    bool is_flipping;
    Sprite *sprite_pause_icon;

  public:

    PauseIcon(Game *game, int x, int y);
    ~PauseIcon(void);

    void update(void);
    void rebuild(void);

};

#endif

