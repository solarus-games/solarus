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
#ifndef SOLARUS_SURFACE_H
#define SOLARUS_SURFACE_H

#include "Common.h"
#include "lowlevel/Rectangle.h"
#include <SDL.h>

/**
 * @brief Represents a graphic surface.
 *
 * A surface is a rectangle of pixels.
 * A surface can be drawn or blitted on another surface.
 * This class basically encapsulates a library-dependent surface object.
 */
class Surface {

  // low-level classes allowed to manipulate directly the internal SDL surface encapsulated
  friend class TextSurface;
  friend class VideoManager;
  friend class PixelBits;

  public:

    /**
     * @brief The base directory to use when opening image files.
     */
    enum ImageDirectory {
      DIR_DATA,        /**< the root directory of the data package */
      DIR_SPRITES,     /**< the sprites subdirectory of the data package (default) */
      DIR_LANGUAGE     /**< the language-specific image directory of the data package, for the current language */
    };

  private:

    SDL_Surface* internal_surface;               /**< the SDL_Surface encapsulated */
    bool internal_surface_created;               /**< indicates that internal_surface was allocated from this class */

    SDL_Surface* get_internal_surface();

  public:

    Surface(int width, int height);
    Surface(const std::string& file_name, ImageDirectory base_directory = DIR_SPRITES);
    Surface(SDL_Surface* internal_surface);
    Surface(const Surface& other);
    ~Surface();

    int get_width();
    int get_height();
    const Rectangle get_size();

    void set_transparency_color(Color& color);
    void set_opacity(int opacity);
    void set_clipping_rectangle(const Rectangle& clipping_rectangle = Rectangle());
    void fill_with_color(Color& color);
    void fill_with_color(Color& color, const Rectangle& where);
    void blit(Surface* destination);
    void blit(Surface* destination, const Rectangle& dst_position);
    void blit(const Rectangle& src_position, Surface* destination);
    void blit(const Rectangle& src_position, Surface* destination, const Rectangle& dst_position);
};

#endif

