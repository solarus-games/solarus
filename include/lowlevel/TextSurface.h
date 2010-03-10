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
#ifndef SOLARUS_TEXT_SURFACE_H
#define SOLARUS_TEXT_SURFACE_H

#include "Common.h"
#include "lowlevel/Color.h"
#include "lowlevel/Rectangle.h"
#include <SDL/SDL_ttf.h>

/**
 * This class displays some text on a surface.
 * Basically, it encapsulates a graphic surface and handles text rendering,
 * horizontal and vertical text alignment, color and other properties.
 * This is the only class related to a font library (SDL_ttf).
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
      TEXT_SOLID,          /**< the text is displayed without smooth effect */
      TEXT_SHADED,         /**< the text is displayed with a smooth effect for an unicolor background */
      TEXT_BLENDED         /**< the text is displayed with a smooth effect for any background */
    };

    /**
     * The fonts available.
     */
    enum FontId {
      FONT_LA       = 0,   /**< italic font (default) */
      FONT_STANDARD = 1,   /**< a more common font, with fixed width too */
      FONT_LA_BIG   = 2,   /**< italic font with double size */
      FONT_NB
    };

  private:

    /**
     * This structures stores the data of a font.
     */
    struct FontData {
      std::string file_name;
      int font_size;
      char *buffer;
      SDL_RWops *rw;
      TTF_Font *font;
    };

    static FontData data[FONT_NB];

    FontId font_id;
    HorizontalAlignment horizontal_alignment;
    VerticalAlignment vertical_alignment;
    RenderingMode rendering_mode;
    Color text_color;
    Color background_color; // only for the TEXT_SHADED rendering

    int x;
    int y;
    Surface *surface;
    Rectangle text_position;

    std::string text;

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
    void set_text_color(Color &color);
    void set_text_color(int r, int g, int b);
    void set_background_color(Color &color);
    void set_position(int x, int y);
    void set_x(int x);
    void set_y(int y);
    void set_text(const std::string &text);
    const std::string & get_text(void);
    bool is_empty(void);
    void add_char(char c);

    int get_width(void);
    int get_height(void);
    const Rectangle get_size(void);

    void display(Surface *destination);

};

#endif

