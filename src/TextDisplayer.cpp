/**
 * This module defines the class TextDisplayer.
 */
#include "TextDisplayer.h"
#include "FileTools.h"

/**
 * Constructor.
 */
TextDisplayer::TextDisplayer(void):
horizontal_alignment(ALIGN_LEFT), vertical_alignment(ALIGN_MIDDLE),
rendering_mode(TEXT_SOLID) {

  text_color.r = 255;
  text_color.g = 255;
  text_color.b = 255;

  background_color.r = 0;
  background_color.g = 0;
  background_color.b = 0;

  TTF_Init();
  font = TTF_OpenFont(FileTools::data_file_add_prefix("zsdx.ttf"), 11);
  if (font == NULL) {
    cerr << "Cannot load font 'zsdx.ttf'." << endl;
    exit(1);
  } 
}

/**
 * Destructor.
 */
TextDisplayer::~TextDisplayer(void) {
  TTF_CloseFont(font);
  TTF_Quit();
}

/**
 * Sets the text alignment.
 * @param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * @param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
void TextDisplayer::set_alignment(HorizontalAlignment horizontal_alignment,
				  VerticalAlignment vertical_alignment) {
  this->horizontal_alignment = horizontal_alignment;
  this->vertical_alignment = vertical_alignment;
}

/**
 * Sets the rendering mode of the text.
 * @param rendering_mode rendering mode: TEXT_SOLID, TEXT_SHADED or TEXT_BLENDED
 */
void TextDisplayer::set_rendering_mode(TextRenderingMode rendering_mode) {
  this->rendering_mode = rendering_mode;
}

/**
 * Sets the color of the text.
 * @param r red component (0 to 255)
 * @param g green component (0 to 255)
 * @param b blue component (0 to 255)
 */
void TextDisplayer::set_text_color(int r, int g, int b) {
  this->text_color.r = r;
  this->text_color.g = g;
  this->text_color.b = b;
}

/**
 * Sets the background color of the text.
 * This is only useful for the TEXT_SHADED rendering.
 * @param r red component (0 to 255)
 * @param g green component (0 to 255)
 * @param b blue component (0 to 255)
 */
void TextDisplayer::set_background_color(int r, int g, int b) {
  this->background_color.r = r;
  this->background_color.g = g;
  this->background_color.b = b;  
}

/**
 * Displays the text specified with the current color and the current alignment.
 * The coordinate system depends on the current alignment.
 * @param text the text to display
 * @param destination the destination surface
 * @param x x coordinate of the text
 * @param y y coordinate of the text
 */
void TextDisplayer::show_text(const char *text,
			      SDL_Surface *destination,
			      int x, int y) {
  
  if (text[0] == '\0') {
    return;
  }
  
  // create the text surface
  SDL_Surface *text_surface = NULL;

  switch (rendering_mode) {

  case TEXT_SOLID:
    text_surface = TTF_RenderText_Solid(font, text, text_color);
    break;

  case TEXT_SHADED:
    text_surface = TTF_RenderText_Shaded(font, text, text_color, background_color);
    break;

  case TEXT_BLENDED:
    text_surface = TTF_RenderText_Blended(font, text, text_color);
    break;
  }

  // calculate the coordinates of the top-left corner
  int x_left = 0, y_top = 0;

  switch (horizontal_alignment) {

  case ALIGN_LEFT:
    x_left = x;
    break;

  case ALIGN_CENTER:
    x_left = x - text_surface->w / 2;
    break;

  case ALIGN_RIGHT:
    x_left = x - text_surface->w;
    break;
  }

  switch (vertical_alignment) {

  case ALIGN_TOP:
    y_top = y;
    break;

  case ALIGN_MIDDLE:
    y_top = y - text_surface->h / 2;
    break;

  case ALIGN_BOTTOM:
    y_top = y - text_surface->h;
    break;
  }

  SDL_Rect position = {x_left, y_top, 0, 0};
  SDL_BlitSurface(text_surface, NULL, destination, &position);

  delete text_surface;
}
