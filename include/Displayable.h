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
#ifndef SOLARUS_DISPLAYABLE_H
#define SOLARUS_DISPLAYABLE_H

#include "Common.h"

/**
 * @brief Represents anything that can be drawn on a surface.
 */
class Displayable {

  public:

    virtual ~Displayable();

    void display(Surface& dst_surface);
    void display(Surface& dst_surface, int x, int y);
    void display(Surface& dst_surface, const Rectangle& dst_position);

  protected:

    Displayable();

    /**
     * @brief Displays this object on a surface at 0,0.
     * @param dst_surface the destination surface
     * @param x x coordinate of where to display
     * @param y y coordinate of where to display
     */
    virtual void blit_0(Surface& dst_surface);

    /**
     * @brief Displays this object on a surface.
     * @param dst_surface the destination surface
     * @param x x coordinate of where to display
     * @param y y coordinate of where to display
     */
    virtual void blit_xy(Surface& dst_surface, int x, int y) = 0;

    virtual void update();
};

#endif

