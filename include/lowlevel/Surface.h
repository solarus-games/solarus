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
#ifndef SOLARUS_SURFACE_H
#define SOLARUS_SURFACE_H

#include "Common.h"
#include <SDL/SDL.h>

/**
 * Represents a surface that can be drawn or blitted on another surface.
 * This class basically encapsulates a library-dependent surface object.
 */
class Surface {

  public:

    /**
     * Indicates the base directory to use when opening image files.
     */
    enum ImageDirectory {
      DIR_DATA,        /**< the root directory of the data package */
      DIR_SPRITES,     /**< the sprites subdirectory of the data package (default) */
      DIR_LANGUAGE     /**< the language-specific image directory of the data package, for the current language */
    };

  private:

    SDL_Surface *internal_surface;               /**< the SDL_Surface encapsulated */
    bool internal_surface_created;               /**< indicates that internal_surface was allocated from this class */

  public:

    Surface(int width, int height);
    Surface(const std::string &file_name, ImageDirectory base_directory = DIR_SPRITES);
    Surface(SDL_Surface *internal_surface);
    ~Surface(void);

    int get_width(void);
    int get_height(void);

    void set_transparency_color(Color &color);
    void set_opacity(int opacity);
    void fill_with_color(Color &color);
    void fill_with_color(Color &color, const Rectangle &where);
    void blit(Surface *destination);
    void blit(Surface *destination, const Rectangle &dst_position);
    void blit(const Rectangle &src_position, Surface *destination);
    void blit(const Rectangle &src_position, Surface *destination, const Rectangle &dst_position);

    SDL_Surface * get_internal_surface(void);
};

#endif

