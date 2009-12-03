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
#ifndef ZSDX_TRANSITION_SCROLLING_H
#define ZSDX_TRANSITION_SCROLLING_H

#include "Common.h"
#include "Transition.h"

/**
 * Scrolling transition between two maps.
 */
class TransitionScrolling: public Transition {

 private:

  SDL_Surface *both_maps_surface;      /**< an intermediate surface where the two map surfaces will be blitted */
  int scrolling_direction;             /**< direction of the scrolling (0 to 3) */
  uint32_t next_scroll_date;             /**< date of the next scrolling step */

  int dx;                              /**< x change for each scrolling step */
  int dy;                              /**< y change for each scrolling step */

  static const SDL_Rect previous_map_dst_positions[];

  SDL_Rect previous_map_dst_position;  /**< position where the previous map is blitted on both_maps_surface */
  SDL_Rect current_map_dst_position;   /**< position where the current map is blitted on both_maps_surface */
  SDL_Rect current_scrolling_position; /**< the rectangle from both_maps_surface that is currently visible */

  void scroll(void);

 public:
  TransitionScrolling(Direction direction);
  ~TransitionScrolling(void);

  bool needs_previous_surface(void);

  void start(void);
  bool is_started(void);
  bool is_over(void);
  void display(SDL_Surface *surface);
};

#endif
