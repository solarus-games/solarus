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
#ifndef ZSDX_SURFACE_H
#define ZSDX_SURFACE_H

#include "Common.h"
#include <SDL/SDL.h>

/**
 * Represents a surface that can be drawn or blitted on another surface.
 * This class basically encapsulates a library-dependent surface object.
 */
class Surface {

  private:

    SDL_Surface *surface; /**< the SDL_Surface encapsulated */

  public:

    Surface(int width, int height);
    ~Surface(void);

    void fill_with_color(Color color);
    void blit(Surface *destination);
    void blit(const Rectangle & src_position, Surface *dst, const Rectangle & dst_position);

};

#endif

