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
#include "lowlevel/Surface.h"
#include "lowlevel/Color.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/PixelFilter.h"
#include "lua/LuaContext.h"
#include "Transition.h"
#include <SDL.h>
#include <SDL_image.h>

/**
 * \brief Stores the tree of what surfaces have to be drawn on other surfaces.
 *
 * When a drawing is requested, if the destination surface is in GPU, no
 * drawing actually occurs: instead, the information is stored into this tree.
 * At rendering time, the tree is traverses to perform all drawings
 * accelerated in GPU.
 *
 * The root node of the tree is the screen, and the children nodes are the
 * surfaces drawn on it.
 *
 * Each node represents a source surface drawn somewhere, and the list of
 * surfaces drawn on itself.
 */
class Surface::SubSurfaceNode: public RefCountable {

  public:

    SubSurfaceNode(
        Surface* src_surface,
        const Rectangle& src_rect,
        const Rectangle& dst_rect,
        const std::vector<SubSurfaceNode*> subsurfaces
    ):
      src_surface(src_surface),
      src_rect(src_rect),
      dst_rect(dst_rect),
      subsurfaces(subsurfaces) {

      RefCountable::ref(src_surface);
      for (unsigned i = 0; i < subsurfaces.size(); ++i) {
        RefCountable::ref(subsurfaces[i]);
      }
    }

    ~SubSurfaceNode() {

      RefCountable::unref(src_surface);

      for (unsigned i = 0; i < subsurfaces.size(); ++i) {
        RefCountable::unref(subsurfaces[i]);
      }
    }

    Surface* src_surface;                        /**< Surface to draw. */

    Rectangle src_rect;                          /**< Region of the Subsurface to draw. */
    Rectangle dst_rect;                          /**< The rectangle where to draw the Subsurface, relative to the parent Surface. */
    std::vector<SubSurfaceNode*> subsurfaces;    /**< Subsurfaces drawn onto src_surface. */
};

/**
 * \brief Creates a surface with the specified size.
 * \param width The width in pixels.
 * \param height The height in pixels.
 */
Surface::Surface(int width, int height):
  Drawable(),
  internal_color(NULL),
  software_destination(false),
  internal_surface(NULL),
  internal_texture(NULL),
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
 * This constructor must be used only by lowlevel classes that manipulate directly
 * SDL dependent surfaces.
 *
 * \param internal_surface The internal surface data.
 * The destructor will free it.
 */
Surface::Surface(SDL_Surface* internal_surface):
  Drawable(),
  internal_color(NULL),
  software_destination(false),
  internal_surface(internal_surface),
  internal_texture(NULL),
  is_rendered(false),
  internal_opacity(255) {

  width = internal_surface->w;
  height = internal_surface->h;
}

/**
 * \brief Destructor.
 */
Surface::~Surface() {

  if (internal_texture != NULL) {
    SDL_DestroyTexture(internal_texture);
  }
  if (internal_surface != NULL) {
    SDL_FreeSurface(internal_surface);
  }

  delete internal_color;

  clear_subsurfaces();
}

/**
 * \brief Creates a surface with the specified size.
 * \param width The width in pixels.
 * \param height The height in pixels.
 */
Surface* Surface::create(int width, int height) {
  return new Surface(width, height);
}

/**
 * \brief Creates a surface with the specified size.
 * \param width The width in pixels.
 * \param height The height in pixels.
 */
Surface* Surface::create(const Rectangle& size) {
  return new Surface(size.get_width(), size.get_height());
}

/**
 * \brief Creates a surface from the specified image file name.
 *
 * This function acts like a constructor excepts that it returns NULL if the
 * file does not exist or is not a valid image.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The surface created, or NULL if the file could not be loaded.
 */
Surface* Surface::create(const std::string& file_name,
    ImageDirectory base_directory) {

  SDL_Surface* sdl_surface = get_surface_from_file(file_name, base_directory);

  if (sdl_surface == NULL) {
    return NULL;
  }

  Surface* surface = new Surface(sdl_surface);
  surface->set_software_destination(true);
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
  
  if (!FileTools::data_file_exists(prefixed_file_name, language_specific)) {
    // File not found.
    return NULL;
  }

  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(prefixed_file_name, &buffer, &size, language_specific);
  SDL_RWops* rw = SDL_RWFromMem(buffer, int(size));

  SDL_Surface* software_surface = IMG_Load_RW(rw, 0);

  SDL_RWclose(rw);
  FileTools::data_file_close_buffer(buffer);

  Debug::check_assertion(software_surface != NULL, StringConcat() <<
    "Cannot load image '" << prefixed_file_name << "'");

  return software_surface;
}

/**
 * \brief Creates a hardware texture from the software surface.
 *
 * Also converts the software surface to a preferred format if necessary.
 */
void Surface::create_texture_from_surface() {

  SDL_Renderer* main_renderer = VideoManager::get_instance()->get_renderer();
  SDL_PixelFormat* pixel_format = VideoManager::get_pixel_format();

  if (main_renderer != NULL) {

    Debug::check_assertion(internal_surface != NULL,
        "Missing software surface to create texture from");
    
    // Make sure the software surface has the same format as the texture.
    // This is because SDL_UpdateTexture does not have a format parameter
    // for performance reasons.
    if (internal_surface->format->format != pixel_format->format) {
      // Convert to the preferred pixel format.
      SDL_Surface* converted_surface = SDL_ConvertSurface(
          internal_surface,
          pixel_format,
          0
      );
      SDL_FreeSurface(internal_surface);
      internal_surface = converted_surface;
    }

    // Create the texture.
    internal_texture = SDL_CreateTexture(
        main_renderer,
        pixel_format->format,
        SDL_TEXTUREACCESS_STATIC,
        internal_surface->w,
        internal_surface->h
    );
    SDL_SetTextureBlendMode(internal_texture, SDL_BLENDMODE_BLEND);

    // Copy the pixels of the sofware surface to the GPU texture.
    SDL_UpdateTexture(internal_texture, NULL, internal_surface->pixels, internal_surface->pitch);
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
const Rectangle Surface::get_size() const {

  return Rectangle(0, 0, get_width(), get_height());
}

/**
 * \brief Sets the opacity of this surface.
 * \param opacity the opacity (0 to 255).
 */
void Surface::set_opacity(int opacity) {

  internal_opacity = opacity;
}

/**
 * When this surface is used as the destination of a drawing operation,
 * return whether the drawing operation is performed in RAM or by the GPU
 * at rendering time.
 *
 * By default, this setting is false and all drawing operations are
 * performed by the GPU when 2D acceleration is active.
 */
bool Surface::is_software_destination() const {
  return software_destination;
}

/**
 * When this surface is used as the destination of a drawing operation,
 * return whether the drawing operation is performed in RAM or by the GPU
 * at rendering time.
 *
 * By default, this setting is false and all drawing operations are
 * performed by the GPU when 2D acceleration is active.
 *
 * You should set this to \c true if your surface is built from lots of source
 * surfaces that don't change often.
 * If in doubt, leave it to \c false.
 */
void Surface::set_software_destination(bool software_destination) {

  this->software_destination = software_destination;
  // The software surface if any will be created lazily.

  if (!software_destination && internal_surface != NULL) {
    SDL_FreeSurface(internal_surface);
    internal_surface = NULL;
  }
}

/**
 * \brief Creates an internal surface in software mode for this surface.
 */
void Surface::create_software_surface() {

  Debug::check_assertion(internal_surface == NULL,
      "Software surface already exists");

  // Create a surface with the appropriate pixel format.
  SDL_PixelFormat* format = VideoManager::get_pixel_format();
  internal_surface = SDL_CreateRGBSurface(
      0,
      width,
      height,
      32,
      format->Rmask,
      format->Gmask,
      format->Bmask,
      format->Amask
  );
  is_rendered = false;
}

/**
 * \brief Fills the entire surface with the specified color.
 * \param color A color.
 */
void Surface::fill_with_color(Color& color) {

  fill_with_color(color, Rectangle(0, 0, width, height));
}

/**
 * \brief Fill the rectangle at given coords with the specified color.
 * \param color A color.
 * \param where The rectangle to fill.
 */
void Surface::fill_with_color(Color& color, const Rectangle& where) {

  if (software_destination  // The destination surface is in RAM.
      || !VideoManager::get_instance()->is_acceleration_enabled()  // The rendering is in RAM.
  ) {

    // We have to draw on a software surface: draw pixels directly.
    if (internal_surface == NULL) {
      create_software_surface();
    }

    uint32_t color_value = color.get_internal_value();
    if (internal_surface->format->format != VideoManager::get_pixel_format()->format) {
      // Get a color value in the pixel format of the destination surface.
      int r, g, b, a;
      color.get_components(r, g, b, a);
      color_value = SDL_MapRGBA(internal_surface->format, r, g, b, a);
    }

    SDL_FillRect(
        internal_surface,
        Rectangle(where).get_internal_rect(),
        color_value
    );
    is_rendered = false;  // The surface has changed.
  }
  // Else, create a Surface with the requested size and color, and add it to the subsurface queue.
  else {
    Rectangle subsurface_size(0, 0, where.get_width(), where.get_height());
    Surface* subsurface = Surface::create(subsurface_size);
    RefCountable::ref(subsurface);
    subsurface->internal_color = new Color(color);
    
    subsurface->raw_draw_region(subsurface_size, *this, where);
  }
}

/**
 * \brief Add a SubSurface to draw on this surface.
 * \param src_surface The Surface to draw.
 * \param region The subrectangle to draw in the source surface.
 * \param dst_position Coordinates on this surface.
 */
void Surface::add_subsurface(
    Surface& src_surface,
    const Rectangle& region,
    const Rectangle& dst_position) {

  SubSurfaceNode* node = new SubSurfaceNode(
      &src_surface,
      region,
      dst_position,
      src_surface.subsurfaces
  );
  RefCountable::ref(node);

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

  for (unsigned i = 0; i < subsurfaces.size() ; ++i) {
    RefCountable::unref(subsurfaces[i]);
  }
  subsurfaces.clear();
}

/**
 * \brief Draws this surface on another surface.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw(Surface& dst_surface, const Rectangle& dst_position) {

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
    const Rectangle& dst_position) {

  if (dst_surface.software_destination  // The destination surface is in RAM.
      || !VideoManager::get_instance()->is_acceleration_enabled()  // The rendering is in RAM.
  ) {

    if (this->internal_surface != NULL) {
      // The source surface is not empty: draw it onto the destination.

      if (dst_surface.internal_surface == NULL) {
        dst_surface.create_software_surface();
      }

      SDL_BlitSurface(
          this->internal_surface,
          region.get_internal_rect(),
          dst_surface.internal_surface,
          Rectangle(dst_position).get_internal_rect()
      );
    }
  }
  else {
    dst_surface.add_subsurface(*this, region, dst_position);
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
 * \brief Draws this surface with a pixel filter on another surface.
 * \param filter The pixel filter to apply.
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

  SDL_Surface* src_internal_surface = this->internal_surface;
  SDL_Surface* dst_internal_surface = dst_surface.internal_surface;

  Debug::check_assertion(src_internal_surface != NULL,
      "Missing software source surface for pixel filter");
  Debug::check_assertion(dst_internal_surface != NULL,
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
 *
 * Clears the subsurfaces vector at the end of the method.
 *
 * \param renderer The renderer where to draw.
 */
void Surface::render(SDL_Renderer* renderer) {

  const Rectangle size(get_size());
  render(renderer, size, size, size, 255, subsurfaces);
}

/**
 * \brief Draws the internal texture if any, and all subtextures on the
 * renderer.
 *
 * Clears the subsurfaces vector at the end of the method.
 *
 * \param renderer The renderer where to draw.
 * \param src_rect The subrectangle of the texture to draw.
 * \param dst_rect The portion of renderer where to draw.
 * \param clip_rect The clip area, correspond to parents dst_rect.
 * \param opacity The opacity of the parent surface.
 */
void Surface::render(
    SDL_Renderer* renderer,
    const Rectangle& src_rect,
    const Rectangle& dst_rect,
    const Rectangle& clip_rect,
    int opacity,
    const std::vector<SubSurfaceNode*>& subsurfaces) {

  const int current_opacity = std::min(internal_opacity, opacity);

  // Accelerate the internal software surface.
  if (internal_surface != NULL) {

    if (internal_texture == NULL) {
      create_texture_from_surface();
    }

    // If the software surface has changed, update the hardware texture.
    else if (software_destination && !is_rendered) {
      SDL_UpdateTexture(
          internal_texture,
          NULL,
          internal_surface->pixels,
          internal_surface->pitch
      );
    }
  }

  // Draw the internal color as background color.
  if (internal_color != NULL) {
    Rectangle viewport = clip_rect;
    int r, g, b, a;
    VideoManager::set_absolute_position(viewport);
    internal_color->get_components(r, g, b, a);
    
    SDL_SetRenderDrawColor(renderer, r, g, b, std::min(a, current_opacity));
    SDL_RenderSetClipRect(renderer, viewport.get_internal_rect());
    SDL_RenderFillRect(renderer, clip_rect.get_internal_rect());
  }

  // Draw the internal texture.
  if (internal_texture != NULL) {
    Rectangle viewport = clip_rect;
    VideoManager::set_absolute_position(viewport);
    
    SDL_SetTextureAlphaMod(internal_texture, current_opacity);
    SDL_RenderSetClipRect(renderer, viewport.get_internal_rect());
    SDL_RenderCopy(
        renderer,
        internal_texture,
        src_rect.get_internal_rect(),
        dst_rect.get_internal_rect());
  }

  // Draw all subtextures.
  std::vector<SubSurfaceNode*>::const_iterator it;
  const std::vector<SubSurfaceNode*>::const_iterator end = subsurfaces.end();
  for (it = subsurfaces.begin(); it != end; ++it) {
    SubSurfaceNode* subsurface = *it;

    // Calculate absolute destination subrectangle position on screen.
    Rectangle subsurface_dst_rect(
        dst_rect.get_x() + subsurface->dst_rect.get_x() - src_rect.get_x(),
        dst_rect.get_y() + subsurface->dst_rect.get_y() - src_rect.get_y(),
        subsurface->src_rect.get_width(),
        subsurface->src_rect.get_height());

    // Set the intersection of the subsurface destination and this surface's clip as clipping rectangle.
    Rectangle superimposed_clip_rect;
    if (SDL_IntersectRect(subsurface_dst_rect.get_internal_rect(),
        dst_rect.get_internal_rect(),
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

  SOLARUS_ASSERT(internal_surface != NULL,
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

  Debug::die(StringConcat() << "Unknown pixel depth: " << format->BitsPerPixel);
  return 0;
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
  bool with_colorkey = SDL_GetColorKey(internal_surface, &colorkey) == 0;

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
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& Surface::get_lua_type_name() const {
  return LuaContext::surface_module_name;
}

