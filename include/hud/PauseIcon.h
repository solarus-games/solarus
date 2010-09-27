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
#ifndef SOLARUS_PAUSE_ICON_H
#define SOLARUS_PAUSE_ICON_H

#include "Common.h"
#include "hud/HudElement.h"
#include "KeysEffect.h"

/**
 * @brief Displays the pause icon.
 *
 * This icon displays what happens when the player presses the pause key.
 * Usually, the icon indicates that the player can pause the pause
 * by pressing the key. When the game is paused,
 * the icon indicates that he can get back to the game by pressing
 * the same key.
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

    PauseIcon(Game &game, int x, int y);
    ~PauseIcon();

    void update();
    void rebuild();

};

#endif

