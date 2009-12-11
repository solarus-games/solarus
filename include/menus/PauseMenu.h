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
#ifndef ZSDX_PAUSE_MENU_H
#define ZSDX_PAUSE_MENU_H

#include "Common.h"
#include "Controls.h"
#include "KeysEffect.h"

/**
 * The menus displayed when the game is paused.
 */
class PauseMenu {

  private:

    Game *game;
    Savegame *savegame;
    KeysEffect *keys_effect;

    Surface *backgrounds_surface;
    PauseSubmenu *current_submenu;

    Sprite *save_dialog_sprite;
    int save_dialog_state; // 0: not visible, 1: save dialog, 2: quit dialog
    int save_dialog_choice; // 0 or 1
    KeysEffect::ActionKeyEffect action_key_effect_saved;
    KeysEffect::SwordKeyEffect sword_key_effect_saved;

    TextSurface *question_text[2];
    TextSurface *answer_text[2];

    void set_current_submenu(int submenu_index);

  public:

    PauseMenu(Game *game);
    ~PauseMenu(void);
    void quit(void);

    void key_pressed(Controls::GameKey key);
    void update(void);
    void display(Surface *destination);

    void show_left_submenu(void);
    void show_right_submenu(void);
};

#endif

