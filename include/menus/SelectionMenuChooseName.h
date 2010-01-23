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
#ifndef SOLARUS_SELECTION_MENU_CHOOSE_NAME_H
#define SOLARUS_SELECTION_MENU_CHOOSE_NAME_H

#include "Common.h"
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuPhase.h"
#include "menus/SelectionMenu.h"

/**
 * Phase of the selection menu where the user
 * chooses the name to create its savegame.
 */
class SelectionMenuChooseName: public SelectionMenuPhase {

  private:

    char player_name[11];                /**< the name the player is currently typing */
    int letter_cursor_x;                 /**< x position of the letter cursor */
    int letter_cursor_y;                 /**< y position of the letter cursor */

    Surface *arrow_img;                  /**< arrow image shown next to the name */
    Surface *letters_img;                /**< image of all letters */

    TextSurface *player_name_text;       /**< text surface representing the current name */

    uint32_t next_key_date;              /**< date when a key other than the arrows can be pressed again */

    // player name
    bool select_letter(void);
    bool validate_player_name(void);

  public:

    // creation and destruction
    SelectionMenuChooseName(SelectionMenu *menu);
    ~SelectionMenuChooseName(void);

    // update and display
    void display(Surface *destination_surface);
    void handle_event(const SDL_Event &event);
};

#endif

