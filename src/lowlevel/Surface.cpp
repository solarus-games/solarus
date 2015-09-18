/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lowlevel/PixelFilter.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Transition.h"
#include <algorithm>
#include <sstream>

namespace Solarus {

/**
 * \brief Stores the tree of what surfaces have to be drawn on other surfaces.
 *
 * When a drawing is requested, if the destination surface is in GPU, no
 * drawing actually occurs: instead, the information is stored into this tree.
 * At rendering time, the tree is traversed to perform all drawings
 * accelerated in GPU.
 *
 * The root node of the tree is the screen, and the children nodes are the
 * surfaces drawn on it.
 *
 * Each node represents a source surface drawn somewhere, and the list of
 * surfaces drawn on itself.
 */
class Surface::SubSurfaceNode {

  public:

    /**
     * Creates a subsurface node.
     * \param src_surface The surface to draw.
     * \param src_rect Region of the surface to draw.
     * \param dst_rect The rectangle where to draw the surface, relative to
     * the parent surface.
     * \param subsurfaces Surfaces drawn onto src_surface.
     */
    SubSurfaceNode(
        SurfacePtr src_surface,
        const Rectangle& src_rect,
        const Rectangle& dst_rect,
        const std::vector<SubSurfaceNodePtr>& subsurfaces
    ):
      src_surface(src_surface),
      src_rect(src_rect),
      dst_rect(dst_rect),
      subsurfaces(subsurfaces) {

      // Clip the source rectangle to the size of the source surface.
      // Otherwise, SDL_RenderCopy() will stretch the image.
      // FIXME still buggy with software renderer for now but should be fixed soon :
      // https://bugzilla.libsdl.org/show_bug.cgi?id=1968
      if (this->src_rect.get_x() < 0) {
        this->src_rect.set_x(0);
        this->src_rect.set_width(this->src_rect.get_width() + src_rect.get_x());
        this->dst_rect.add_x(-src_rect.get_x());
      }
      if (this->src_rect.get_x() + this->src_rect.get_width() > src_surface->get_width()) {
        this->src_rect.set_width(src_surface->get_width() - this->src_rect.get_x());
      }
      if (src_rect.get_y() < 0) {
        this->src_rect.set_y(0);
        this->src_rect.set_height(this->src_rect.get_height() + src_rect.get_y());
        this->dst_rect.add_y(-src_rect.get_y());
      }
      if (this->src_rect.get_y() + this->src_rect.get_height() > src_surface->get_height()) {
        this->src_rect.set_height(src_surface->get_height() - this->src_rect.get_y());
      }

    }

    SurfacePtr src_surface;                      /**< Surface to draw. */
    Rectangle src_rect;                          /**< Region of the surface to draw. */
    Rectangle dst_rect;                          /**< The rectangle where to draw the surface, relative to the parent surface. */
    std::vector<SubSurfaceNodePtr> subsurfaces;  /**< Subsurfaces drawn onto src_surface. */
};

/**
 * \brief Creates a surface with the specified size.
 * \param width The width in pixels.
 * \param height The height in pixels.
 */
Surface::Surface(int width, int height):
  Drawable(),
  software_destination(true),
  internal_surface(nullptr),
  internal_texture(nullptr),
  internal_color(nullptr),
  is_rendered(false),
  internal_opacity(255),
  width(width),
  height(height) {

  Debug::check_assertion(width > 0 && height > 0,
      "Attempt to create a surface with an empty size");
}

/**
 * \brief Creates a surface form the specified SDL texture.
 *
 * This constructor must be used only by low-level classes that manipulate directly
 * SDL dependent surfaces.
 *
 * \param internal_surface The internal surface data.
 * The created surface takes ownership of this object.
 */
Surface::Surface(SDL_Surface* internal_surface):
  Drawable(),
  software_destination(true),
  internal_surface(internal_surface),
  internal_texture(nullptr),
  internal_color(nullptr),
  is_rendered(false),
  internal_opacity(255) {

  width = internal_surface->w;
  height = internal_surface->h;
}

/**
 * \brief Creates a surface with the specified size.
 *
 * The surface is initially transparent.
 *
 * \param width The width in pixels.
 * \param height The height in pixels.
 * \return The created surface.
 */
SurfacePtr Surface::create(int width, int height) {
  SurfacePtr surface = std::make_shared<Surface>(width, height);
  return surface;
}

/**
 * \brief Creates a surface with the specified size.
 * \param size The size in pixels.
 * \return The created surface.
 */
SurfacePtr Surface::create(const Size& size) {
  SurfacePtr surface = std::make_shared<Surface>(size.width, size.height);
  return surface;
}

/**
 * \brief Creates a surface from the specified image file name.
 *
 * This function acts like a constructor excepts that it returns nullptr if the
 * file does not exist or is not a valid image.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The surface created, or nullptr if the file could not be loaded.
 */
SurfacePtr Surface::create(const std::string& file_name,
    ImageDirectory base_directory) {

  SDL_Surface* sdl_surface = get_surface_from_file(file_name, base_directory);

  if (sdl_surface == nullptr) {
    return nullptr;
  }

  SurfacePtr surface = std::make_shared<Surface>(sdl_surface);
  return surface;
}

/**
 * \brief Returns the SDL_Surface corresponding to the requested file.
 *
 * The returned SDL_Surface has to be manually deleted.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The SDL_Surface.
 */
SDL_Surface* Surface::get_surface_from_file(
    const std::string& file_name,
    ImageDirectory base_directory) {

  std::string prefix;
  bool language_specific = false;

  if (base_directory == DIR_SPRITES) {
    prefix = "sprites/";
  }
  else if (base_directory == DIR_LANGUAGE) {
    language_specific = true;
    prefix = "images/";
  }
  std::string prefixed_file_name = prefix + file_name;

  if (!QuestFiles::data_file_exists(prefixed_file_name, language_specific)) {
    // File not found.
    return nullptr;
  }

  const std::string& buffer = QuestFiles::data_file_read(prefixed_file_name, language_specific);
  SDL_RWops* rw = SDL_RWFromMem(const_cast<char*>(buffer.data()), (int) buffer.size());

  SDL_Surface* software_surface = IMG_Load_RW(rw, 0);

  SDL_RWclose(rw);

  Debug::check_assertion(software_surface != nullptr,
      std::string("Cannot load image '") + prefixed_file_name + "'");

  return software_surface;
}

/**
 * \brief Converts the software surface to the preferred pixel format
 * (32-bit with alpha channel).
 */
void Surface::convert_software_surface() {

  Debug::check_assertion(internal_surface != nullptr,
      "Missing software surface to convert");

  SDL_PixelFormat* pixel_format = Video::get_pixel_format();
  if (internal_surface->format->format != pixel_format->format) {
    // Convert to the preferred pixel format.
    uint8_t opacity;
    SDL_GetSurfaceAlphaMod(internal_surface.get(), &opacity);
    SDL_Surface* converted_surface = SDL_ConvertSurface(
        internal_surface.get(),
        pixel_format,
        0
    );
    Debug::check_assertion(converted_surface != nullptr,
        "Failed to convert software surface");

    internal_surface = SDL_Surface_UniquePtr(converted_surface);
    SDL_SetSurfaceAlphaMod(internal_surface.get(), opacity);  // Re-apply the alpha.
  }
}

/**
 * \brief Creates a hardware texture from the software surface.
 *
 * Also converts the software surface to a preferred format if necessary.
 */
void Surface::create_texture_from_surface() {

  SDL_Renderer* main_renderer = Video::get_renderer();
  if (main_renderer != nullptr) {

    Debug::check_assertion(internal_surface != nullptr,
        "Missing software surface to create texture from");

    // Make sure the software surface has the same format as the texture.
    // This is because SDL_UpdateTexture does not have a format parameter
    // for performance reasons.
    convert_software_surface();

    // Create the texture.
    internal_texture = SDL_Texture_UniquePtr(
        SDL_CreateTexture(
            main_renderer,
            Video::get_pixel_format()->format,
            SDL_TEXTUREACCESS_STATIC,
            internal_surface->w,
            internal_surface->h
        )
    );
    SDL_SetTextureBlendMode(internal_texture.get(), SDL_BLENDMODE_BLEND);

    // Copy the pixels of the software surface to the GPU texture.
    SDL_UpdateTexture(internal_texture.get(), nullptr, internal_surface->pixels, internal_surface->pitch);
    SDL_GetSurfaceAlphaMod(internal_surface.get(), &internal_opacity);
  }
}

/**
 * \brief Returns the width of the surface.
 * \return the width in pixels
 */
int Surface::get_width() const {
  return width;
}

/**
 * \brief Returns the height of the surface.
 * \return the height in pixels
 */
int Surface::get_height() const {
  return height;
}

/**
 * \brief Returns the size of this surface.
 * \return the size of this surface
 */
Size Surface::get_size() const {
  return { get_width(), get_height() };
}

/**
 * \brief Sets the opacity of this surface.
 * \param opacity the opacity (0 to 255).
 */
void Surface::set_opacity(uint8_t opacity) {

  if (software_destination  // The destination surface is in RAM.
      || !Video::is_acceleration_enabled()  // The rendering is in RAM.
  ) {
    if (internal_surface == nullptr) {
      create_software_surface();
    }

    // The surface must be 32-bit with alpha value for this function to work.
    convert_software_surface();

    int error = SDL_SetSurfaceAlphaMod(internal_surface.get(), opacity);
    if (error != 0) {
      Debug::error(SDL_GetError());
    }
    is_rendered = false;  // The surface has changed.
  }
  else {
    internal_opacity = opacity;
  }
}

/**
 * \brief Returns a buffer of the raw pixels of this surface.
 *
 * Pixels returned have the RGBA 32-bit format.
 *
 * \return The pixel buffer.
 */
std::string Surface::get_pixels() const {

  if (!software_destination &&
      Video::is_acceleration_enabled()) {
    // The surface is in GPU.
    return "";  // TODO
  }

  const int num_pixels = get_width() * get_height();
  if (internal_surface == nullptr) {
    // No surface: this may be a color.

    if (internal_color == nullptr) {
      // No color either: fully transparent surface.
      return std::string(num_pixels * 4, (size_t) 0);
    }

    uint8_t r, g, b, a;
    internal_color->get_components(r, g, b, a);
    std::string pixel;
    pixel += r;
    pixel += g;
    pixel += b;
    pixel += a;
    std::ostringstream oss;
    for (int i = 0; i < num_pixels; ++i) {
      oss << pixel;
    }
    return oss.str();
  }

  if (internal_surface->format->format == SDL_PIXELFORMAT_ABGR8888) {
    // No conversion needed.
    const char* buffer = static_cast<const char*>(internal_surface->pixels);
    return std::string(buffer, num_pixels * 4);
  }

  // Convert to RGBA format.
  SDL_PixelFormat* format = SDL_AllocFormat(SDL_PIXELFORMAT_ABGR8888);  // TODO keep this object
  SDL_Surface_UniquePtr converted_surface(SDL_ConvertSurface(
      internal_surface.get(),
      format,
      0
  ));
  SDL_FreeFormat(format);
  Debug::check_assertion(converted_surface != nullptr,
      "Failed to convert pixels to RGBA format");
  const char* buffer = static_cast<const char*>(converted_surface->pixels);
  return std::string(buffer, num_pixels * 4);
}

/**
 * When this surface is used as the destination of a drawing operation,
 * returns whether the drawing operation is performed in RAM or by the GPU.
 *
 * By default, this setting is true and all drawing operations are performed
 * in RAM.
 * Otherwise, when 2D acceleration is active, drawing operations are delayed
 * and performed by the GPU at rendering time.
 *
 * \return Whether this surface is a software destination surface.
 */
bool Surface::is_software_destination() const {
  return software_destination;
}

/**
 * When this surface is used as the destination of a drawing operation,
 * sets whether the drawing operation is performed immediately in RAM or later
 * by the GPU.
 *
 * By default, this setting is true and all drawing operations are performed
 * in RAM.
 * Otherwise, when 2D acceleration is active, drawing operations are delayed
 * and performed by the GPU at rendering time.
 *
 * You should leave this to \c true if your surface is built from lots of source
 * surfaces that don't change often.
 *
 * Hardware destinations are intended to be used for internal optimizations of
 * the engine. They do not support all operations that software ones do.
 * In particular, a hardware surface can never be drawn on a software surface.
 * Also, when you draw on a hardware surface after it was rendered, previous
 * drawings on this surface get automatically cleared.
 *
 * Use hardware surfaces only if you know what you are doing.
 * If in doubt, leave this to \c true.
 *
 * \param software_destination Whether this surface is a software destination
 * surface.
 */
void Surface::set_software_destination(bool software_destination) {

  this->software_destination = software_destination;
  // The software surface if any will be created lazily.
}

/**
 * \brief Creates an internal surface in software mode for this surface.
 */
void Surface::create_software_surface() {

  Debug::check_assertion(internal_surface == nullptr,
      "Software surface already exists");

  // Create a surface with the appropriate pixel format.
  SDL_PixelFormat* format = Video::get_pixel_format();
  internal_surface = SDL_Surface_UniquePtr(
      SDL_CreateRGBSurface(
          0,
          width,
          height,
          32,
          format->Rmask,
          format->Gmask,
          format->Bmask,
          format->Amask
      )
  );
  SDL_SetSurfaceBlendMode(internal_surface.get(), SDL_BLENDMODE_BLEND);
  is_rendered = false;

  Debug::check_assertion(internal_surface != nullptr,
      "Failed to create software surface");
}

/**
 * \brief Clears this surface.
 *
 * The surface becomes fully transparent and its size remains unchanged.
 * The opacity property of the surface is preserved.
 */
void Surface::clear() {

  clear_subsurfaces();

  internal_color = nullptr;

  if (internal_texture != nullptr) {
    internal_texture = nullptr;
  }

  if (internal_surface != nullptr) {
    if (software_destination) {
      SDL_FillRect(
          internal_surface.get(),
          nullptr,
          get_color_value(Color::transparent)
      );
    }
    else {
      internal_surface = nullptr;
    }
  }
}

/**
 * \brief Clears a rectangle of this surface.
 *
 * This is only supported for software surfaces.
 * The rectangle cleared becomes fully transparent.
 *
 * \param where The rectangle to clear.
 */
void Surface::clear(const Rectangle& where) {

  Debug::check_assertion(software_destination,
      "Partial surface clear is only supported with software surfaces");

  if (internal_surface == nullptr) {
    // Nothing to do.
    return;
  }

  SDL_FillRect(
      internal_surface.get(),
      where.get_internal_rect(),
      get_color_value(Color::transparent)
  );
  is_rendered = false;  // The surface has changed.
}

/**
 * \brief Fills the entire surface with the specified color.
 *
 * If the color has a non-opaque alpha component, then the color is
 * alpha-blended onto the surface.
 *
 * \param color A color.
 */
void Surface::fill_with_color(const Color& color) {

  fill_with_color(color, Rectangle(0, 0, width, height));
}

/**
 * \brief Fills the rectangle at given coordinates with the specified color.
 *
 * If the color has a non-opaque alpha component, then the color is
 * alpha-blended onto the surface.
 *
 * \param color A color.
 * \param where The rectangle to fill.
 */
void Surface::fill_with_color(const Color& color, const Rectangle& where) {

  // Create a surface with the requested size and color and draw it.
  SurfacePtr colored_surface = Surface::create(where.get_size());
  colored_surface->set_software_destination(false);
  colored_surface->internal_color = std::unique_ptr<Color>(new Color(color));
  colored_surface->raw_draw_region(Rectangle(colored_surface->get_size()), *this, where.get_xy());
}

/**
 * \brief Add a SubSurface to draw on this surface.
 * \param src_surface The Surface to draw.
 * \param region The subrectangle to draw in the source surface.
 * \param dst_position Coordinates on this surface.
 */
void Surface::add_subsurface(
    const SurfacePtr& src_surface,
    const Rectangle& region,
    const Point& dst_position) {

  SubSurfaceNodePtr node(new SubSurfaceNode(
      src_surface,
      region,
      Rectangle(dst_position),
      src_surface->subsurfaces
  ));

  // Clear the subsurface queue if the current dst_surface has already been rendered.
  if (is_rendered) {
    clear_subsurfaces();
  }

  subsurfaces.push_back(node);
}

/**
 * \brief clear the internal SubSurface queue.
 */
void Surface::clear_subsurfaces() {

  subsurfaces.clear();
}

/**
 * \brief Draws this surface on another surface.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw(Surface& dst_surface, const Point& dst_position) {

  Rectangle region(0, 0, width, height);
  raw_draw_region(region, dst_surface, dst_position);
}

/**
 * \brief Draws a subrectangle of this surface on another surface.
 * \param region The subrectangle to draw in this object.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw_region(
    const Rectangle& region,
    Surface& dst_surface,
    const Point& dst_position) {

  if (dst_surface.software_destination  // The destination surface is in RAM.
      || !Video::is_acceleration_enabled()  // The rendering is in RAM.
  ) {

    if (dst_surface.internal_surface == nullptr) {
      dst_surface.create_software_surface();
    }

    // First, draw subsurfaces if any.
    // They can exist if the video mode recently switched from an accelerated
    // one to a software one.
    if (!subsurfaces.empty()) {

      if (this->internal_surface == nullptr) {
        create_software_surface();
      }

      std::vector<SubSurfaceNodePtr> subsurfaces = this->subsurfaces;
      this->subsurfaces.clear();  // Avoid infinite recursive calls if there are cycles.

      for (SubSurfaceNodePtr& subsurface: subsurfaces) {

        // TODO draw the subsurfaces of the whole tree recursively instead.
        // The current version is not correct because it handles only one level
        // (it ignores subsurface->subsurfaces).
        // Plus it needs the workaround above to avoid a stack overflow.
        subsurface->src_surface->raw_draw_region(
            subsurface->src_rect,
            *this,
            subsurface->dst_rect.get_xy()
        );
        subsurface = nullptr;
      }
      clear_subsurfaces();
    }

    if (this->internal_surface != nullptr) {
      // The source surface is not empty: draw it onto the destination.

      SDL_BlitSurface(
          this->internal_surface.get(),
          region.get_internal_rect(),
          dst_surface.internal_surface.get(),
          Rectangle(dst_position).get_internal_rect()
      );
    }
    else if (internal_color != nullptr) { // No internal surface to draw: this may be a color.

      if (internal_color->get_alpha() == 255) {
        // Fill with opaque color: we can directly modify the destination pixels.
        Rectangle dst_rect(
            dst_position,
            region.get_size()
        );
        SDL_FillRect(
            dst_surface.internal_surface.get(),
            dst_rect.get_internal_rect(),
            get_color_value(*internal_color)
        );
      }
      else {
        // Fill with semi-transparent pixels: perform alpha-blending.
        create_software_surface();
        SDL_FillRect(
            this->internal_surface.get(),
            nullptr,
            get_color_value(*internal_color)
        );
        SDL_BlitSurface(
            this->internal_surface.get(),
            region.get_internal_rect(),
            dst_surface.internal_surface.get(),
            Rectangle(dst_position).get_internal_rect()
        );
      }
    }
  }
  else {
    // The destination is a GPU surface (a texture).
    // Do not draw anything, just store the operation in the tree instead.
    // The actual drawing will be done at rendering time in GPU.

    SurfacePtr src_surface = std::static_pointer_cast<Surface>(shared_from_this());
    dst_surface.add_subsurface(src_surface, region, dst_position);
  }

  dst_surface.is_rendered = false;
}

/**
 * \brief Draws a transition effect on this drawable object.
 * \param transition The transition effect to apply.
 */
void Surface::draw_transition(Transition& transition) {
  transition.draw(*this);
}

/**
 * \brief Draws this software surface with a pixel filter on another software
 * surface.
 * \param pixel_filter The pixel filter to apply.
 * \param dst_surface The destination surface. It must have the size of the
 * this surface multiplied by the scaling factor of the filter.
 */
void Surface::apply_pixel_filter(
    const PixelFilter& pixel_filter, Surface& dst_surface) {

  const int factor = pixel_filter.get_scaling_factor();
  Debug::check_assertion(dst_surface.get_width() == get_width() * factor,
      "Wrong destination surface size");
  Debug::check_assertion(dst_surface.get_height() == get_height() * factor,
      "Wrong destination surface size");

  SDL_Surface* src_internal_surface = this->internal_surface.get();
  SDL_Surface* dst_internal_surface = dst_surface.internal_surface.get();

  if (src_internal_surface == nullptr) {
    // This is possible if nothing was drawn on the surface yet.
    return;
  }

  Debug::check_assertion(dst_internal_surface != nullptr,
      "Missing software destination surface for pixel filter");

  SDL_LockSurface(src_internal_surface);
  SDL_LockSurface(dst_internal_surface);

  uint32_t* src = static_cast<uint32_t*>(src_internal_surface->pixels);
  uint32_t* dst = static_cast<uint32_t*>(dst_internal_surface->pixels);

  pixel_filter.filter(src, get_width(), get_height(), dst);

  SDL_UnlockSurface(dst_internal_surface);
  SDL_UnlockSurface(src_internal_surface);

  // The destination surface has changed.
  dst_surface.is_rendered = false;
}

/**
 * \brief Draws the internal texture if any, and all subtextures on the
 * renderer.
 * \param renderer The renderer where to draw.
 */
void Surface::render(SDL_Renderer* renderer) {

  const Rectangle size(get_size());
  render(renderer, size, size, size, 255, subsurfaces);
}

/**
 * \brief Renders the internal texture if any, and all subsurfaces that are
 * drawn onto it.
 * \param renderer The renderer where to draw.
 * \param src_rect The subrectangle of the texture to draw.
 * \param dst_rect The position where to draw on the renderer.
 * \param clip_rect A portion of the renderer where to restrict the drawing.
 * \param opacity The opacity of the parent surface.
 * \param subsurfaces The subsurfaces drawn onto this texture. They will be
 * renderered recursively.
 */
void Surface::render(
    SDL_Renderer* renderer,
    const Rectangle& src_rect,
    const Rectangle& dst_rect,
    const Rectangle& clip_rect,
    uint8_t opacity,
    const std::vector<SubSurfaceNodePtr>& subsurfaces
) {

  //FIXME SDL_RenderSetClipRect is buggy for now, but should be fixed soon.
  // It means that software and hardware surface doesn't have the exact same behavior for now.
  // Uncomment the two lines using it when https://bugzilla.libsdl.org/show_bug.cgi?id=2336 will be solved.

  // Accelerate the internal software surface.
  if (internal_surface != nullptr) {

    if (internal_texture == nullptr) {
      create_texture_from_surface();
    }

    // If the software surface has changed, update the hardware texture.
    else if (
        (software_destination || !Video::is_acceleration_enabled())
         && !is_rendered) {
      convert_software_surface();
      SDL_UpdateTexture(
          internal_texture.get(),
          nullptr,
          internal_surface->pixels,
          internal_surface->pitch
      );
      SDL_GetSurfaceAlphaMod(internal_surface.get(), &internal_opacity);
    }
  }

  const uint8_t current_opacity = std::min(internal_opacity, opacity);

  // Draw the internal color as background color.
  if (internal_color != nullptr) {
    uint8_t r, g, b, a;
    internal_color->get_components(r, g, b, a);

    SDL_SetRenderDrawColor(renderer, r, g, b, std::min((uint8_t) a, current_opacity));
    //SDL_RenderSetClipRect(renderer, clip_rect.get_internal_rect());
    SDL_RenderFillRect(renderer, clip_rect.get_internal_rect());
  }

  // Draw the internal texture.
  if (internal_texture != nullptr) {
    SDL_SetTextureAlphaMod(internal_texture.get(), current_opacity);
    //SDL_RenderSetClipRect(renderer, clip_rect.get_internal_rect());

    SDL_RenderCopy(
        renderer,
        internal_texture.get(),
        src_rect.get_internal_rect(),
        dst_rect.get_internal_rect()
    );
  }

  // The surface is rendered. Now draw all subtextures.
  for (const SubSurfaceNodePtr& subsurface: subsurfaces) {

    // subsurface has to be drawn on this surface

    // Calculate absolute destination subrectangle position on screen.
    Rectangle subsurface_dst_rect(
        dst_rect.get_x() + subsurface->dst_rect.get_x() - src_rect.get_x(),
        dst_rect.get_y() + subsurface->dst_rect.get_y() - src_rect.get_y(),
        subsurface->src_rect.get_width(),
        subsurface->src_rect.get_height()
    );

    // Set the intersection of the subsurface destination and this surface's clip as clipping rectangle.
    Rectangle superimposed_clip_rect;
    if (SDL_IntersectRect(subsurface_dst_rect.get_internal_rect(),
        clip_rect.get_internal_rect(),
        superimposed_clip_rect.get_internal_rect())) {

      // If there is an intersection, render the subsurface.
      subsurface->src_surface->render(
          renderer,
          subsurface->src_rect,
          subsurface_dst_rect,
          superimposed_clip_rect,
          current_opacity,
          subsurface->subsurfaces
      );
    }
  }

  is_rendered = true;
}

/**
 * \brief Returns the surface where transitions on this drawable object
 * are applied.
 * \return The surface for transitions.
 */
Surface& Surface::get_transition_surface() {
  return *this;
}

/**
 * \brief Returns a pixel value of this surface.
 *
 * The pixel format is preserved: if it is lower than 32 bpp, then the unused
 * upper bits of the value are is padded with zeros.
 *
 * \param index Index of the pixel to get.
 * \return The value of this pixel.
 */
uint32_t Surface::get_pixel(int index) const {

  SOLARUS_ASSERT(internal_surface != nullptr,
    "Attempt to read a pixel on a hardware or a buffer surface.");

  SDL_PixelFormat* format = internal_surface->format;

  // Test from the most common to the most exotic.
  switch (format->BytesPerPixel) {

    case 1:
      {
        uint8_t* pixels = static_cast<uint8_t*>(internal_surface->pixels);
        return pixels[index];
      }

    case 4:
      {
        uint32_t* pixels = static_cast<uint32_t*>(internal_surface->pixels);
        return pixels[index];
      }

    case 2:
      {
        uint16_t* pixels = static_cast<uint16_t*>(internal_surface->pixels);
        return pixels[index];
      }

    case 3:
      {
        // Manual cast of the pixel into uint32_t.
        uint8_t* bytes = static_cast<uint8_t*>(internal_surface->pixels);
        return *reinterpret_cast<uint32_t*>(&bytes[index * 3]) & 0xffffff00 >> 8;
      }
  }

  std::ostringstream oss;
  oss << "Unknown pixel depth: " << format->BitsPerPixel;
  Debug::die(oss.str());
}

/**
 * \brief Returns whether a pixel is transparent.
 *
 * A pixel is transparent if it corresponds to the colorkey
 * or if its alpha channel is equal to 0.
 *
 * \param index The index of the pixel to test.
 * \return \c true if the pixel is transparent.
 */
bool Surface::is_pixel_transparent(int index) const {

  uint32_t pixel = get_pixel(index);
  uint32_t colorkey;
  bool with_colorkey = SDL_GetColorKey(internal_surface.get(), &colorkey) == 0;

  if (with_colorkey && pixel == colorkey) {
    // The pixel has the transparency color.
    return true;
  }

  if (internal_surface->format->Amask != 0               // There exists an alpha channel.
      && (pixel & internal_surface->format->Amask) == 0  // The pixel is fully transparent.
  ) {
    return true;
  }

  return false;
}

/**
 * \brief Converts a color to a 32-bit value in the current video format.
 * \param color The color to convert.
 * \return The pixel value of this color in the current format.
 */
uint32_t Surface::get_color_value(const Color& color) const {
  uint8_t r, g, b, a;
  color.get_components(r, g, b, a);
  return SDL_MapRGBA(Video::get_pixel_format(), r, g, b, a);
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& Surface::get_lua_type_name() const {
  return LuaContext::surface_module_name;
}

}

