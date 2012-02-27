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
#include "DynamicTextSurface.h"
#include "lowlevel/TextSurface.h"

/**
 * @brief Creates a dynamic text surface.
 * @param basic_text_surface the basic text surface to encapsulate
 * @param script the owner script of this object, or NULL
 */
DynamicTextSurface::DynamicTextSurface(TextSurface& basic_text_surface, Script* script):
  DynamicDisplayable(basic_text_surface, script) {

}

/**
 * @brief Destructor.
 */
DynamicTextSurface::~DynamicTextSurface() {

}

/**
 * @brief Returns the text surface encapsulated.
 */
TextSurface& DynamicTextSurface::get_basic_text_surface() {

  return (TextSurface&) get_basic_displayable();
}

