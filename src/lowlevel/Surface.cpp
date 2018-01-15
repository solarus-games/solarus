/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lowlevel/SoftwarePixelFilter.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/Transition.h"
#include <algorithm>
#include <iostream>
#include <sstream>

namespace Solarus {

/**
 * \brief Creates a surface with the specified size.
 * \param width The width in pixels.
 * \param height The height in pixels.
 */
Surface::Surface(int width, int height):
  Drawable(),
  internal_surface(nullptr),
  opacity(255) {

  Debug::check_assertion(width > 0 && height > 0,
      "Attempt to create a surface with an empty size");

  SDL_PixelFormat* format = Video::get_pixel_format();
  internal_surface = SDL_Surface_UniquePtr(SDL_CreateRGBSurface(
      0,
      width,
      height,
      32,
      format->Rmask,
      format->Gmask,
      format->Bmask,
      format->Amask
  ));

  Debug::check_assertion(internal_surface != nullptr,
      std::string("Failed to create SDL surface: ") + SDL_GetError());
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
  internal_surface(internal_surface),
  opacity(255) {

  // Convert to the preferred pixel format.
  SDL_PixelFormat* pixel_format = Video::get_pixel_format();
  if (internal_surface->format->format != pixel_format->format) {

    SDL_Surface* converted_surface = SDL_ConvertSurface(
        internal_surface,
        pixel_format,
        0
    );
    Debug::check_assertion(converted_surface != nullptr,
                           std::string("Failed to convert surface") + SDL_GetError());
    this->internal_surface = SDL_Surface_UniquePtr(converted_surface);
  }
}

/**
 * \brief Destructor.
 */
Surface::~Surface() {

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
 * \brief Creates an SDL surface corresponding to the requested file.
 *
 * The returned SDL surface has to be manually deleted.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The SDL surface created.
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

  SDL_Surface* surface = IMG_Load_RW(rw, 0);

  SDL_RWclose(rw);

  Debug::check_assertion(surface != nullptr,
      std::string("Cannot load image '") + prefixed_file_name + "'");

  SDL_PixelFormat* pixel_format = Video::get_pixel_format();
  if (surface->format->format == pixel_format->format) {
    return surface;
  }

  // Convert to the preferred pixel format.
  uint8_t opacity;
  SDL_GetSurfaceAlphaMod(surface, &opacity);
  SDL_Surface* converted_surface = SDL_ConvertSurface(
        surface,
        pixel_format,
        0
        );
  Debug::check_assertion(converted_surface != nullptr,
                         std::string("Failed to convert software surface: ") + SDL_GetError());
  SDL_FreeSurface(surface);

  SDL_SetSurfaceAlphaMod(converted_surface, opacity);  // Re-apply the alpha.
  SDL_SetSurfaceBlendMode(converted_surface, SDL_BLENDMODE_BLEND);

  return converted_surface;
}

/**
 * \brief Returns the width of the surface.
 * \return the width in pixels
 */
int Surface::get_width() const {
  return internal_surface->w;
}

/**
 * \brief Returns the height of the surface.
 * \return the height in pixels
 */
int Surface::get_height() const {
  return internal_surface->h;
}

/**
 * \brief Returns the size of this surface.
 * \return the size of this surface
 */
Size Surface::get_size() const {
  return { get_width(), get_height() };
}

/**
 * \brief Returns the opacity of this surface.
 * \return The opacity (0 to 255).
 */
uint8_t Surface::get_opacity() const {
  return opacity;
}

/**
 * \brief Sets the opacity of this surface.
 * \param opacity The opacity (0 to 255).
 */
void Surface::set_opacity(uint8_t opacity) {

  this->opacity = opacity;
  int error = SDL_SetSurfaceAlphaMod(internal_surface.get(), opacity);
  if (error != 0) {
    Debug::error(SDL_GetError());
  }
}

/**
 * \brief Returns the SDL surface wrapped.
 * \return The internal SDL surface.
 */
SDL_Surface* Surface::get_internal_surface() {
  return internal_surface.get();
}

/**
 * \brief Returns a buffer of the raw pixels of this surface.
 *
 * Pixels returned have the RGBA 32-bit format.
 *
 * \return The pixel buffer.
 */
std::string Surface::get_pixels() const {

  const int num_pixels = get_width() * get_height();

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
      std::string("Failed to convert pixels to RGBA format") + SDL_GetError());
  const char* buffer = static_cast<const char*>(converted_surface->pixels);
  return std::string(buffer, num_pixels * 4);
}

/**
 * \brief Clears this surface.
 *
 * The surface becomes fully transparent and its size remains unchanged.
 * The opacity property of the surface is preserved.
 */
void Surface::clear() {

  SDL_FillRect(
      internal_surface.get(),
      nullptr,
      get_color_value(Color::transparent)
  );
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

  SDL_FillRect(
      internal_surface.get(),
      where.get_internal_rect(),
      get_color_value(Color::transparent)
  );
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
  fill_with_color(color, Rectangle(Point(0, 0), get_size()));
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

  if (color.get_alpha() == 255) {
    // Opaque color: directly replace the pixel values.
    SDL_FillRect(internal_surface.get(), where.get_internal_rect(), get_color_value(color));
  }
  else {
    // We need an intermediate surface to perform alpha blending.
    if (alpha_color_surface == nullptr) {
      SDL_PixelFormat* format = Video::get_pixel_format();
      alpha_color_surface = SDL_Surface_UniquePtr(SDL_CreateRGBSurface(
          0,
          where.get_width(),
          where.get_height(),
          32,
          format->Rmask,
          format->Gmask,
          format->Bmask,
          format->Amask
      ));

      Debug::check_assertion(alpha_color_surface != nullptr,
          std::string("Failed to create SDL surface: ") + SDL_GetError());
    }

    SDL_FillRect(alpha_color_surface.get(), nullptr, get_color_value(color));
    SDL_BlitSurface(
        alpha_color_surface.get(),
        nullptr,
        internal_surface.get(),
        Rectangle(where).get_internal_rect()
    );
  }
}

/**
 * \brief Draws this surface on another surface.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw(Surface& dst_surface, const Point& dst_position) {

  Rectangle region(0, 0, get_width(), get_height());
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

  SDL_SetSurfaceBlendMode(
        this->internal_surface.get(),
        get_sdl_blend_mode()
  );
  SDL_BlitSurface(
      this->internal_surface.get(),
      region.get_internal_rect(),
      dst_surface.internal_surface.get(),
      Rectangle(dst_position).get_internal_rect()
  );
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
    const SoftwarePixelFilter& pixel_filter, Surface& dst_surface) {

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
 * \brief Returns the blend mode as an SDL_BlendMode value.
 * \return The blend mode.
 */
SDL_BlendMode Surface::get_sdl_blend_mode() const {

  switch (get_blend_mode()) {

  case BlendMode::NONE:
    return SDL_BLENDMODE_NONE;

  case BlendMode::BLEND:
    return SDL_BLENDMODE_BLEND;

  case BlendMode::ADD:
    return SDL_BLENDMODE_ADD;

  case BlendMode::MULTIPLY:
    return SDL_BLENDMODE_MOD;
  }

  return SDL_BLENDMODE_BLEND;
}

/**
 * \brief Renders this surface onto a hardware texture.
 */
void Surface::render(SDL_Texture& render_target) {

  SDL_UpdateTexture(
      &render_target,
      nullptr,
      internal_surface->pixels,
      internal_surface->pitch
  );
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& Surface::get_lua_type_name() const {
  return LuaContext::surface_module_name;
}

}

