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
#ifndef ZSDX_SELECTION_MENU_CHOOSE_NAME_H
#define ZSDX_SELECTION_MENU_CHOOSE_NAME_H

#include "Common.h"
#include "menus/SelectionMenuChooseName.h"
#include "menus/SelectionMenuPhase.h"

/**
 * Phase of the selection menu where the user
 * chooses the name to create its savegame.
 */
class SelectionMenuChooseName: public SelectionMenuPhase {

 private:

  char player_name[11];
  int x_letter_cursor;
  int y_letter_cursor;

  SDL_Surface *img_arrow;
  SDL_Surface *img_letters;

  TextSurface *text_new_player_name;

  uint32_t next_key_date; /**< date when a key other than the arrows can be pressed again */

  // player name
  bool select_letter(void);
  bool validate_player_name(void);

 public:

  // creation and destruction
  SelectionMenuChooseName(SelectionMenuPhase *previous);
  ~SelectionMenuChooseName(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void display(SDL_Surface *screen_surface);
};

#endif
