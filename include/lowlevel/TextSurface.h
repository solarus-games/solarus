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
#ifndef SOLARUS_TEXT_SURFACE_H
#define SOLARUS_TEXT_SURFACE_H

#include "Common.h"
#include "Drawable.h"
#include "lowlevel/Color.h"
#include "lowlevel/Rectangle.h"
#include <SDL_ttf.h>
#include <map>

struct lua_State;

/**
 * \brief Draws a line of text on a surface.
 *
 * This class encapsulates a graphic surface and handles text rendering,
 * horizontal and vertical text alignment, color and other properties.
 * This is the only class related to a font library (SDL_ttf).
 *
 * Two types of fonts are supported:
 * - usual fonts (TTF and other formats are supported),
 * - an image containing characters drawn.
 */
class TextSurface: public Drawable {

  public:

    /**
     * Horizontal alignment of the text.
     */
    enum HorizontalAlignment {
      ALIGN_LEFT,                                     /**< the text is aligned to the left */
      ALIGN_CENTER,                                   /**< the text is centered */
      ALIGN_RIGHT                                     /**< the text is aligned to the right */
    };

    /**
     * Vertical alignment of the text.
     *
     */
    enum VerticalAlignment {
      ALIGN_TOP,                                      /**< the text is aligned to the top */
      ALIGN_MIDDLE,                                   /**< the text is aligned in the middle */
      ALIGN_BOTTOM                                    /**< the text is aligned to the bottom */
    };

    /**
     * Rendering mode of the text.
     */
    enum RenderingMode {
      TEXT_SOLID,                                     /**< letter are drawn without effect (faster, visible pixels) */
      TEXT_ANTIALIASING                               /**< letters are drawn with a smooth effect (slower) */
    };

    static void initialize();
    static void quit();

    TextSurface(int x, int y);
    TextSurface(int x, int y,
        HorizontalAlignment horizontal_alignment,
        VerticalAlignment vertical_alignment);
    ~TextSurface();

    static bool has_font(const std::string& font_id);
    const std::string& get_font() const;
    void set_font(const std::string& font_id);
    HorizontalAlignment get_horizontal_alignment() const;
    void set_horizontal_alignment(HorizontalAlignment horizontal_alignment);
    VerticalAlignment get_vertical_alignment() const;
    void set_vertical_alignment(VerticalAlignment vertical_alignment);
    void set_alignment(HorizontalAlignment horizontal_alignment,
        VerticalAlignment vertical_alignment);
    RenderingMode get_rendering_mode() const;
    void set_rendering_mode(RenderingMode rendering_mode);
    const Color& get_text_color() const;
    void set_text_color(const Color& color);
    void set_text_color(int r, int g, int b);

    void set_position(int x, int y);
    int get_x() const;
    void set_x(int x);
    int get_y() const;
    void set_y(int y);
    const std::string& get_text() const;
    void set_text(const std::string& text);
    bool is_empty() const;
    void add_char(char c);

    int get_width() const;
    int get_height() const;
    const Rectangle get_size() const;

    void raw_draw(Surface& dst_surface, const Rectangle& dst_position);
    void raw_draw_region(const Rectangle& region,
        Surface& dst_surface, const Rectangle& dst_position);
    void draw_transition(Transition& transition);
    Surface& get_transition_surface();

    const std::string& get_lua_type_name() const;

  private:

    /**
     * This structures stores the data of a font.
     */
    struct FontData {
      std::string file_name;                          /**< name of the font file, relative to the data directory */
      int font_size;                                  /**< size of the characters */
      char* buffer;                                   /**< the file loaded into memory */
      SDL_RWops* rw;                                  /**< read/write object used to open the font file from memory */
      TTF_Font* internal_font;                        /**< the library-dependent font object */
      Surface* bitmap;                                /**< only used if it's a PNG font */
    };

    static void load_fonts();
    static int l_font(lua_State* l);
 
    void rebuild();
    void rebuild_bitmap();
    void rebuild_ttf();

    static bool fonts_loaded;                         /**< Whether fonts.dat was read. */
    static std::map<std::string, FontData> fonts;     /**< the data of each font, loaded from the file text/fonts.dat
                                                       * (fond id -> font data) */
    static std::string default_font_id;               /**< id of the default font to use */

    std::string font_id;                              /**< id of the font of the current text surface */
    HorizontalAlignment horizontal_alignment;         /**< horizontal alignment of the current text surface */
    VerticalAlignment vertical_alignment;             /**< vertical alignment of the current text surface */
    RenderingMode rendering_mode;                     /**< rendering mode of the current text surface */
    Color text_color;                                 /**< color of the text */

    int x;                                            /**< x coordinate of where the text is aligned */
    int y;                                            /**< y coordinate of where the text is aligned */
    Surface* surface;                                 /**< the surface to draw */
    Rectangle text_position;                          /**< position of the top-left corner of the surface on the screen */

    std::string text;                                 /**< the string to draw (only one line) */

};

#endif

