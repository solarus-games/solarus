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
#ifndef SOLARUS_FONT_RESOURCE_H
#define SOLARUS_FONT_RESOURCE_H

#include "solarus/core/Common.h"
#include "solarus/graphics/SurfacePtr.h"
#include <map>
#include <memory>
#include <string>
#include <SDL_ttf.h>

namespace Solarus {

/**
 * \brief Provides access to font files.
 */
class FontResource {

  public:

    static void initialize();
    static void quit();

    static std::string get_default_font_id();
    static bool exists(const std::string& font_id);
    static bool is_bitmap_font(const std::string& font_id);
    static SurfacePtr get_bitmap_font(const std::string& font_id);
    static TTF_Font& get_outline_font(const std::string& font_id, int size);

  private:

    struct SDL_RWops_Deleter {
      void operator()(SDL_RWops* rw) {
        SDL_FreeRW(rw);
      }
    };
    using SDL_RWops_UniquePtr = std::unique_ptr<SDL_RWops, SDL_RWops_Deleter>;

    struct TTF_Font_Deleter {
      void operator()(TTF_Font* font) {
        TTF_CloseFont(font);
      }
    };
    using TTF_Font_UniquePtr = std::unique_ptr<TTF_Font, TTF_Font_Deleter>;

    /**
     * Reading an outline font for a given font size.
     */
    struct OutlineFontReader {
        SDL_RWops_UniquePtr rw;
        TTF_Font_UniquePtr outline_font;
    };

    /**
     * This structure stores in memory the content of a font file.
     */
    struct FontFile {
      std::string file_name;                          /**< Name of the font file, relative to the data directory. */
      std::string buffer;                             /**< The font file loaded into memory. */

      SurfacePtr bitmap_font;                         /**< The font bitmap. Only used for bitmap fonts. */
      std::map<int, OutlineFontReader>
          outline_fonts;                              /**< This font in any size it was loaded with.
                                                       * Only used for outline fonts. */
    };

    static void load_fonts();

    static bool fonts_loaded;
    static std::map<std::string, FontFile> fonts;

};

}

#endif

