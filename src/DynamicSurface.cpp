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
#include "DynamicSurface.h"
#include "lowlevel/Surface.h"

/**
 * @brief Creates a dynamic surface.
 * @param basic_surface the basic surface to encapsulate
 * @param script the owner script of this object, or NULL
 */
DynamicSurface::DynamicSurface(Surface& basic_surface, Script* script):
  DynamicDisplayable(basic_surface, script) {

}

/**
 * @brief Destructor.
 */
DynamicSurface::~DynamicSurface() {

}

/**
 * @brief Returns the surface encapsulated.
 */
Surface& DynamicSurface::get_basic_surface() {

  return (Surface&) get_basic_displayable();
}

