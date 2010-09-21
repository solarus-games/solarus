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
 * @brief Abstract class for a transition effect between two phases.
 *
 * The transitions may be applied to maps or any surface.
 */
class Transition {

  public:

    /**
     * @brief Styles of transitions.
     */
    enum Style {
      IMMEDIATE = 0,  // no transition between the two surfaces
      FADE      = 1,  // fade in and fade out
      SCROLLING = 2   // scrolling between two maps
    };

    /**
     * @brief Possible directions of a transition.
     */
    enum Direction {
      IN  = 0,
      OUT = 1
    };

  protected:

    Game *game;                    /**< the current game if any (required by some kinds of transitions) */
    Direction direction;           /**< direction of the transition (in or out) */
    Surface *previous_surface;     /**< during an in transition, this is the surface that was displayed 
				    * when the out transition was played */

    Transition(Direction direction);

  public:

    virtual ~Transition();
    static Transition *create(Style style, Direction direction, Game *game = NULL);

    Direction get_direction();
    void set_previous_surface(Surface *previous_surface);
    virtual bool needs_previous_surface();

    /**
     * @brief Starts this transition effect.
     */
    virtual void start() = 0;

    /**
     * @brief Returns whether the transition effect is started.
     * @return true if the transition effect is started
     */
    virtual bool is_started() = 0;

    /**
     * @brief Returns whether the transition effect is finished.
     * @return true if the transition effect is finished
     */
    virtual bool is_finished() = 0;

    /**
     * @brief Updates this transition effect.
     */
    virtual void update() = 0;

    /**
     * @brief Displays the transition effect on a surface.
     * @param surface the surface to draw
     */
    virtual void display(Surface *surface) = 0;
};

#endif

