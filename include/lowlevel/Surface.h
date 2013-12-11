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
#ifndef SOLARUS_SURFACE_H
#define SOLARUS_SURFACE_H

#include "Common.h"
#include "Drawable.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/PixelBits.h"
#include <vector>
#include <map>

typedef struct SDL_Surface SDL_Surface;
typedef struct SDL_Texture SDL_Texture;

/**
 * \brief Represents a graphic surface.
 *
 * A surface is a rectangle of pixels.
 * A surface can be drawn or blitted on another surface.
 * This class basically encapsulates a library-dependent surface object.
 */
class Surface: public Drawable {

  // low-level classes allowed to manipulate directly the internal SDL surface encapsulated
  friend class TextSurface;
  friend class PixelBits;

  public:

    /**
     * \brief The base directory to use when opening image files.
     */
    enum ImageDirectory {
      DIR_DATA,        /**< the root directory of the data package */
      DIR_SPRITES,     /**< the sprites subdirectory of the data package (default) */
      DIR_LANGUAGE     /**< the language-specific image directory of the data package, for the current language */
    };

    ~Surface();

    // Constructors are private so that surfaces are only created on the heap.
    // This is because they use the refcount system internally for drawing.
    static Surface* create(int width, int height);
    static Surface* create(const Rectangle& size);
    static Surface* create(const std::string& file_name,
        ImageDirectory base_directory = DIR_SPRITES);

    int get_width() const;
    int get_height() const;
    const Rectangle get_size() const;

    bool is_software_destination() const;
    void set_software_destination(bool software_destination);

    void set_opacity(int opacity);
    void fill_with_color(Color& color);
    void fill_with_color(Color& color, const Rectangle& where);

    void render(SDL_Renderer* renderer);

    const std::string& get_lua_type_name() const;

  protected:

    // Implementation from Drawable.
    void raw_draw(
        Surface& dst_surface,
        const Rectangle& dst_position);
    void raw_draw_region(
        const Rectangle& region,
        Surface& dst_surface,
        const Rectangle& dst_position);
    void draw_transition(Transition& transition);
    Surface& get_transition_surface();

  private:

    class SubSurfaceNode;

    Surface(int width, int height);
    explicit Surface(SDL_Surface* internal_surface);

    uint32_t get_pixel(int index) const;
    bool is_pixel_transparent(int index) const;
  
    static SDL_Surface* get_surface_from_file(
        const std::string& file_name,
        ImageDirectory base_directory);
    static SDL_Texture* get_texture_from_surface(SDL_Surface* software_surface);

    void add_subsurface(Surface& src_surface, const Rectangle& region, const Rectangle& dst_position);
    void clear_subsurfaces();
    void render(
        SDL_Renderer* renderer,
        const Rectangle& src_rect,
        const Rectangle& dst_rect,
        const Rectangle& clip_rect,
        int opacity,
        const std::vector<SubSurfaceNode*>& subsurfaces
    );

    std::vector<SubSurfaceNode*> subsurfaces;      /**< Source Subsurfaces not in the tree yet */

    bool software_destination;            /**< indicates that this surface is modified on software side
                                           * (and therefore immediately) when used as a destination */
    SDL_Surface* internal_surface;        /**< the SDL_Surface encapsulated, if any. */
    SDL_Texture* internal_texture;        /**< the SDL_Texture encapsulated. */
    Color* internal_color;                /**< the background color to use, if any. */
    bool is_rendered;                     /**< indicated if the current surface has been rendered. Set to false when drawing a surface on this one. */
    int internal_opacity;                 /**< opacity to apply to all subtexture. */
    int width, height;                    /**< size of the texture, avoid to use SDL_QueryTexture. */
};

#endif

