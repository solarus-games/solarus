/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#ifndef SOLARUS_DYNAMIC_DISPLAYABLE_H
#define SOLARUS_DYNAMIC_DISPLAYABLE_H

#include "Common.h"
#include "lowlevel/Rectangle.h"
#include <lua.hpp>

/**
 * @brief An object that can be drawn on a surface with additional effects.
 *
 * Possible additional effects are movements and transitions.
 * Lua callbacks may be associated to the end of these effects.
 */
class DynamicDisplayable {

  public:

    virtual ~DynamicDisplayable();

    Displayable& get_displayable();

    void start_movement(Movement& movement, int callback_ref = LUA_REFNIL);
    void stop_movement();
    void set_movement_callback(int movement_callback_ref);

    void start_transition(Transition& transition, int callback_ref = LUA_REFNIL);
    void stop_transition();
    void set_transition_callback(int transition_callback_ref);

    void update();
    void display(Surface& dst_surface, Rectangle dst_position);

  protected:

    DynamicDisplayable(Displayable& displayable, Script* script = NULL);

  private:

    Displayable* displayable;     /**< the displayable object (will be deleted) */
    Rectangle last_position;      /**< position of this object after its last
                                   * movement */
    Movement* movement;           /**< a movement applied, or NULL (will be
                                   * deleted then if unused elsewhere) */
    int movement_callback_ref;    /**< Lua registry ref of a function to call
                                   * when the movement finishes */
    Transition* transition;       /**< a transition applied, or NULL
                                   * (will be deleted then) */
    int transition_callback_ref;  /**< Lua registry ref of a function to call
                                   * when the transition finishes */
    Script* script;               /**< the script that owns this object,
                                   * or NULL */
};

#endif

