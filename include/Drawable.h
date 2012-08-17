/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "lua/ExportableToLua.h"
#include "lowlevel/Rectangle.h"
#include <lua.hpp>

/**
 * @brief An object that can be drawn on a surface with additional effects.
 *
 * Possible additional effects are movements and transitions.
 * Lua callbacks may be associated to the end of these effects.
 */
class Drawable: public ExportableToLua {

  public:

    virtual ~Drawable();

    // dynamic effects
    void start_movement(Movement& movement, int callback_ref = LUA_REFNIL,
        LuaContext* lua_context = NULL);
    void stop_movement();

    void start_transition(Transition& transition, int callback_ref = LUA_REFNIL,
        LuaContext* lua_context = NULL);
    void stop_transition();

    // displaying with effects
    void display(Surface& dst_surface);
    void display(Surface& dst_surface, int x, int y);
    void display(Surface& dst_surface, Rectangle dst_position);

    /**
     * @brief Displays this object without applying dynamic effects.
     *
     * Redefine this function to draw your object onto the destination
     * surface.
     *
     * @param dst_surface the destination surface
     * @param dst_position coordinates on the destination surface
     */
    virtual void raw_display(Surface& dst_surface,
        const Rectangle& dst_position) = 0;

    /**
     * @brief Draws a transition effect on this drawable object.
     *
     * Redefine this function to apply the transition effect on the surface
     * of your object.
     *
     * @param transition The transition effect to apply.
     */
    virtual void display_transition(Transition& transition) = 0;
    virtual void update();

  protected:

    Drawable();

  private:

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
    LuaContext* lua_context;      /**< The Lua world used for callbacks. */
};

#endif

