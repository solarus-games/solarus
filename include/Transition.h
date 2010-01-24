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
#ifndef SOLARUS_TRANSITION_H
#define SOLARUS_TRANSITION_H

#include "Common.h"

/**
 * Abstract class for a transition effect between two phases (maps or not).
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

    Game *game;                    /**< the current game if any (used by some kinds of transitions) */
    Direction direction;           /**< direction of the transition (in or out) */
    Surface *previous_surface;     /**< during an in transition, this is the surface that was displayed 
				    * when the out transition was played */

    Transition(Direction direction);

  public:

    virtual ~Transition(void);
    static Transition *create(Style style, Direction direction, Game *game = NULL);

    Direction get_direction(void);
    void set_previous_surface(Surface *previous_surface);
    virtual bool needs_previous_surface(void);

    virtual void start(void) = 0;
    virtual bool is_started(void) = 0;
    virtual bool is_finished(void) = 0;
    virtual void update(void) = 0;
    virtual void display(Surface *surface) = 0;
};

#endif

