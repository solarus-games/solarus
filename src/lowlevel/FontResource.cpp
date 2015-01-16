/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/FontResource.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/CurrentQuest.h"

namespace Solarus {

bool FontResource::fonts_loaded = false;
std::map<std::string, FontResource::FontFile> FontResource::fonts;

/**
 * \brief Initializes the font system.
 */
void FontResource::initialize() {

  TTF_Init();
}

/**
 * \brief Closes the font system.
 */
void FontResource::quit() {

  fonts.clear();
  fonts_loaded = false;
  TTF_Quit();
}

/**
 * \brief Loads the fonts declared in the quest resource list.
 */
void FontResource::load_fonts() {

  // Get the list of available fonts.
  const std::map<std::string, std::string>& font_resource =
      CurrentQuest::get_resources(ResourceType::FONT);

  for (const auto& kvp: font_resource) {

    const std::string& font_id = kvp.first;

    FontFile font;

    // Load the font.

    bool bitmap_font = false;
    const std::string file_name_start = std::string("fonts/") + font_id;
    if (QuestFiles::data_file_exists(file_name_start + ".png")) {
      font.file_name = file_name_start + ".png";
      bitmap_font = true;
    }
    else if (QuestFiles::data_file_exists(file_name_start + ".PNG")) {
      font.file_name = file_name_start + ".PNG";
      bitmap_font = true;
    }
    else if (QuestFiles::data_file_exists(file_name_start + ".ttf")) {
      font.file_name = file_name_start + ".ttf";
    }
    else if (QuestFiles::data_file_exists(file_name_start + ".TTF")) {
      font.file_name = file_name_start + ".TTF";
    }
    else if (QuestFiles::data_file_exists(file_name_start + ".ttc")) {
      font.file_name = file_name_start + ".ttc";
    }
    else if (QuestFiles::data_file_exists(file_name_start + ".TTC")) {
      font.file_name = file_name_start + ".TTC";
    }
    else if (QuestFiles::data_file_exists(file_name_start + ".fon")) {
      font.file_name = file_name_start + ".fon";
    }
    else if (QuestFiles::data_file_exists(file_name_start + ".FON")) {
      font.file_name = file_name_start + ".FON";
    }
    else {
      Debug::error(std::string("Cannot find font file 'fonts/")
          + font_id + "' (tried with extensions .png, .ttf, .ttc and .fon)"
      );
      continue;
    }

    if (bitmap_font) {
      // It's a bitmap font.
      font.bitmap_font = Surface::create(font.file_name, Surface::DIR_DATA);
    }

    else {
      // It's an outline font.
      font.buffer = QuestFiles::data_file_read(font.file_name);
      font.bitmap_font = nullptr;
    }

    fonts.insert(std::make_pair(font_id, std::move(font)));
  }

  fonts_loaded = true;
}

/**
 * \brief Returns the id of default font.
 * \return Id of the first font in alphabetical order, or an empty string
 * if there is no font at all.
 */
std::string FontResource::get_default_font_id() {

  if (!fonts_loaded) {
    load_fonts();
  }

  if (fonts.empty()) {
    return "";
  }

  return fonts.begin()->first;
}

/**
 * \brief Returns whether the specified font exists.
 * \param font_id The id to test.
 * \return \c true if there is a valid font with this id in the quest resource
 * list.
 */
bool FontResource::exists(const std::string& font_id) {

  if (!fonts_loaded) {
    load_fonts();
  }

  return fonts.find(font_id) != fonts.end();
}

/**
 * \brief Returns whether the specified font is a bitmap or an outline font.
 * \param font_id Id of the font to test. It must exist.
 * \return \c true if this is a bitmap font, \c false if this is an outline font.
 */
bool FontResource::is_bitmap_font(const std::string& font_id) {

  if (!fonts_loaded) {
    load_fonts();
  }

  const auto& kvp = fonts.find(font_id);
  Debug::check_assertion(kvp != fonts.end(), std::string("No such font: '") + font_id + "'");
  return kvp->second.bitmap_font != nullptr;
}

/**
 * \brief Returns the surface image of a bitmap font.
 * \param font_id Id of the bitmap font to get. It must exist.
 * \return The bitmap.
 */
SurfacePtr FontResource::get_bitmap_font(const std::string& font_id) {

  if (!fonts_loaded) {
    load_fonts();
  }

  const auto& kvp = fonts.find(font_id);
  Debug::check_assertion(kvp != fonts.end(), std::string("No such font: '") + font_id + "'");
  Debug::check_assertion(kvp->second.bitmap_font != nullptr, std::string("This is not a bitmap font: '") + font_id + "'");
  return kvp->second.bitmap_font;
}

/**
 * \brief Returns an outline font with the specified size.
 * \param font_id Id of the outline font to get. It must exist.
 * \param size Size to use.
 * \return The font.
 */
TTF_Font& FontResource::get_outline_font(const std::string& font_id, int size) {

  if (!fonts_loaded) {
    load_fonts();
  }

  const auto& kvp = fonts.find(font_id);
  Debug::check_assertion(kvp != fonts.end(), std::string("No such font: '") + font_id + "'");
  FontFile& font = kvp->second;
  Debug::check_assertion(font.bitmap_font == nullptr, std::string("This is not an outline font: '") + font_id + "'");

  std::map<int, OutlineFontReader>& outline_fonts = kvp->second.outline_fonts;

  const auto& kvp2 = outline_fonts.find(size);
  if (kvp2 != outline_fonts.end()) {
    return *kvp2->second.outline_font;
  }

  // First time we want this font with this particular size.
  SDL_RWops_UniquePtr rw = SDL_RWops_UniquePtr(SDL_RWFromMem(
      const_cast<char*>(font.buffer.data()),
      (int) font.buffer.size()
  ));
  TTF_Font_UniquePtr outline_font(TTF_OpenFontRW(rw.get(), 0, size));
  Debug::check_assertion(outline_font != nullptr,
      std::string("Cannot load font from file '") + font.file_name
      + "': " + TTF_GetError()
  );
  OutlineFontReader reader = { std::move(rw), std::move(outline_font) };
  outline_fonts.insert(std::make_pair(size, std::move(reader)));
  return *outline_fonts.at(size).outline_font;
}

}

