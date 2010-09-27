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
#ifndef SOLARUS_SWORD_ICON_H
#define SOLARUS_SWORD_ICON_H

#include "Common.h"
#include "hud/HudElement.h"
#include "KeysEffect.h"

/**
 * @brief Displays the sword icon.
 *
 * The sword icon displays what happens when the sword key is pressed.
 * In other words, it displays some information from the KeysEffect class.
 * Most of the time, a sword image is displayed.
 * When the game is paused, the sword icon changes and indicates that
 * the player can save the game by pressing the sword key.
 */
class SwordIcon: public HudElement {

  private:

    // static icon
    KeysEffect::SwordKeyEffect sword_key_effect_displayed;
    int sword_number_displayed;
    Surface *img_sword_icon;

    // icon flipping
    bool is_flipping;
    Sprite *sprite_sword_icon;

  public:

    SwordIcon(Game &game, int x, int y);
    ~SwordIcon();

    bool is_visible();
    void update();
    void rebuild();

};

#endif

