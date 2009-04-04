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
#ifndef ZSDX_SELECTION_MENU_CHOOSE_MODE_H
#define ZSDX_SELECTION_MENU_CHOOSE_MODE_H

#include "Common.h"
#include "menus/SelectionMenuChooseMode.h"
#include "menus/SelectionMenuPhase.h"

/**
 * Phase of the selection menu where the user
 * chooses the game mode.
 */
class SelectionMenuChooseMode: public SelectionMenuPhase {

 private:

  /**
   * True if the Adventure mode if selected,
   * false if the Solarus Dreams mode is selected.
   */
  bool adventure_mode;

  SDL_Surface *img_mode;
  SDL_Surface *savegame_surface;
  
 public:

  // creation and destruction
  SelectionMenuChooseMode(SelectionMenuPhase *previous);
  ~SelectionMenuChooseMode(void);

  // update and display
  void handle_event(const SDL_Event &event);
  void update(void);
  void display(SDL_Surface *screen_surface);
};

#endif
