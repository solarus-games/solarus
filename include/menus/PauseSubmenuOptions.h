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
#ifndef PAUSE_SUBMENU_OPTIONS_H
#define PAUSE_SUBMENU_OPTIONS_H

#include "Common.h"
#include "menus/PauseSubmenu.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief The Options submenu of the pause menu.
 *
 * This submenu allows the player to change the video mode
 * and to customize his game keys.
 */
class PauseSubmenuOptions: public PauseSubmenu {

  private:

    GameControls *controls;

    std::string *video_mode_strings;
    std::string caption_strings[3];

    TextSurface *video_mode_text;

    TextSurface *controls_text;
    TextSurface *keyboard_text;
    TextSurface *joypad_text;

    TextSurface *game_key_texts[9];
    TextSurface *keyboard_control_texts[9];
    TextSurface *joypad_control_texts[9];

    Surface *controls_surface;
    int highest_visible_key; // 1 to 9
    int controls_visible_y;

    Sprite *up_arrow_sprite;
    Sprite *down_arrow_sprite;

    Sprite *cursor_sprite;
    Rectangle cursor_sprite_position;

    Sound *cursor_sound;
    Sound *ok_sound;
    int cursor_position; /**< 0: fullscreen, 1 to 9: key 1 to 9 */
    bool customizing;

    void load_control_texts(void);
    void set_cursor_position(int position);
    void action_key_pressed(void);

    void display_cursor(Surface *destination);

  public:

    PauseSubmenuOptions(PauseMenu *pause_menu, Game *game);
    ~PauseSubmenuOptions(void);

    void key_pressed(GameControls::GameKey key);
    void update(void);
    void display(Surface *destination);
};

#endif

