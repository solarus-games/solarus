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
  internal_texture(NULL),
  owns_internal_texture(false),
  internal_opacity(255),
  width(width),
  height(height),
  clipping_rect(Rectangle(0, 0, width, height)) {

  Debug::check_assertion(width > 0 && height > 0,
      "Attempt to create a surface with an empty size");
}

/**
 * \brief Creates a surface with the specified size.
 * \param size The size in pixels.
 */
Surface::Surface(const Rectangle& size):
Drawable(),
  internal_texture(NULL),
  owns_internal_texture(false),
  internal_opacity(255),
  width(size.get_width()),
  height(size.get_height()),
  clipping_rect(Rectangle(0, 0, width, height))  {

  Debug::check_assertion(size.get_width() > 0 && size.get_height() > 0, "Empty surface");
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
  owns_internal_texture(true),
  internal_opacity(255) {

  SDL_Surface* software_surface = get_surface_from_file(file_name, base_directory);
  width = software_surface->w;
  height = software_surface->h;
  clipping_rect = Rectangle(0, 0, width, height);
  internal_texture = get_texture_from_surface(software_surface);
}

/**
 * \brief Creates a surface form the specified SDL texture.
 *
 * This constructor must be used only by lowlevel classes that manipulate directly
 * SDL dependent surfaces.
 *
 * \param internal_texture The internal texture data. It won't be copied.
 * It must remain valid during the lifetime of this surface.
 * The destructor will not free it.
 */
Surface::Surface(SDL_Texture* internal_texture):
  Drawable(),
  internal_texture(internal_texture),
  owns_internal_texture(false),
  internal_opacity(255)
{
  Uint32 format;
  int access;
  SDL_QueryTexture(internal_texture, &format, &access, &width, &height);
  clipping_rect = Rectangle(0, 0, width, height);
}

/**
 * \brief Creates a surface from an existing surface.
 *
 * The internal texture encapsulated is not copied: its ownership is
 * transferred to the new one.
 * Use with care!
 * Transitions and movements applied on the existing surface are not copied.
 *
 * \param other A surface to copy.
 */
Surface::Surface(Surface& other):
  Drawable(),
  internal_texture(other.internal_texture),
  owns_internal_texture(other.owns_internal_texture),
  internal_opacity(255),
  width(other.get_width()),
  height(other.get_height()),
  clipping_rect(other.clipping_rect){

  other.owns_internal_texture = false;
}

/**
 * \brief Destructor.
 */
Surface::~Surface() {

  if (owns_internal_texture) {
    SDL_DestroyTexture(internal_texture);
  }
  for(int i=0 ; i<subsurfaces.size() ; i++)
    delete_subsurface(*subsurfaces.at(i));
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

  SDL_Texture* hardware_surface = get_texture_from_file(file_name, base_directory);

  Surface* surface = new Surface(hardware_surface);
  surface->owns_internal_texture = true;
  return surface;
}

/**
 * \brief Creates a surface from the specified image file name and fill a pixel mask.
 *
 * This function acts like a constructor excepts that it returns NULL if the
 * file does not exist or is not a valid image.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param pixel_mask The pointer to the pointer of the pixel mask to fill.
 * \param base_directory The base directory to use.
 * \return The surface created, or NULL if the file could not be loaded.
 */
Surface* Surface::create_from_file(const std::string& file_name, 
  PixelBits** pixel_mask, ImageDirectory base_directory) {
  
  SDL_Surface* software_surface = get_surface_from_file(file_name, base_directory);
  //*pixel_mask = new PixelBits(software_surface);
  SDL_Texture* hardware_surface = get_texture_from_surface(software_surface);
  
  Surface* surface = new Surface(hardware_surface);
  surface->owns_internal_texture = true;
  return surface;
}

/**
 * \brief Return the SDL_Surface corresponding to the requested file.
 *
 * The returned SDL_Surface have to manually delete.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The SDL_Surface.
 */
SDL_Surface* Surface::get_surface_from_file(const std::string& file_name,
                                            ImageDirectory base_directory)
{
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
 * \brief Return the given SDL_Surface converted to a SDL_Texture.
 *
 * Free the given SDL_Surface.
 *
 * \param software_surface The software surface to convert.
 * \return The SDL_Texture.
 */
SDL_Texture* Surface::get_texture_from_surface(SDL_Surface* software_surface)
{  
  SDL_Texture* hardware_surface = SDL_CreateTextureFromSurface(
    VideoManager::get_instance()->get_renderer(), software_surface);
  SDL_SetTextureBlendMode(hardware_surface, SDL_BLENDMODE_BLEND);
  
  SDL_FreeSurface(software_surface);
  
  return hardware_surface;
}

/**
 * \brief Return the SDL_Texture corresponding to the requested file.
 *
 * The returned SDL_Texture have to manually delete.
 *
 * \param file_name Name of the image file to load, relative to the base directory specified.
 * \param base_directory The base directory to use.
 * \return The SDL_Texture.
 */
SDL_Texture* Surface::get_texture_from_file(const std::string& file_name,
  ImageDirectory base_directory)
{
  SDL_Surface* software_surface = get_surface_from_file(file_name, base_directory);
  
  return get_texture_from_surface(software_surface);
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

  if (clipping_rectangle.is_flat()) {
    clipping_rect = Rectangle(0, 0, width, height);
  }
  else {
    clipping_rect = clipping_rectangle;
  }
}

/**
 * \brief Fills the entire surface with the specified color.
 * \param color a color
 */
void Surface::fill_with_color(Color& color) {
  
  const Rectangle& size = Rectangle(0, 0, width, height);
  fill_with_color(color, size);
}

/**
 * \brief Fills a rectangle of this surface with the specified color.
 * \param color a color
 * \param where the rectangle to fill
 */
void Surface::fill_with_color(Color& color, const Rectangle& where) {

  const int array_size = where.get_width() * where.get_height();
  std::vector<uint32_t> pixels(array_size);  // TODO keep an array of pixels as a field?

  const uint32_t color_value = color.get_internal_value();
  for (int i = 0; i < array_size; ++i) {
    pixels[i] = color_value;
  }

  if (internal_texture == NULL) {
    create_streaming_texture();
  }

  SDL_UpdateTexture(internal_texture,
      where.get_internal_rect(),
      &pixels[0],
      where.get_width() * sizeof(uint32_t));
}

/**
 * \brief Create the internal texture optimized for streaming access.
 */
void Surface::create_streaming_texture()
{
  internal_texture = SDL_CreateTexture(VideoManager::get_instance()->get_renderer(),
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STATIC,
    width, height);
  SDL_SetTextureBlendMode(internal_texture, SDL_BLENDMODE_BLEND);
  
  owns_internal_texture = true;
}

/**
 * \brief Add a SubSurface to draw on the vector buffer.
 * \param subsurface The SubSurface to add on the buffer queue.
 */
void Surface::add_subsurface(SubSurface& subsurface) {
  
  subsurfaces.push_back(&subsurface);
}

/**
 * \brief Delete a SubSurface.
 * \param subsurface The SubSurface to delete.
 */
void Surface::delete_subsurface(SubSurface& subsurface)
{
  delete &subsurface;
}

/**
 * \brief Draws this surface on another surface.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void Surface::raw_draw(Surface& dst_surface, const Rectangle& dst_position) {

  Rectangle region = Rectangle(0, 0, width, height);
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

  SubSurface* subsurface = new SubSurface();
  subsurface->surface = this;
  subsurface->src_rect = region;
  subsurface->dst_rect = dst_position;
  
  if(subsurface->dst_rect.is_flat())
  {
    subsurface->dst_rect.set_width(region.get_width());
    subsurface->dst_rect.set_height(region.get_height());
  }
  
  dst_surface.add_subsurface(*subsurface);
}

/**
 * \brief Draws a transition effect on this drawable object.
 * \param transition The transition effect to apply.
 */
void Surface::draw_transition(Transition& transition) {
  transition.draw(*this);
}

/**
 * \brief Draw the internal texture if any, and all subtextures on the renderer.
 *
 * Empty the subsurfaces vector at the end of the method.
 * \param renderer The renderer where to draw.
 * \param src_rect The subrectangle of the texture to draw.
 * \param dst_rect The portion of renderer where to draw.
 * \param opacity The opacity of the parent surface.
 */
void Surface::render(SDL_Renderer* renderer, Rectangle& src_rect, Rectangle& dst_rect, int opacity) {
  
  int current_opacity = std::min(internal_opacity, opacity);
  
  // Draw the internal texture.
  if(internal_texture)
  {
    // Calculate absolute clipping rectangle position.
    Rectangle final_clipping_rect = Rectangle(
      dst_rect.get_x() + clipping_rect.get_x(),
      dst_rect.get_y() + clipping_rect.get_y(),
      clipping_rect.get_width(),
      clipping_rect.get_height());
    
    SDL_SetTextureAlphaMod(internal_texture, current_opacity);
    SDL_RenderSetClipRect(renderer, final_clipping_rect.get_internal_rect());
    SDL_RenderCopy(renderer, internal_texture, src_rect.get_internal_rect(), dst_rect.get_internal_rect());
  }
  
  // Draw all subtextures.
  for(int i=0 ; i<subsurfaces.size() ; i++)
  {
    // Calculate absolute destination subrectangle position.
    Rectangle dst_subrect = Rectangle(
      dst_rect.get_x() + subsurfaces.at(i)->dst_rect.get_x(),
      dst_rect.get_y() + subsurfaces.at(i)->dst_rect.get_y(),
      subsurfaces.at(i)->dst_rect.get_width(),
      subsurfaces.at(i)->dst_rect.get_height());
      
    subsurfaces.at(i)->surface->render(renderer, subsurfaces.at(i)->src_rect, dst_subrect, current_opacity);
    delete_subsurface(*subsurfaces.at(i));
  }
  
  subsurfaces.clear();
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
 * \brief Returns the SDL texture encapsulated by this object.
 *
 * This method should only be used by low-level classes.
 *
 * \return The SDL texture encapsulated.
 */
SDL_Texture* Surface::get_internal_texture() {
  return internal_texture;
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

  /*SDL_PixelFormat* format = internal_surface->format;

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

  Debug::die(StringConcat() << "Unknown pixel depth: " << format->BitsPerPixel);*/
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
  
  /*uint32_t pixel = get_pixel(index);
  uint32_t colorkey;
  bool with_colorkey = false;
  // TODO with_colorkey = SDL_GetColorKey() == 0;
  
  if (with_colorkey && pixel == colorkey) {
    // The pixel has the transparency color.
    return true;
  }

  if (internal_surface->format->Amask != 0               // There exists an alpha channel.
      && (pixel & internal_surface->format->Amask) == 0  // The pixel is fully transparent.
      ) {
    return true;
  }*/
  
  return false;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& Surface::get_lua_type_name() const {
  return LuaContext::surface_module_name;
}

