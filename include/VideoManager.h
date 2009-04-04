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
#ifndef ZSDX_VIDEO_MANAGER_H
#define ZSDX_VIDEO_MANAGER_H

#include "Common.h"
 
/**
 * Displays the window and handles the resolution mode.
 */
class VideoManager {

 public:

  /**
   * The different possible video modes.
   */
  enum VideoMode {
    WINDOWED_STRETCHED,       /**< the 320*240 game surface is stretched into a 640*480 window (default) */
    WINDOWED_SCALE2X,         /**< the 320*240 game surface is scaled into a 640*480 window with the Scale2x algorithm */
    WINDOWED_NORMAL,          /**< the 320*240 game surface is directly displayed on a 320*240 window */
    FULLSCREEN_NORMAL,        /**< the 320*240 game surface is directly displayed on a 320*240 screen */
    FULLSCREEN_WIDE,          /**< the 320*240 game surface is stretched into a 640*480 surface
			       * and then displayed on a widescreen resolution if possible (768*480 or 720*480)
			       * with two black side bars */
    FULLSCREEN_SCALE2X,       /**< the 320*240 game surface is scaled into a 640*480 screen with the Scale2x algorithm */
    FULLSCREEN_SCALE2X_WIDE,  /**< the 320*240 game surface is scaled into a 640*480 surface with the Scale2x algorithm
			       * and then displayed on a widescreen resolution if possible (768*480 or 720*480)
			       * with two black side bars */
    FULLSCREEN_CENTERED,      /**< the 320*240 game surface is displayed as a box inside a 640*480 screen */
    FULLSCREEN_CENTERED_WIDE, /**< the 320*240 game surface is displayed as a box inside a 640*400 screen */
    NB_MODES
  };

 private:

  static SDL_Rect video_mode_sizes[NB_MODES]; /**< size of the surface for each video mode */
  SDL_Rect dst_position_wide;                 /**< position of the 640*480 surface on the 768*480 or 720*480
					       * video surface */

  VideoMode video_mode;                       /**< current video mode of the screen */
  SDL_Surface *screen_surface;                /**< the screen surface */

  SDL_Rect dst_position_centered;             /**< position of the 320*240 game surface on the screen surface */
  int width;                                  /**< width of the current screen surface */
  int offset;                                 /**< width of a side bar when using a widescreen resolution */
  int end_row_increment;                      /**< increment used by the stretching and scaling functions 
					       * when changing the row */

  bool is_mode_supported(VideoMode mode);
  bool is_fullscreen(VideoMode mode);

  void blit(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void blit_stretched(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void blit_scale2x(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void blit_centered(SDL_Surface *src_surface, SDL_Surface *dst_surface);

 public:

  VideoManager(void);
  ~VideoManager(void);

  void switch_video_mode(void);
  void set_default_video_mode(void);
  void set_video_mode(VideoMode mode);
  VideoMode get_video_mode(void);

  void display(SDL_Surface *src_surface);
};

#endif
