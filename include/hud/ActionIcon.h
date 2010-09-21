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
#ifndef SOLARUS_ACTION_ICON_H
#define SOLARUS_ACTION_ICON_H

#include "Common.h"
#include "hud/HudElement.h"
#include "KeysEffect.h"

/**
 * @brief Displays the action icon.
 *
 * The action icon displays what can be done by pressing the action key.
 * In other words, this class displays some information from the KeysEffect class.
 */
class ActionIcon: public HudElement {

  private:

    // static icon
    KeysEffect::ActionKeyEffect action_key_effect_displayed;
    Surface *img_action_icon;

    // icon flipping
    bool is_flipping;
    Sprite *sprite_action_icon;

    bool is_visible();

  public:

    ActionIcon(Game *game, int x, int y);
    ~ActionIcon();

    void update();
    void rebuild();

};

#endif

