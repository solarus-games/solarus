#ifndef ZSDX_TEXT_SURFACE_H
#define ZSDX_TEXT_SURFACE_H

#include "Common.h"

/**
 * This class displays some text on a surface.
 * Basically, it encapsulates an SDL_Surface and handles horizontal
 * and vertical text alignment, color and other properties.
 */
class TextSurface {

 public:

  /**
   * Horizontal alignment of the text.
   */
  enum HorizontalAlignment {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
  };

  /**
   * Vertical alignment of the text.
   */
  enum VerticalAlignment {
    ALIGN_TOP,
    ALIGN_MIDDLE,
    ALIGN_BOTTOM
  };

  /**
   * Rendering mode of the text.
   */
  enum RenderingMode {
    TEXT_SOLID,  /**< the text is displayed without smooth effect */
    TEXT_SHADED, /**< the text is displayed with a smooth effect for an unicolor background */
    TEXT_BLENDED /**< the text is displayed with a smooth effect for any background */
  };

  /**
   * The fonts available.
   */
  enum FontId {
    FONT_LA       = 0,   /**< Link's Awakening font (default) */
    FONT_STANDARD = 1,   /**< a more common font, with fixed width too */
  };

 private:

  static TTF_Font *fonts[2];

  FontId font_id;
  HorizontalAlignment horizontal_alignment;
  VerticalAlignment vertical_alignment;
  RenderingMode rendering_mode;
  SDL_Color text_color;
  SDL_Color background_color; // only for the TEXT_SHADED rendering

  int x;
  int y;
  SDL_Surface *surface;
  SDL_Rect text_position;

  string text;

  void rebuild(void);

 public:

  static void initialize(void);
  static void quit(void);

  TextSurface(int x, int y);
  TextSurface(int x, int y,
	      HorizontalAlignment horizontal_alignment,
	      VerticalAlignment vertical_alignment);
  ~TextSurface(void);

  void set_font(FontId font);
  void set_alignment(HorizontalAlignment horizontal_alignment,
		     VerticalAlignment vertical_alignment);
  void set_rendering_mode(RenderingMode rendering_mode);
  void set_text_color(int r, int g, int b);
  void set_background_color(int r, int g, int b);
  void set_position(int x, int y);
  void set_x(int x);
  void set_y(int y);
  void set_text(string text);
  string get_text(void);
  void add_char(char c);

  void display(SDL_Surface *destination);

};

#endif
