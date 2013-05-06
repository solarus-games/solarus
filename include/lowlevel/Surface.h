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
#ifndef SOLARUS_SURFACE_H
#define SOLARUS_SURFACE_H

#include "Common.h"
#include "Drawable.h"
#include "lowlevel/Rectangle.h"
#include <SDL.h>

/**
 * @brief Represents a graphic surface.
 *
 * A surface is a rectangle of pixels.
 * A surface can be drawn or blitted on another surface.
 * This class basically encapsulates a library-dependent surface object.
 */
class Surface: public Drawable {

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

  public:

    Surface(int width = SOLARUS_SCREEN_WIDTH, int height = SOLARUS_SCREEN_HEIGHT);
    Surface(const Rectangle& size);
    Surface(const std::string& file_name, ImageDirectory base_directory = DIR_SPRITES);
    Surface(SDL_Surface* internal_surface);
    Surface(const Surface& other);
    ~Surface();

    int get_width() const;
    int get_height() const;
    const Rectangle get_size() const;

    Color get_transparency_color();
    void set_transparency_color(const Color& color);
    void set_opacity(int opacity);
    void set_clipping_rectangle(const Rectangle& clipping_rectangle = Rectangle());
    void fill_with_color(Color& color);
    void fill_with_color(Color& color, const Rectangle& where);

    void draw_region(const Rectangle& src_position, Surface& dst_surface);
    void draw_region(const Rectangle& src_position, Surface& dst_surface, const Rectangle& dst_position);

    const std::string& get_lua_type_name() const;

  protected:

    // implementation from Drawable
    void raw_draw(Surface& dst_surface, const Rectangle& dst_position);
    void raw_draw_region(const Rectangle& region,
        Surface& dst_surface, const Rectangle& dst_position);
    void draw_transition(Transition& transition);

  private:

    SDL_Surface* internal_surface;               /**< the SDL_Surface encapsulated */
    bool internal_surface_created;               /**< indicates that internal_surface was allocated from this class */

    uint32_t get_pixel32(int idx_pixel);
    SDL_Surface* get_internal_surface();
    uint32_t get_mapped_pixel(int idx_pixel, SDL_PixelFormat* dst_format);
};

#endif

