/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/Common.h"
#include "solarus/lowlevel/PixelBits.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include "solarus/Drawable.h"
#include <SDL.h>
#include <SDL_image.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Solarus {

class Color;
class Size;
class SoftwarePixelFilter;
class Surface;

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

    Surface(int width, int height);
    explicit Surface(SDL_Surface* internal_surface);

    // Surfaces should only created with std::make_shared.
    // This is what create() functions do, so you should call them rather than
    // constructors.
    // This is because they are always reference-counted with shared_ptr
    // internally for drawing.
    static SurfacePtr create(int width, int height);
    static SurfacePtr create(const Size& size);
    static SurfacePtr create(const std::string& file_name,
        ImageDirectory base_directory = DIR_SPRITES);

    int get_width() const;
    int get_height() const;
    virtual Size get_size() const override;

    bool is_software_destination() const;
    void set_software_destination(bool software_destination);

    void clear();
    void clear(const Rectangle& where);
    void fill_with_color(const Color& color);
    void fill_with_color(const Color& color, const Rectangle& where);
    uint8_t get_opacity() const;
    void set_opacity(uint8_t opacity);

    std::string get_pixels() const;

    void apply_pixel_filter(const SoftwarePixelFilter& pixel_filter, Surface& dst_surface);

    void render(SDL_Renderer* renderer);

    virtual const std::string& get_lua_type_name() const override;

  protected:

    // Implementation from Drawable.
    virtual void raw_draw(
        Surface& dst_surface,
        const Point& dst_position
    ) override;
    virtual void raw_draw_region(
        const Rectangle& region,
        Surface& dst_surface,
        const Point& dst_position
    ) override;
    virtual void draw_transition(Transition& transition) override;
    virtual Surface& get_transition_surface() override;

  private:

    class SubSurfaceNode;
    using SubSurfaceNodePtr = std::shared_ptr<Surface::SubSurfaceNode>;

    struct SDL_Surface_Deleter {
        void operator()(SDL_Surface* sdl_surface) {
          SDL_FreeSurface(sdl_surface);
        }
    };
    using SDL_Surface_UniquePtr = std::unique_ptr<SDL_Surface, SDL_Surface_Deleter>;

    struct SDL_Texture_Deleter {
        void operator()(SDL_Texture* sdl_texture) {
          SDL_DestroyTexture(sdl_texture);
        }
    };
    using SDL_Texture_UniquePtr = std::unique_ptr<SDL_Texture, SDL_Texture_Deleter>;

    uint32_t get_pixel(int index) const;
    bool is_pixel_transparent(int index) const;
    uint32_t get_color_value(const Color& color) const;
    SDL_BlendMode get_sdl_blend_mode() const;

    static SDL_Surface* get_surface_from_file(
        const std::string& file_name,
        ImageDirectory base_directory);

    void create_software_surface();
    void convert_software_surface();
    void create_texture_from_surface();
    void add_subsurface(const SurfacePtr& src_surface, const Rectangle& region, const Point& dst_position);
    void clear_subsurfaces();
    void render(
        SDL_Renderer* renderer,
        const Rectangle& src_rect,
        const Rectangle& dst_rect,
        const Rectangle& clip_rect,
        uint8_t opacity,
        const std::vector<SubSurfaceNodePtr>& subsurfaces
    );

    std::vector<SubSurfaceNodePtr>
        subsurfaces;                      /**< Source Subsurfaces not in the tree yet */

    bool software_destination;            /**< Whether this surface should be modified on software side
                                           * (and therefore immediately) when used as a destination */
    SDL_Surface_UniquePtr
        internal_surface;                 /**< The SDL_Surface encapsulated, if any. */
    SDL_Texture_UniquePtr
        internal_texture;                 /**< The SDL_Texture encapsulated, if any. */
    std::unique_ptr<Color>
        internal_color;                   /**< The background color to use, if any. */
    bool is_rendered;                     /**< Whether the current surface has been rendered.
                                           * Set to false when drawing a surface on this one. */
    uint8_t opacity;                      /**< Opacity (0: transparent, 255: opaque). */
    int width, height;                    /**< Size of the texture, avoid to use SDL_QueryTexture. */

};

}

#endif

