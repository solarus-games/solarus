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
#ifndef ZSDX_PAUSE_SUBMENU_H
#define ZSDX_PAUSE_SUBMENU_H

#include "Common.h"
#include "Controls.h"

/**
 * Abstract class for a submenu of the pause menu.
 */
class PauseSubmenu {

 protected:

  PauseMenu *pause_menu;
  Game *game;
  Savegame *savegame;
  Equipment *equipment;
  DungeonEquipment *dungeon_equipment;

  TextSurface *caption_text_1;
  TextSurface *caption_text_2;

  PauseSubmenu(PauseMenu *pause_menu, Game *game);

  void set_caption_text(const std::string &text);

 public:

  virtual ~PauseSubmenu(void);

  virtual void key_pressed(Controls::GameKey key) = 0;
  virtual void update(void) = 0;
  virtual void display(SDL_Surface *destination);
};

#endif
