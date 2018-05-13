/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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

#include "solarus/core/Common.h"
#include "solarus/core/Point.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/Drawable.h"
#include <map>
#include <string>
#include <SDL_ttf.h>

namespace Solarus {

class Size;

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
    enum class HorizontalAlignment {
      LEFT,                                      /**< the text is aligned to the left */
      CENTER,                                    /**< the text is centered */
      RIGHT                                      /**< the text is aligned to the right */
    };

    /**
     * Vertical alignment of the text.
     *
     */
    enum class VerticalAlignment {
      TOP,                                       /**< the text is aligned to the top */
      MIDDLE,                                    /**< the text is aligned in the middle */
      BOTTOM                                     /**< the text is aligned to the bottom */
    };

    /**
     * Rendering mode of the text.
     */
    enum class RenderingMode {
      SOLID,                                     /**< letters are drawn without effect (faster, visible pixels) */
      ANTIALIASING                               /**< letters are drawn with a smooth effect (slower) */
    };

    TextSurface(int x, int y);
    TextSurface(int x, int y,
        HorizontalAlignment horizontal_alignment,
        VerticalAlignment vertical_alignment);

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
    int get_font_size() const;
    void set_font_size(int font_size);

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
    virtual Size get_size() const override;

    virtual void raw_draw(Surface& dst_surface,const DrawInfos& infos) const override;
    virtual void raw_draw_region(Surface& dst_surface,const DrawInfos& infos) const override;

    virtual Rectangle get_region() const override;

    virtual const std::string& get_lua_type_name() const override;

    static constexpr int default_font_size = 11;      /**< Default font size for outline fonts. */

  private:

    void rebuild();
    void rebuild_bitmap();
    void rebuild_ttf();

    std::string font_id;                              /**< id of the font of the current text surface */
    HorizontalAlignment horizontal_alignment;         /**< horizontal alignment of the current text surface */
    VerticalAlignment vertical_alignment;             /**< vertical alignment of the current text surface */
    RenderingMode rendering_mode;                     /**< rendering mode of the current text surface */
    Color text_color;                                 /**< color of the text */
    int font_size;                                    /**< size of the font */

    int x;                                            /**< x coordinate of where the text is aligned */
    int y;                                            /**< y coordinate of where the text is aligned */

    SurfacePtr surface;                               /**< the surface to draw */
    Point text_position;                              /**< position of the top-left corner of the surface on the screen */

    std::string text;                                 /**< the string to draw (only one line) */

};

}

#endif

