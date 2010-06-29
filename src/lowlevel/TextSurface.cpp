/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lowlevel/IniFile.h"

std::map<std::string, TextSurface::FontData> TextSurface::fonts;
std::string TextSurface::default_font_id = "";

/**
 * @brief Initializes the font system.
 */
void TextSurface::initialize(void) {

  TTF_Init();

  // first determine the available fonts
  IniFile ini("text/fonts.dat", IniFile::READ);
  for (ini.start_group_iteration(); ini.has_more_groups(); ini.next_group()) {

    // get the font metadata
    std::string font_id = ini.get_group();
    std::string file_name = ini.get_string_value("file", "");
    if (file_name.size() == 0) {
      DIE("Missing font file name in file 'text/fonts.dat' for group '" << font_id << "'");
    }
    int font_size = ini.get_integer_value("size", 0);
    if (font_size == 0) {
      DIE("Missing font size in file 'text/fonts.dat' for group '" << font_id << "'");
    }
    fonts[font_id].file_name = file_name;
    fonts[font_id].font_size = font_size;

    if (ini.get_boolean_value("default", false)) {
      default_font_id = font_id;
    }

    // load the font
    size_t size;
    FileTools::data_file_open_buffer(file_name, &fonts[font_id].buffer, &size);
    fonts[font_id].rw = SDL_RWFromMem(fonts[font_id].buffer, size);
    fonts[font_id].internal_font = TTF_OpenFontRW(fonts[font_id].rw, 0, font_size);
    if (fonts[font_id].internal_font == NULL) {
      DIE("Cannot load font from file '" << file_name << "': " << TTF_GetError());
    }
  }

  if (default_font_id.size() == 0) {
    DIE("No default font set in file 'text/fonts.dat'");
  }
}

/**
 * @brief Closes the font system.
 */
void TextSurface::quit(void) {

  std::map<std::string, FontData>::iterator it;
  for (it = fonts.begin(); it != fonts.end(); it++) {
    std::string font_id = it->first;
    FontData *font = &it->second;
    TTF_CloseFont(font->internal_font);
    SDL_RWclose(font->rw);
    FileTools::data_file_close_buffer(font->buffer);
  }

  TTF_Quit();
}

/**
 * @brief Creates a text to display with the default properties.
 *
 * The default properties are:
 * - font: the default font defined in file text/fonts.dat
 * - horizontal alignment: left
 * - vertical alignment: middle
 * - rendering mode: solid
 * - text color: white
 * - background color: black
 *
 * @param x x position of the text on the destination surface
 * @param y y position of the text on the destination surface
 */
TextSurface::TextSurface(int x, int y):
  font_id(default_font_id),
  horizontal_alignment(ALIGN_LEFT),
  vertical_alignment(ALIGN_MIDDLE),
  rendering_mode(TEXT_SOLID),
  surface(NULL) {

  text = "";
  set_text_color(Color::get_white());
  set_background_color(Color::get_black());
  set_position(x, y);
}

/**
 * @brief Creates a text to display with the specified alignment properties.
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

  font_id(default_font_id),
  horizontal_alignment(horizontal_alignment),
  vertical_alignment(vertical_alignment),
  rendering_mode(TEXT_SOLID),
  surface(NULL) {

  text = "";
  set_text_color(Color::get_white());
  set_background_color(Color::get_black());
  set_position(x, y);
}

/**
 * @brief Destructor.
 */
TextSurface::~TextSurface(void) {

  if (surface != NULL) {
    SDL_FreeSurface(surface->get_internal_surface());
  }
  delete surface;
}

/**
 * @brief Sets the font to use.
 * @param font_id a font
 */
void TextSurface::set_font(const std::string font_id) {
  this->font_id = font_id;
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
 * @brief Sets the rendering mode of the text.
 * @param rendering_mode rendering mode: TEXT_SOLID, TEXT_SHADED or TEXT_BLENDED
 */
void TextSurface::set_rendering_mode(TextSurface::RenderingMode rendering_mode) {
  this->rendering_mode = rendering_mode;

  rebuild();
}

/**
 * @brief Sets the color of the text.
 * @param color the color to set
 */
void TextSurface::set_text_color(Color &color) {
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
 * @brief Sets the background color of the text.
 *
 * This is only useful for the TEXT_SHADED rendering.
 *
 * @param color the background color to set
 */
void TextSurface::set_background_color(Color &color) {
  this->background_color = color;
  rebuild();
}

/**
 * @brief Sets the position of the text on the destination surface.
 * @param x x position of the text on the destination surface
 * @param y y position of the text on the destination surface
 */
void TextSurface::set_position(int x, int y) {
  this->x = x;
  this->y = y;
  rebuild();
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
 * @brief Sets the y position of the text on the destination surface.
 * @param y y position of the text
 */
void TextSurface::set_y(int y) {
  this->y = y;
  rebuild();
}

/**
 * @brief Sets the string drawn.
 *
 * If the specified string is the same than the current text, nothing is done.
 *
 * @param text the text to display (cannot be NULL)
 */
void TextSurface::set_text(const std::string &text) {

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
 * @brief Returns the text currently displayed.
 * @return the text currently displayed, or NULL if there is no text
 */
const std::string & TextSurface::get_text(void) {
  return text;
}

/**
 * @brief Returns whether the current text is an empty string.
 * @return true if there is no texte
 */
bool TextSurface::is_empty(void) {
  return text == "";
}

/**
 * @brief Returns the width of the surface containing the text.
 * @return the width in pixels
 */
int TextSurface::get_width(void) {
  return surface->get_width();
}

/**
 * @brief Returns the height of the surface containing the text.
 * @return the height in pixels
 */
int TextSurface::get_height(void) {
  return surface->get_height();
}


/**
 * @brief Returns the size of the surface containing the text.
 * @return the size of the surface
 */
const Rectangle TextSurface::get_size(void) {
  return Rectangle(0, 0, get_width(), get_height());
}

/**
 * @brief Creates the text surface.
 *
 * This function is called when there is a change.
 */
void TextSurface::rebuild(void) {

  if (surface != NULL) {
    // another text was previously set: delete it
    SDL_FreeSurface(surface->get_internal_surface());
    delete surface;
    surface = NULL;
  }

  if (is_empty()) {
    // empty string: no surface to create
    return;
  }

  // create the text surface

  SDL_Surface *internal_surface = NULL;
  switch (rendering_mode) {

  case TEXT_SOLID:
    internal_surface = TTF_RenderUTF8_Solid(fonts[font_id].internal_font, text.c_str(), *text_color.get_internal_color());
    break;

  case TEXT_SHADED:
    internal_surface = TTF_RenderUTF8_Shaded(fonts[font_id].internal_font, text.c_str(), *text_color.get_internal_color(),
	*background_color.get_internal_color());
    break;

  case TEXT_BLENDED:
    internal_surface = TTF_RenderUTF8_Blended(fonts[font_id].internal_font, text.c_str(), *text_color.get_internal_color());
    break;
  }

  if (internal_surface == NULL) {
    DIE("Cannot create the text surface for string '" << text << "': " << SDL_GetError());
  }
  surface = new Surface(internal_surface);

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
 * @brief Displays the text on a surface.
 *
 * This method just blits on the given surface the text surface created
 * when you called set_text().
 *
 * @param destination the destination surface
 */
void TextSurface::display(Surface *destination) {

  if (surface != NULL) {
    surface->blit(destination, text_position);
  }
}

