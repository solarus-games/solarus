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
#include "Displayable.h"
#include "lowlevel/Rectangle.h"

/**
 * @brief Constructor.
 */
Displayable::Displayable() {

}

/**
 * @brief Destructor.
 */
Displayable::~Displayable() {

}

/**
 * @brief Displays this object on a surface at position 0,0.
 * @param dst_surface the destination surface
 */
void Displayable::display(Surface& dst_surface) {

  blit_0(dst_surface);
}

/**
 * @brief Displays this object on a surface.
 * @param dst_surface the destination surface
 * @param x x coordinate of where to display
 * @param y y coordinate of where to display
 */
void Displayable::display(Surface& dst_surface, int x, int y) {

  blit_xy(dst_surface, x, y);
}

/**
 * @brief Displays this object on a surface.
 * @param dst_surface the destination surface
 * @param dst_position position on this surface
 */
void Displayable::display(Surface& dst_surface, const Rectangle& dst_position) {

  blit_xy(dst_surface, dst_position.get_x(), dst_position.get_y());
}

/**
 * @brief Displays this object on a surface at position 0,0.
 *
 * You may redefine this function to do something faster than calling
 * blit_xy with 0,0.
 *
 * @param dst_surface the destination surface
 */
void Displayable::blit_0(Surface& dst_surface) {

  blit_xy(dst_surface, 0, 0);
}

/**
 * @brief Updates this object.
 *
 * This function is called repeatedly. You can redefine it for your needs.
 */
void Displayable::update() {
  // nothing to do by default
}
