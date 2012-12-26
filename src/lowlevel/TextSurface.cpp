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
#include "lowlevel/TextSurface.h"
#include "lowlevel/Surface.h"
#include "lowlevel/System.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lua/LuaContext.h"
#include "Transition.h"
#include <lua.hpp>

std::map<std::string, TextSurface::FontData> TextSurface::fonts;
std::string TextSurface::default_font_id;

/**
 * @brief Initializes the font system.
 */
void TextSurface::initialize() {

  TTF_Init();

  // Load the list of available fonts.
  static const std::string file_name = "text/fonts.dat";

  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  lua_register(l, "font", l_font);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(StringConcat() << "Failed to load the fonts file '"
        << file_name << "': " << lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  lua_close(l);
}

/**
 * @brief Closes the font system.
 */
void TextSurface::quit() {

  std::map<std::string, FontData>::iterator it;
  for (it = fonts.begin(); it != fonts.end(); it++) {
    std::string font_id = it->first;
    FontData *font = &it->second;

    if (font->bitmap != NULL) {
      // It's a bitmap font.
      delete font->bitmap;
      font->bitmap = NULL;
    }
    else {
      // It's a normal font.
      TTF_CloseFont(font->internal_font);
      SDL_RWclose(font->rw);
      FileTools::data_file_close_buffer(font->buffer);
    }
  }

  TTF_Quit();
}

/**
 * @brief Function called by the Lua data file to add a font.
 *
 * - Argument 1 (table): properties of the font.
 *
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int TextSurface::l_font(lua_State* l) {

  luaL_checktype(l, 1, LUA_TTABLE);

  const std::string& font_id = LuaContext::check_string_field(l, 1, "id");
  const std::string& file_name = LuaContext::check_string_field(l, 1, "file");
  int font_size = LuaContext::opt_int_field(l, 1, "size", 11);
  bool is_default = LuaContext::opt_boolean_field(l, 1, "default", false);

  fonts[font_id].file_name = file_name;
  fonts[font_id].font_size = font_size;

  if (is_default || default_font_id.empty()) {
    default_font_id = font_id;
  }

  // Load the font.
  size_t index = file_name.rfind('.');
  std::string extension;
  if (index != std::string::npos) {
    extension = file_name.substr(index);
  }

  if (extension == ".png" || extension == ".PNG") {
    // It's a bitmap font.
    fonts[font_id].bitmap = new Surface(file_name, Surface::DIR_DATA);
  }
  else {
    // It's a normal font.
    size_t size;
    FileTools::data_file_open_buffer(file_name, &fonts[font_id].buffer, &size);
    fonts[font_id].rw = SDL_RWFromMem(fonts[font_id].buffer, size);
    fonts[font_id].internal_font = TTF_OpenFontRW(fonts[font_id].rw, 0, font_size);
    Debug::check_assertion(fonts[font_id].internal_font != NULL,
        StringConcat() << "Cannot load font from file '" << file_name << "': " << TTF_GetError());
  }

  return 0;
}

/**
 * @brief Creates a text to draw with the default properties.
 *
 * The default properties are:
 * - font: the default font defined in file text/fonts.dat
 * - horizontal alignment: left
 * - vertical alignment: middle
 * - rendering mode: solid
 * - text color: white
 *
 * @param x x position of the text on the destination surface
 * @param y y position of the text on the destination surface
 */
TextSurface::TextSurface(int x, int y):
  Drawable(),
  font_id(default_font_id),
  horizontal_alignment(ALIGN_LEFT),
  vertical_alignment(ALIGN_MIDDLE),
  rendering_mode(TEXT_SOLID),
  surface(NULL) {

  text = "";
  set_text_color(Color::get_white());
  set_position(x, y);
}

/**
 * @brief Creates a text to draw with the specified alignment properties.
 * @param x x position of the text on the destination surface
 * @param y y position of the text on the destination surface
 * @param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * @param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
TextSurface::TextSurface(int x, int y,
			 TextSurface::HorizontalAlignment horizontal_alignment,
			 TextSurface::VerticalAlignment vertical_alignment):
  Drawable(),
  font_id(default_font_id),
  horizontal_alignment(horizontal_alignment),
  vertical_alignment(vertical_alignment),
  rendering_mode(TEXT_SOLID),
  surface(NULL) {

  text = "";
  set_text_color(Color::get_white());
  set_position(x, y);
}

/**
 * @brief Destructor.
 */
TextSurface::~TextSurface() {

  if (surface != NULL && !surface->internal_surface_created) {
    SDL_FreeSurface(surface->get_internal_surface());
  }
  delete surface;
}

/**
 * @brief Returns whether a font exists.
 * @param font_id Id of a font.
 * @return true if this font exists.
 */
bool TextSurface::has_font(const std::string& font_id) {

  return fonts.find(font_id) != fonts.end();
}

/**
 * @brief Returns the font used to draw this text.
 * @return Id of a font.
 */
const std::string& TextSurface::get_font() {
  return font_id;
}

/**
 * @brief Sets the font to use.
 * @param font_id Id of a font.
 */
void TextSurface::set_font(const std::string& font_id) {

  Debug::check_assertion(has_font(font_id), StringConcat() <<
      "No such font: '" << font_id << "'");
  this->font_id = font_id;
  rebuild();
}

/**
 * @brief Returns the horizontal text alignment.
 * @return Horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT.
 */
TextSurface::HorizontalAlignment TextSurface::get_horizontal_alignment() {
  return horizontal_alignment;
}

/**
 * @brief Sets the horizontal text alignment.
 * @param horizontal_alignment Horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT.
 */
void TextSurface::set_horizontal_alignment(HorizontalAlignment horizontal_alignment) {

  this->horizontal_alignment = horizontal_alignment;

  rebuild();
}

/**
 * @brief Returns the vertical text alignment.
 * @return Vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM.
 */
TextSurface::VerticalAlignment TextSurface::get_vertical_alignment() {
  return vertical_alignment;
}

/**
 * @brief Sets the vertical text alignment.
 * @param vertical_alignment Vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM.
 */
void TextSurface::set_vertical_alignment(VerticalAlignment vertical_alignment) {

  this->vertical_alignment = vertical_alignment;

  rebuild();
}

/**
 * @brief Sets the text alignment.
 * @param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * @param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
void TextSurface::set_alignment(HorizontalAlignment horizontal_alignment,
				  VerticalAlignment vertical_alignment) {
  this->horizontal_alignment = horizontal_alignment;
  this->vertical_alignment = vertical_alignment;

  rebuild();
}

/**
 * @brief Returns the rendering mode of the text.
 * @return The rendering mode: TEXT_SOLID or TEXT_ANTIALIASING.
 */
TextSurface::RenderingMode TextSurface::get_rendering_mode() {
  return rendering_mode;
}

/**
 * @brief Sets the rendering mode of the text.
 * @param rendering_mode Rendering mode: TEXT_SOLID or TEXT_ANTIALIASING.
 */
void TextSurface::set_rendering_mode(TextSurface::RenderingMode rendering_mode) {

  this->rendering_mode = rendering_mode;
  rebuild();
}

/**
 * @brief Returns the color of the text.
 * @return The text color.
 */
const Color& TextSurface::get_text_color() {
  return text_color;
}

/**
 * @brief Sets the color of the text.
 * @param color The color to set.
 */
void TextSurface::set_text_color(const Color &color) {
  this->text_color = color;
  rebuild();
}

/**
 * @brief Sets the color of the text.
 * @param r red component (0 to 255)
 * @param g green component (0 to 255)
 * @param b blue component (0 to 255)
 */
void TextSurface::set_text_color(int r, int g, int b) {
  this->text_color = Color(r, g, b);
  rebuild();
}

/**
 * @brief Sets the position of the text on the destination surface.
 * @param x X position of the text on the destination surface.
 * @param y Y position of the text on the destination surface.
 */
void TextSurface::set_position(int x, int y) {
  this->x = x;
  this->y = y;
  rebuild();
}

/**
 * @brief Returns the x position of the text on the destination surface.
 * @return X position of the text.
 */
int TextSurface::get_x() {
  return x;
}

/**
 * @brief Sets the x position of the text on the destination surface.
 * @param x x position of the text
 */
void TextSurface::set_x(int x) {
  this->x = x;
  rebuild();
}

/**
 * @brief Returns the y position of the text on the destination surface.
 * @return y position of the text.
 */
int TextSurface::get_y() {
  return y;
}

/**
 * @brief Sets the y position of the text on the destination surface.
 * @param y y position of the text
 */
void TextSurface::set_y(int y) {
  this->y = y;
  rebuild();
}

/**
 * @brief Returns whether the current text is an empty string.
 * @return true if there is no texte
 */
bool TextSurface::is_empty() {
  return text.empty();
}

/**
 * @brief Returns the text currently displayed.
 * @return the text currently displayed, or NULL if there is no text
 */
const std::string& TextSurface::get_text() {
  return text;
}

/**
 * @brief Sets the string drawn.
 *
 * If the specified string is the same than the current text, nothing is done.
 *
 * @param text the text to display (cannot be NULL)
 */
void TextSurface::set_text(const std::string& text) {

  if (text != this->text) {

    // there is a change
    this->text = text;
    rebuild();
  }
}

/**
 * @brief Adds a character to the string drawn.
 *
 * This is equivalent to set_text(get_text() + c).
 *
 * @param c the character to add
 */
void TextSurface::add_char(char c) {
  set_text(text + c);
}

/**
 * @brief Returns the width of the surface containing the text.
 * @return the width in pixels
 */
int TextSurface::get_width() {
  return surface->get_width();
}

/**
 * @brief Returns the height of the surface containing the text.
 * @return the height in pixels
 */
int TextSurface::get_height() {
  return surface->get_height();
}


/**
 * @brief Returns the size of the surface containing the text.
 * @return the size of the surface
 */
const Rectangle TextSurface::get_size() {
  return Rectangle(0, 0, get_width(), get_height());
}

/**
 * @brief Redraws the text surface.
 *
 * This function is called when there is a change.
 */
void TextSurface::rebuild() {

  if (surface != NULL) {
    // another text was previously set: delete it
    if (!surface->internal_surface_created) {
      SDL_FreeSurface(surface->get_internal_surface());
    }
    delete surface;
    surface = NULL;
  }

  if (is_empty()) {
    // empty string: no surface to create
    return;
  }

  if (fonts[font_id].bitmap) {
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

  text_position.set_xy(x_left, y_top);
}

/**
 * @brief Redraws the text surface in the case of a bitmap font.
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
  Surface& bitmap = *fonts[font_id].bitmap;
  const Rectangle& bitmap_size = bitmap.get_size();
  int char_width = bitmap_size.get_width() / 128;
  int char_height = bitmap_size.get_height() / 16;

  surface = new Surface(char_width * num_chars, char_height);
  surface->set_transparency_color(bitmap.get_transparency_color());

  // Traverse the string again to draw the characters.
  Rectangle dst_position(0, 0);
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
    bitmap.draw_region(src_position, *surface, dst_position);
    dst_position.add_x(char_width - 1);
  }
}

/**
 * @brief Redraws the text surface in the case of a normal font.
 *
 * This function is called when there is a change.
 */
void TextSurface::rebuild_ttf() {

  // create the text surface

  SDL_Surface *internal_surface = NULL;
  switch (rendering_mode) {

  case TEXT_SOLID:
    internal_surface = TTF_RenderUTF8_Solid(fonts[font_id].internal_font, text.c_str(), *text_color.get_internal_color());
    break;

  case TEXT_ANTIALIASING:
    internal_surface = TTF_RenderUTF8_Blended(fonts[font_id].internal_font, text.c_str(), *text_color.get_internal_color());
    break;
  }

  Debug::check_assertion(internal_surface != NULL, StringConcat()
      << "Cannot create the text surface for string '" << text << "': " << SDL_GetError());
  surface = new Surface(internal_surface);
}

/**
 * @brief Draws the text on a surface.
 *
 * This method blits the text at the position previously set
 * (by set_x(), set_y() or set_position())
 * plus the parameter dst_position.
 *
 * @param dst_surface the destination surface
 * @param dst_position coordinates on the destination surface
 */
void TextSurface::raw_draw(Surface& dst_surface,
    const Rectangle& dst_position) {

  if (surface != NULL) {

    Rectangle dst_position2(text_position);
    dst_position2.add_xy(dst_position);
    surface->raw_draw(dst_surface, dst_position2);
  }
}

/**
 * @brief Draws a transition effect on this drawable object.
 * @param transition The transition effect to apply.
 */
void TextSurface::draw_transition(Transition& transition) {
  transition.draw(*surface);
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return the name identifying this type in Lua
 */
const std::string& TextSurface::get_lua_type_name() const {
  return LuaContext::text_surface_module_name;
}

