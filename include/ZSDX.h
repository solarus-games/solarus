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
#ifndef ZSDX_ZSDX_H
#define ZSDX_ZSDX_H

#include "Common.h"
#include "VideoManager.h"

/**
 * Number of times the screen in redrawn is a second.
 */
extern const int FRAMES_PER_SECOND;

/**
 * Delay between two frames in millisecond
 * FRAME_DELAY = 1000 / FRAMES_PER_SECOND
 */
extern const int FRAME_INTERVAL;

/**
 * Only instance.
 */
extern ZSDX *zsdx;

/**
 * Main class of the game engine. It contains objects to
 * initialize the game and handles the screen.
 */
class ZSDX {

 private:
 
  /**
   * The current screen displayed: the title screen, the selection menu, the game, etc.
   */
  Screen *current_screen;
  SDL_Surface *root_surface;  /**< the surface where everything is drawn (always 320*240) */
  VideoManager *video_manager;
  bool exiting;

  void launch_adventure_mode(Savegame *savegame);
  void launch_solarus_dreams_mode(Savegame *savegame);

  void handle_event(const SDL_Event &event);
  void display(void);

  bool is_exiting(void);

 public:

  TTF_Font *font;
  Game *game;

  ZSDX(void);
  ~ZSDX(void);

  void main(void);

  VideoManager *get_video_manager(void);

  void set_game(Game *game);
  void set_exiting(void);

};

#endif
