/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
 * \brief An object that can be drawn on a surface with additional effects.
 *
 * Possible additional effects are movements and transitions.
 * Lua callbacks may be associated to the end of these effects.
 */
class Drawable: public ExportableToLua {

  public:

    virtual ~Drawable();

    // dynamic effects
    void start_movement(Movement& movement);
    void stop_movement();
    Movement* get_movement();
    const Rectangle& get_xy() const;
    void set_xy(const Rectangle& xy);

    void start_transition(Transition& transition, int callback_ref, LuaContext* lua_context);
    void stop_transition();
    Transition* get_transition();

    // drawing with effects
    void draw(Surface& dst_surface);
    void draw(Surface& dst_surface, int x, int y);
    void draw(Surface& dst_surface, const Rectangle& dst_position);
    void draw_region(const Rectangle& region, Surface& dst_surface);
    void draw_region(const Rectangle& region,
        Surface& dst_surface, const Rectangle& dst_position);

    /**
     * \brief Draws this object without applying dynamic effects.
     *
     * Redefine this function to draw your object onto the destination
     * surface.
     *
     * \param dst_surface The destination surface.
     * \param dst_position Coordinates on the destination surface.
     */
    virtual void raw_draw(Surface& dst_surface,
        const Rectangle& dst_position) = 0;

    /**
     * \brief Draws a subrectangle of this object without applying dynamic
     * effects.
     *
     * Redefine this function to draw your object onto the destination
     * surface.
     *
     * \param region The subrectangle to draw in this object.
     * \param dst_surface The destination surface.
     * \param dst_position Coordinates on the destination surface.
     */
    virtual void raw_draw_region(
        const Rectangle& region,
        Surface& dst_surface,
        const Rectangle& dst_position) = 0;

    /**
     * \brief Draws a transition effect on this drawable object.
     *
     * Redefine this function to apply the transition effect on the surface
     * of your object.
     *
     * \param transition The transition effect to apply.
     *
     * TODO Since there is get_transition_surface() now, this function can probably be removed.
     */
    virtual void draw_transition(Transition& transition) = 0;

    /**
     * \brief Returns the surface where transitions on this drawable object
     * are applied.
     * \return The surface for transitions.
     */
    virtual Surface& get_transition_surface() = 0;
    virtual void update();

  protected:

    Drawable();

  private:

    Rectangle xy;                 /**< Current position of this object
                                   * (result of movements). */
    Movement* movement;           /**< A movement applied, or NULL (will be
                                   * deleted then if unused elsewhere). */
    Transition* transition;       /**< A transition applied, or NULL
                                   * (will be deleted then). */
    int transition_callback_ref;  /**< Lua registry ref of a function to call
                                   * when the transition finishes */
    LuaContext* lua_context;      /**< The Lua world used for callbacks. */
};

#endif

