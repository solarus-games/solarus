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
#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

/**
 * A transition effect between two screens (maps or not).
 */
class Transition {

 public:

  /**
   * Style of transition between two screens.
   */
  enum Style {
    IMMEDIATE = 0,  // no transition between the two screens
    FADE      = 1,  // fade in and fade out
    SCROLLING = 2,  // scrolling between two maps
  };

  /**
   * Direction of the transition.
   */
  enum Direction {
    IN  = 0,
    OUT = 1,
  };

 protected:

  Direction direction;           /**< direction of the transition (in or out) */
  SDL_Surface *previous_surface; /**< during an in transition, this is the surface that was displayed 
				  * when the out transition was played */

  Transition(Direction direction);

 public:
  virtual ~Transition(void);
  static Transition *create(Style style, Direction direction);

  Direction get_direction(void);
  void set_previous_surface(SDL_Surface *previous_surface);
  virtual bool needs_previous_surface(void);

  virtual void start(void) = 0;
  virtual bool is_started(void) = 0;
  virtual bool is_over(void) = 0;
  virtual void display(SDL_Surface *surface) = 0;
};

#endif

