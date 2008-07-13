#include "TextDisplayed.h"
#include "FileTools.h"

/**
 * The two fonts, created in the initialize() function.
 */
TTF_Font *TextDisplayed::fonts[2] = {NULL, NULL};

/**
 * Initializes the font system.
 */
void TextDisplayed::initialize(void) {

  TTF_Init();

  string file_name = FileTools::data_file_add_prefix("zsdx.ttf");

  fonts[FONT_LA] = TTF_OpenFont(file_name.c_str(), 11);
  if (fonts[FONT_LA] == NULL) {
    DIE("Cannot load font '" << file_name << "'");
  }
  
  file_name = FileTools::data_file_add_prefix("fixed8.fon");

  fonts[FONT_STANDARD] = TTF_OpenFont(file_name.c_str(), 11);
  if (fonts[FONT_STANDARD] == NULL) {
    DIE("Cannot load font 'fixed8.fon'");
  }
}

/**
 * Closes the font system.
 */
void TextDisplayed::quit(void) {

  TTF_CloseFont(fonts[FONT_LA]);
  TTF_CloseFont(fonts[FONT_STANDARD]);
  TTF_Quit();
}

/**
 * Creates a text to display with the default properties:
 * - font: Link's Awakening
 * - horizontal alignment: left
 * - vertical alignment: middle
 * - rendering mode: solid
 * - text color: white
 * - background color: black
 * @param x x position of the text on the destination surface
 * @param y y position of the text on the destination surface
 */
TextDisplayed::TextDisplayed(int x, int y):
  font_id(FONT_LA),
  horizontal_alignment(ALIGN_LEFT),
  vertical_alignment(ALIGN_MIDDLE),
  rendering_mode(TEXT_SOLID),
  text_surface(NULL) {

  text[0] = '\0';
  set_text_color(255, 255, 255);
  set_background_color(0, 0, 0);
  set_position(x, y);
}

/**
 * Creates a text to display with the specified alignment properties.
 * @param x x position of the text on the destination surface
 * @param y y position of the text on the destination surface
 * @param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * @param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
TextDisplayed::TextDisplayed(int x, int y,
			     HorizontalAlignment horizontal_alignment,
			     VerticalAlignment vertical_alignment):

  font_id(FONT_LA),
  horizontal_alignment(horizontal_alignment),
  vertical_alignment(vertical_alignment),
  rendering_mode(TEXT_SOLID),
  text_surface(NULL) {

  text[0] = '\0';
  set_text_color(255, 255, 255);
  set_background_color(0, 0, 0);
  set_position(x, y);
}

/**
 * Destructor.
 */
TextDisplayed::~TextDisplayed(void) {
  SDL_FreeSurface(text_surface);
}

/**
 * Sets the font to use.
 * @param font_id a font: FONT_LA or FONT_STANDARD
 */
void TextDisplayed::set_font(FontId font_id) {
  this->font_id = font_id;
}

/**
 * Sets the text alignment.
 * @param horizontal_alignment horizontal alignment of the text: ALIGN_LEFT,
 * ALIGN_CENTER or ALIGN_RIGHT
 * @param vertical_alignment vertical alignment of the text: ALIGN_TOP,
 * ALIGN_MIDDLE or ALIGN_BOTTOM
 */
void TextDisplayed::set_alignment(HorizontalAlignment horizontal_alignment,
				  VerticalAlignment vertical_alignment) {
  this->horizontal_alignment = horizontal_alignment;
  this->vertical_alignment = vertical_alignment;

  rebuild();
}

/**
 * Sets the rendering mode of the text.
 * @param rendering_mode rendering mode: TEXT_SOLID, TEXT_SHADED or TEXT_BLENDED
 */
void TextDisplayed::set_rendering_mode(TextRenderingMode rendering_mode) {
  this->rendering_mode = rendering_mode;

  rebuild();
}

/**
 * Sets the color of the text.
 * @param r red component (0 to 255)
 * @param g green component (0 to 255)
 * @param b blue component (0 to 255)
 */
void TextDisplayed::set_text_color(int r, int g, int b) {
  this->text_color.r = r;
  this->text_color.g = g;
  this->text_color.b = b;

  rebuild();
}

/**
 * Sets the background color of the text.
 * This is only useful for the TEXT_SHADED rendering.
 * @param r red component (0 to 255)
 * @param g green component (0 to 255)
 * @param b blue component (0 to 255)
 */
void TextDisplayed::set_background_color(int r, int g, int b) {
  this->background_color.r = r;
  this->background_color.g = g;
  this->background_color.b = b; 

  rebuild(); 
}

/**
 * Sets the position of the text on the destination surface.
 * @param x x position of the text on the destination surface
 * @param y y position of the text on the destination surface
 */
void TextDisplayed::set_position(int x, int y) {
  this->x = x;
  this->y = y;

  rebuild();
}

/**
 * Sets the string drawn.
 * If the specified string is the same than the current text, nothing is done.
 * @param text the text to display (cannot be NULL)
 */
void TextDisplayed::set_text(const char *text) {

  if (strcmp(text, this->text)) {

    // there is a change
    strncpy(this->text, text, 63);
    rebuild();
  }
}

/**
 * Returns the text currently displayed.
 * @return the text currently displayed, or NULL if there is no text
 */
const char *TextDisplayed::get_text(void) {
  return text;
}

/**
 * Creates the text surface.
 * This function is called when there is a change.
 */
void TextDisplayed::rebuild(void) {

  if (text_surface != NULL) {
    // another text was previously set: delete it
    SDL_FreeSurface(text_surface);
    text_surface = NULL;
  }
  
  if (text[0] == '\0') {
    // empty string: no surface to create
    return;
  }
  
  // create the text surface

  switch (rendering_mode) {

  case TEXT_SOLID:
    text_surface = TTF_RenderUTF8_Solid(fonts[font_id], text, text_color);
    break;

  case TEXT_SHADED:
    text_surface = TTF_RenderUTF8_Shaded(fonts[font_id], text, text_color, background_color);
    break;

  case TEXT_BLENDED:
    text_surface = TTF_RenderUTF8_Blended(fonts[font_id], text, text_color);
    break;
  }

  if (text_surface == NULL) {
    DIE("Cannot create the text surface for string '" << text << "': " << SDL_GetError());
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

  text_position.x = x_left;
  text_position.y = y_top;
}

/**
 * Displays the text on a surface.
 * This method just blits on the given surface the text surface created
 * when you called set_text().
 * @param destination the destination surface
 */
void TextDisplayed::display(SDL_Surface *destination) {

  if (text_surface != NULL) {
    SDL_BlitSurface(text_surface, NULL, destination, &text_position);
  }
}
