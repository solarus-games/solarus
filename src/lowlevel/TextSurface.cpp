/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/FileTools.h"
#include "solarus/lowlevel/FontResource.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lowlevel/TextSurface.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/Transition.h"
#include <lua.hpp>

namespace Solarus {

/**
 * \brief Creates a text to draw with the default properties.
 *
 * The default properties are:
 * - font: the default font defined in file text/fonts.dat
 * - horizontal alignment: left
 * - vertical alignment: middle
 * - rendering mode: solid
 * - text color: white
 *
 * \param x x position of the text on the destination surface
 * \param y y position of the text on the destination surface
 */
TextSurface::TextSurface(int x, int y):
    TextSurface(x, y, ALIGN_LEFT, ALIGN_MIDDLE) {
}

/**
 * \brief Creates a text to draw with the specified alignment properties.
 * \param x x position of the text on the destination surface
 * \param y y position of the text on the destination surface
 * \param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * \param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
TextSurface::TextSurface(int x, int y,
    TextSurface::HorizontalAlignment horizontal_alignment,
    TextSurface::VerticalAlignment vertical_alignment):
  Drawable(),
  font_id(FontResource::get_default_font_id()),
  horizontal_alignment(horizontal_alignment),
  vertical_alignment(vertical_alignment),
  rendering_mode(TEXT_SOLID),
  text_color(Color::white),
  font_size(11),
  x(x),
  y(y),
  surface(nullptr),
  text() {

}

/**
 * \brief Returns the id of the font used to draw this text.
 * \return Id of a font.
 */
const std::string& TextSurface::get_font() const {
  return font_id;
}

/**
 * \brief Sets the font to use.
 * \param font_id Id of a font.
 */
void TextSurface::set_font(const std::string& font_id) {

  if (font_id == this->font_id) {
    return;
  }

  this->font_id = font_id;
  rebuild();
}

/**
 * \brief Returns the horizontal text alignment.
 * \return Horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT.
 */
TextSurface::HorizontalAlignment TextSurface::get_horizontal_alignment() const {
  return horizontal_alignment;
}

/**
 * \brief Sets the horizontal text alignment.
 * \param horizontal_alignment Horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT.
 */
void TextSurface::set_horizontal_alignment(HorizontalAlignment horizontal_alignment) {

  if (horizontal_alignment == this->horizontal_alignment) {
    return;
  }

  this->horizontal_alignment = horizontal_alignment;

  rebuild();
}

/**
 * \brief Returns the vertical text alignment.
 * \return Vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM.
 */
TextSurface::VerticalAlignment TextSurface::get_vertical_alignment() const {
  return vertical_alignment;
}

/**
 * \brief Sets the vertical text alignment.
 * \param vertical_alignment Vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM.
 */
void TextSurface::set_vertical_alignment(VerticalAlignment vertical_alignment) {

  if (vertical_alignment == this->vertical_alignment) {
    return;
  }

  this->vertical_alignment = vertical_alignment;

  rebuild();
}

/**
 * \brief Sets the text alignment.
 * \param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * \param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
void TextSurface::set_alignment(HorizontalAlignment horizontal_alignment,
    VerticalAlignment vertical_alignment) {

  if (horizontal_alignment == this->horizontal_alignment &&
      vertical_alignment == this->vertical_alignment) {
    return;
  }

  this->horizontal_alignment = horizontal_alignment;
  this->vertical_alignment = vertical_alignment;

  rebuild();
}

/**
 * \brief Returns the rendering mode of the text.
 * \return The rendering mode: TEXT_SOLID or TEXT_ANTIALIASING.
 */
TextSurface::RenderingMode TextSurface::get_rendering_mode() const {
  return rendering_mode;
}

/**
 * \brief Sets the rendering mode of the text.
 * \param rendering_mode Rendering mode: TEXT_SOLID or TEXT_ANTIALIASING.
 */
void TextSurface::set_rendering_mode(TextSurface::RenderingMode rendering_mode) {

  if (rendering_mode == this->rendering_mode) {
    return;
  }

  this->rendering_mode = rendering_mode;
  rebuild();
}

/**
 * \brief Returns the color of the text.
 * \return The text color.
 */
const Color& TextSurface::get_text_color() const {
  return text_color;
}

/**
 * \brief Sets the color of the text.
 * \param color The color to set.
 */
void TextSurface::set_text_color(const Color& color) {

  if (color == this->text_color) {
    return;
  }

  this->text_color = color;
  rebuild();
}

/**
 * \brief Returns the font size.
 * \return The font size.
 */
int TextSurface::get_font_size() const {
  return font_size;
}

/**
 * \brief Sets the font size.
 * \param font_size The font size.
 */
void TextSurface::set_font_size(int font_size) {

  if (font_size == this->font_size) {
    return;
  }

  this->font_size = font_size;
  rebuild();
}

/**
 * \brief Sets the position of the text on the destination surface.
 * \param x X position of the text on the destination surface.
 * \param y Y position of the text on the destination surface.
 */
void TextSurface::set_position(int x, int y) {

  if (x == this->x && y == this->y) {
    return;
  }

  this->x = x;
  this->y = y;
  rebuild();
}

/**
 * \brief Returns the x position of the text on the destination surface.
 * \return X position of the text.
 */
int TextSurface::get_x() const {
  return x;
}

/**
 * \brief Sets the x position of the text on the destination surface.
 * \param x x position of the text
 */
void TextSurface::set_x(int x) {

  if (x == this->x) {
    return;
  }

  this->x = x;
  rebuild();
}

/**
 * \brief Returns the y position of the text on the destination surface.
 * \return y position of the text.
 */
int TextSurface::get_y() const {
  return y;
}

/**
 * \brief Sets the y position of the text on the destination surface.
 * \param y y position of the text
 */
void TextSurface::set_y(int y) {

  if (y == this->y) {
    return;
  }

  this->y = y;
  rebuild();
}

/**
 * \brief Returns whether the current text is an empty string.
 *
 * A string with only whitespaces characters is considered empty.
 *
 * \return \c true if there is no text.
 */
bool TextSurface::is_empty() const {

  std::string whitespaces = " \t\n\r";
  return text.find_first_not_of(whitespaces) == std::string::npos;
}

/**
 * \brief Returns the text currently displayed.
 * \return the text currently displayed, or nullptr if there is no text
 */
const std::string& TextSurface::get_text() const {
  return text;
}

/**
 * \brief Sets the string drawn.
 *
 * If the specified string is the same than the current text, nothing is done.
 *
 * \param text the text to display (cannot be nullptr)
 */
void TextSurface::set_text(const std::string& text) {

  if (text == this->text) {
    return;
  }

  this->text = text;
  rebuild();
}

/**
 * \brief Adds a character to the string drawn.
 *
 * This is equivalent to set_text(get_text() + c).
 *
 * \param c the character to add
 */
void TextSurface::add_char(char c) {
  set_text(text + c);
}

/**
 * \brief Returns the width of the surface containing the text.
 * \return the width in pixels
 */
int TextSurface::get_width() const {

  if (surface == nullptr) {
    return 0;
  }

  return surface->get_width();
}

/**
 * \brief Returns the height of the surface containing the text.
 * \return the height in pixels
 */
int TextSurface::get_height() const {

  if (surface == nullptr) {
    return 0;
  }

  return surface->get_height();
}

/**
 * \brief Returns the size of the surface containing the text.
 * \return the size of the surface
 */
Size TextSurface::get_size() const {
  return { get_width(), get_height() };
}

/**
 * \brief Redraws the text surface.
 *
 * This function is called when there is a change.
 */
void TextSurface::rebuild() {

  surface = nullptr;

  if (font_id.empty()) {
    return;
  }

  if (is_empty()) {
    // Empty string or only whitespaces: no surface to create.
    // Some fonts make TTF_Font fail if the string contains only whitespaces.
    return;
  }

  Debug::check_assertion(FontResource::exists(font_id),
      std::string("No such font: '") + font_id + "'"
  );

  if (FontResource::is_bitmap_font(font_id)) {
    rebuild_bitmap();
  }
  else {
    rebuild_ttf();
  }

  // calculate the coordinates of the top-left corner
  int x_left = 0, y_top = 0;

  switch (horizontal_alignment) {

  case ALIGN_LEFT:
    x_left = x;
    break;

  case ALIGN_CENTER:
    x_left = x - surface->get_width() / 2;
    break;

  case ALIGN_RIGHT:
    x_left = x - surface->get_width();
    break;
  }

  switch (vertical_alignment) {

  case ALIGN_TOP:
    y_top = y;
    break;

  case ALIGN_MIDDLE:
    y_top = y - surface->get_height() / 2;
    break;

  case ALIGN_BOTTOM:
    y_top = y - surface->get_height();
    break;
  }

  text_position = { x_left, y_top };
}

/**
 * \brief Redraws the text surface in the case of a bitmap font.
 *
 * This function is called when there is a change.
 */
void TextSurface::rebuild_bitmap() {

  // First count the number of characters in the UTF-8 string.
  int num_chars = 0;
  for (unsigned i = 0; i < text.size(); i++) {
    char current_char = text[i];
    if ((current_char & 0xE0) == 0xC0) {
      // This character uses two bytes.
      ++i;
    }
    ++num_chars;
  }

  // Determine the letter size from the surface size.
  const SurfacePtr& bitmap = FontResource::get_bitmap_font(font_id);
  const Size& bitmap_size = bitmap->get_size();
  int char_width = bitmap_size.width / 128;
  int char_height = bitmap_size.height / 16;

  surface = Surface::create((char_width - 1) * num_chars + 1, char_height);

  // Traverse the string again to draw the characters.
  Point dst_position;
  for (unsigned i = 0; i < text.size(); i++) {
    char first_byte = text[i];
    Rectangle src_position(0, 0, char_width, char_height);
    if ((first_byte & 0xE0) != 0xC0) {
      // This character uses one byte.
      src_position.set_xy(first_byte * char_width, 0);
    }
    else {
      // This character uses two bytes.
      ++i;
      char second_byte = text[i];
      uint16_t code_point = ((first_byte & 0x1F) << 6) | (second_byte & 0x3F);
      src_position.set_xy((code_point % 128) * char_width,
          (code_point / 128) * char_height);
    }
    bitmap->draw_region(src_position, surface, dst_position);
    dst_position.x += char_width - 1;
  }
}

/**
 * \brief Redraws the text surface in the case of a normal font.
 *
 * This function is called when there is a change.
 */
void TextSurface::rebuild_ttf() {

  // create the text surface

  SDL_Surface* internal_surface = nullptr;
  TTF_Font& internal_font = FontResource::get_outline_font(font_id, font_size);
  SDL_Color internal_color;
  text_color.get_components(
      internal_color.r, internal_color.g, internal_color.b, internal_color.a);

  switch (rendering_mode) {

  case TEXT_SOLID:
    internal_surface = TTF_RenderUTF8_Solid(&internal_font, text.c_str(), internal_color);
    break;

  case TEXT_ANTIALIASING:
    internal_surface = TTF_RenderUTF8_Blended(&internal_font, text.c_str(), internal_color);
    break;
  }

  Debug::check_assertion(internal_surface != nullptr,
      std::string("Cannot create the text surface for string '") + text + "': "
      + SDL_GetError()
  );

  surface = std::make_shared<Surface>(internal_surface);
}

/**
 * \brief Draws the text on a surface.
 *
 * This method blits the text at the position previously set
 * (by set_x(), set_y() or set_position())
 * plus the parameter dst_position.
 *
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void TextSurface::raw_draw(Surface& dst_surface,
    const Point& dst_position) {

  if (surface != nullptr) {
    surface->raw_draw(dst_surface, dst_position + text_position);
  }
}

/**
 * \brief Draws a subrectangle of this text surface on another surface.
 * \param region The subrectangle to draw in this object.
 * \param dst_surface The destination surface.
 * \param dst_position Coordinates on the destination surface.
 */
void TextSurface::raw_draw_region(const Rectangle& region,
    Surface& dst_surface, const Point& dst_position) {

  if (surface != nullptr) {
    surface->raw_draw_region(
        region, dst_surface,
        dst_position + text_position);
  }
}

/**
 * \brief Draws a transition effect on this drawable object.
 * \param transition The transition effect to apply.
 */
void TextSurface::draw_transition(Transition& transition) {
  transition.draw(*surface);
}

/**
 * \brief Returns the surface where transitions on this drawable object
 * are applied.
 * \return The surface for transitions.
 */
Surface& TextSurface::get_transition_surface() {
  return *surface;
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return the name identifying this type in Lua
 */
const std::string& TextSurface::get_lua_type_name() const {
  return LuaContext::text_surface_module_name;
}

}

