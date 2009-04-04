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
#ifndef ZSDX_SELECTION_MENU_COMMON_H
#define ZSDX_SELECTION_MENU_COMMON_H

#include "Common.h"

/**
 * This class handles the elements common to
 * all phases of the selection menu.
 * There elements are not changed between two
 * phases of the selection menu.
 */
class SelectionMenuCommon {

  friend class SelectionMenuPhase;

 private:

  /**
   * The savegames shown in the menu.
   */
  Savegame *savegames[3];

  // images
  SDL_Surface *img_cloud, *img_background;
  SDL_Surface *img_save_container, *img_option_container;
  SDL_Surface *img_numbers[3];

  Sprite *cursor;
  int cursor_position; // 1 to 5

  Uint32 background_color;

  // sounds
  Sound *cursor_sound;
  Sound *ok_sound;
  Sound *error_sound;

  // music
  Music *music;

  // icons
  KeysEffect *keys_effect;
  ActionIcon *action_icon;
  SwordIcon *sword_icon;

  // text
  TextSurface *text_option1;
  TextSurface *text_option2;
  TextSurface *text_title;

  // savegames data
  TextSurface *text_player_names[3];
  HeartsView *hearts_views[3];

  // clouds data
  SDL_Rect cloud_positions[16];
  Uint32 next_cloud_move;
  
  // initialization functions
  void initialize_clouds(void);
  void read_savegames(void);

 public:

  // creation and destruction
  SelectionMenuCommon(void);
  ~SelectionMenuCommon(void);

  // update and display
  void update(void);
  void display(SDL_Surface *destination_surface);
};

#endif
