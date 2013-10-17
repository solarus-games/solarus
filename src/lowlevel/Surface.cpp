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
#include "lua/LuaContext.h"
#include "Transition.h"
#include <SDL_image.h>

/**
 * \brief Creates a surface with the specified size.
 * \param width The width in pixels.
 * \param height The height in pixels.
 */
Surface::Surface(int width, int height):
  Drawable(),
  internal_surface(NULL),
  internal_surface_created(true),
  with_colorkey(false),
  colorkey(0) {

  Debug::check_assertion(width > 0 && height > 0,
      "Attempt to create a surface with an empty size");

  this->internal_surface = SDL_CreateRGBSurface(
    SDL_SWSURFACE, width, height, SOLARUS_COLOR_DEPTH, 0, 0, 0, 0);
}

/**
 * \brief Creates a surface with the specified size.
 * \param size The size in pixels.
 */
Surface::Surface(const Rectangle& size):
  Drawable(),
  internal_surface(NULL),
  internal_surface_created(true),
  with_colorkey(false),
  colorkey(0) {

  Debug::check_assertion(size.get_width() > 0 && size.get_height() > 0, "Empty surface");

  this->internal_surface = SDL_CreateRGBSurface(
      SDL_SWSURFACE, size.get_width(), size.get_height(), SOLARUS_COLOR_DEPTH, 0, 0, 0, 0);
}

/**
 * \brief Creates a surface from the specified image file name.
 *
 * An assertion error occurs if the file cannot be loaded.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 */
Surface::Surface(const std::string& file_name, ImageDirectory base_directory):
  Drawable(),
  internal_surface(NULL),
  internal_surface_created(true),
  with_colorkey(false),
  colorkey(0) {

  std::string prefix = "";
  bool language_specific = false;

  if (base_directory == DIR_SPRITES) {
    prefix = "sprites/";
  }
  else if (base_directory == DIR_LANGUAGE) {
    language_specific = true;
    prefix = "images/";
  }
  std::string prefixed_file_name = prefix + file_name;

  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(prefixed_file_name, &buffer, &size, language_specific);
  SDL_RWops* rw = SDL_RWFromMem(buffer, int(size));
  this->internal_surface = IMG_Load_RW(rw, 0);
  FileTools::data_file_close_buffer(buffer);
  SDL_RWclose(rw);

  Debug::check_assertion(internal_surface != NULL, StringConcat() <<
      "Cannot load image '" << prefixed_file_name << "'");
    
  with_colorkey = SDL_GetColorKey(internal_surface, &colorkey) == 0;
}

/**
 * \brief Creates a surface form the specified SDL surface.
 *
 * This constructor must be used only by lowlevel classes that manipulate directly
 * SDL dependent surfaces.
 *
 * \param internal_surface The internal surface data. It must remain valid
 * during the lifetime of this surface. The destructor will not free it.
 */
Surface::Surface(SDL_Surface* internal_surface):
  Drawable(),
  internal_surface(internal_surface),
  internal_surface_created(false),
  with_colorkey(false),
  colorkey(0) {

  with_colorkey = SDL_GetColorKey(internal_surface, &colorkey) == 0;
}

/**
 * \brief Creates a surface from an existing surface.
 *
 * Transitions and movements applied on the existing surface are not copied.
 *
 * \param other A surface to copy.
 */
Surface::Surface(const Surface& other):
  Drawable(),
  internal_surface(NULL),
  internal_surface_created(true),
  with_colorkey(false),
  colorkey(0) {

  // For some reason, since SDL2, SDL_ConvertSurface() does not work to copy
  // the surface. Let's copy it more manually.

  internal_surface = SDL_CreateRGBSurface(
      SDL_SWSURFACE,
      other.get_width(),
      other.get_height(),
      SOLARUS_COLOR_DEPTH, 0, 0, 0, 0);
  with_colorkey = SDL_GetColorKey(other.internal_surface, &colorkey) == 0;
  if (with_colorkey) {
    set_transparency_color(other.colorkey);
  }

  SDL_BlitSurface(other.internal_surface, NULL, internal_surface, NULL);
}

/**
 * \brief Destructor.
 */
Surface::~Surface() {

  if (internal_surface_created) {
    SDL_FreeSurface(internal_surface);
  }
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
Surface* Surface::create_from_file(const std::string& file_name,
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
  SDL_Surface* internal_surface = IMG_Load_RW(rw, 0);
  FileTools::data_file_close_buffer(buffer);
  SDL_RWclose(rw);

  if (internal_surface == NULL) {
    // Not a valid image.
    return NULL;
  }

  Surface* surface = new Surface(internal_surface);
  surface->internal_surface_created = true;
  return surface;
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
const Rectangle Surface::get_size() const {

  return Rectangle(0, 0, get_width(), get_height());
}

/**
 * \brief Returns the transparency color of this surface.
 *
 * Pixels in that color will not be drawn.
 * Return black if no colorkey is found.
 *
 * \return The transparency color.
 */
Color Surface::get_transparency_color() const {

  if (with_colorkey) {
    return Color(colorkey);
  }
  
  return Color();
}

/**
 * \brief Sets the transparency color of this surface.
 *
 * Pixels in that color will not be drawn.
 *
 * \param color The transparency color to set.
 */
void Surface::set_transparency_color(const Color& color) {

  with_colorkey = true;
  colorkey = color.get_internal_value();
  SDL_SetColorKey(internal_surface, SDL_TRUE, colorkey);
}

/**
 * \brief Sets the opacity of this surface.
 * \param opacity the opacity (0 to 255).
 */
void Surface::set_opacity(int opacity) {

  // TODO see if SDL2 solves the problem.
  // SDL has a special handling of the alpha value 128
  // which doesn't work well with my computer.
  if (opacity == 128) {
    opacity = 127;
  }

  set_blending_mode(MODE_BLEND);
  SDL_SetSurfaceAlphaMod(internal_surface, opacity);
}

/**
 * \brief Sets the blending mode of this surface.
 * \param the blending mode.
 */
void Surface::set_blending_mode(BlendingMode mode) {
  
  SDL_SetSurfaceBlendMode(internal_surface, SDL_BlendMode(mode));
}

/**
 * \brief Restricts drawing on this surface to a subarea.
 *
 * Sets a subarea of the surface where the next drawings will be restricted to
 * when this surface is used as the destination of blitting.
 * A zero-sized rectangle means that drawings are not restricted to a subarea of the surface.
 * The rectangle specified may be partially outside this rectangle
 * (then it will be resized to fit inside).
 *
 * \param clipping_rectangle a subarea of the rectangle to restrict the drawing to
 */
void Surface::set_clipping_rectangle(const Rectangle& clipping_rectangle) {

  if (clipping_rectangle.get_width() == 0) {
    SDL_SetClipRect(internal_surface, NULL);
  }
  else {
    Rectangle copy = clipping_rectangle;
    SDL_SetClipRect(internal_surface, copy.get_internal_rect());
  }
}

/**
 * \brief Fills the entire surface with the specified color.
 * \param color a color
 */
void Surface::fill_with_color(Color& color) {
  SDL_FillRect(internal_surface, NULL, color.get_internal_value());
}

/**
 * \brief Fills a rectangle of this surface with the specified color.
 * \param color a color
 * \param where the rectangle to fill
 */
void Surface::fill_with_color(Color& color, const Rectangle& where) {
  Rectangle where2 = where;
  SDL_FillRect(internal_surface, where2.get_internal_rect(), color.get_internal_value());
}

/**
 * \brief Draws this surface on another surface.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw(Surface& dst_surface, const Rectangle& dst_position) {

  // Make a copy of the rectangle because SDL_BlitSurface modifies it.
  Rectangle dst_position2(dst_position);
  SDL_BlitSurface(internal_surface, NULL,
      dst_surface.internal_surface, dst_position2.get_internal_rect());
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

  // Make a copy of the rectangle because SDL_BlitSurface modifies it.
  Rectangle region2(region);
  Rectangle dst_position2(dst_position);
  SDL_BlitSurface(internal_surface, region2.get_internal_rect(),
      dst_surface.internal_surface, dst_position2.get_internal_rect());
}

/**
 * \brief Draws a transition effect on this drawable object.
 * \param transition The transition effect to apply.
 */
void Surface::draw_transition(Transition& transition) {
  transition.draw(*this);
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
 * \brief Returns the SDL surface encapsulated by this object.
 *
 * This method should only be used by low-level classes.
 *
 * \return The SDL surface encapsulated.
 */
SDL_Surface* Surface::get_internal_surface() {
  return internal_surface;
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

  uint32_t pixel = 0;
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
 * \brief Returns a pixel value converted into the format of another surface.
 *
 * The source and destination formats can be anything.
 *
 * \param index Index of the pixel to convert.
 * \param dst_format The destination format.
 * \return The converted pixel.
 */
uint32_t Surface::get_converted_pixel(int index, const Surface& dst_surface) const {

  uint32_t pixel = get_pixel(index);
  if (dst_surface.internal_surface->format->format == internal_surface->format->format) {
    // Same format: nothing to do.
    return pixel;
  }

  uint8_t r, g, b, a;
  SDL_GetRGBA(pixel, internal_surface->format, &r, &g, &b, &a);
  return SDL_MapRGBA(dst_surface.internal_surface->format, r, g, b, a);
}

/**
 * \brief Returns whether a pixel is transparent.
 *
 * A pixel is transparent if it corresponds to the colorkey or if its alpha
 * channel is equal to 0.
 *
 * \param index The index of the pixel to test.
 * \return \c true if the pixel is transparent.
 */
bool Surface::is_pixel_transparent(int index) const {
  
  uint32_t pixel = get_pixel(index);
  
  if (with_colorkey && pixel == colorkey) {
    return true;
  }

  if ((pixel & internal_surface->format->Amask) == 0) {
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

