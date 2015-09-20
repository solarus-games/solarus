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
#include "solarus/entities/GroundInfo.h"
#include "solarus/entities/TilesetData.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaTools.h"
#include <ostream>
#include <sstream>

namespace Solarus {

const std::string EnumInfoTraits<TileScrolling>::pretty_name = "tile scrolling";

const EnumInfo<TileScrolling>::names_type EnumInfoTraits<TileScrolling>::names = {
    { TileScrolling::NONE, "" },
    { TileScrolling::PARALLAX, "parallax" },
    { TileScrolling::SELF, "self" },
};

const std::string EnumInfoTraits<TilePatternRepeatMode>::pretty_name = "tile pattern repeat mode";

const EnumInfo<TilePatternRepeatMode>::names_type EnumInfoTraits<TilePatternRepeatMode>::names = {
    { TilePatternRepeatMode::ALL, "all" },
    { TilePatternRepeatMode::HORIZONTAL, "horizontal" },
    { TilePatternRepeatMode::VERTICAL, "vertical" },
    { TilePatternRepeatMode::NONE, "none" },
};

/**
 * \brief Creates a default single-frame tile pattern.
 */
TilePatternData::TilePatternData() :
    TilePatternData(Rectangle(0, 0, 16, 16)) {
}

/**
 * \brief Creates a single-frame tile pattern.
 * \param frame Coordinates of the single frame to make.
 */
TilePatternData::TilePatternData(const Rectangle& frame) :
    ground(Ground::TRAVERSABLE),
    default_layer(0),
    scrolling(TileScrolling::NONE),
    frames() {

  set_frame(frame);
}

/**
 * \brief Returns the kind of ground of this pattern.
 * \return The ground.
 */
Ground TilePatternData::get_ground() const {
  return ground;
}

/**
 * \brief Sets the kind of ground of this pattern.
 * \param ground The ground.
 */
void TilePatternData::set_ground(Ground ground) {
  this->ground = ground;
}

/**
 * \brief Returns the initial layer when creating a tile with this pattern.
 * \return The default layer.
 */
int TilePatternData::get_default_layer() const {
  return default_layer;
}

/**
 * \brief Sets the initial layer when creating a tile with this pattern.
 * \param default_layer The default layer.
 */
void TilePatternData::set_default_layer(int default_layer) {
  this->default_layer = default_layer;
}

/**
 * \brief Returns the kind of scrolling of this pattern.
 * \return The scrolling property.
 */
TileScrolling TilePatternData::get_scrolling() const {
  return scrolling;
}

/**
 * \brief Sets the kind of scrolling of this pattern.
 * \param scrolling The scrolling property.
 */
void TilePatternData::set_scrolling(TileScrolling scrolling) {
  this->scrolling = scrolling;
}

/**
 * \brief Returns how this pattern is intended to be repeated.
 */
TilePatternRepeatMode TilePatternData::get_repeat_mode() const {
  return repeat_mode;
}

/**
 * \brief Sets how this pattern is intended to be repeated.
 */
void TilePatternData::set_repeat_mode(TilePatternRepeatMode repeat_mode) {
  this->repeat_mode = repeat_mode;
}

/**
 * \brief Returns whether this is a multi-frame pattern.
 * \return \c true if the pattern has more than one frame.
 */
bool TilePatternData::is_multi_frame() const {
  return get_num_frames() > 1;
}

/**
 * \brief Returns the number of frames of this pattern.
 * \return The number of frames: 1, 3 or 4.
 */
int TilePatternData::get_num_frames() const {
  return (int) frames.size();
}

/**
 * \brief Returns the coordinates of this pattern in the tileset image file.
 *
 * This only makes sense for single-frame patterns.
 *
 * \return The coordinates of the single frame of this pattern.
 * Returns an empty rectangle if the pattern is nil.
 */
Rectangle TilePatternData::get_frame() const {

  return frames[0];
}

/**
 * \brief Sets the coordinates of this pattern in the tileset image file.
 *
 * The pattern will have a single frame.
 *
 * \param frame The coordinates of the single frame of this pattern.
 */
void TilePatternData::set_frame(const Rectangle& frame) {
  this->frames.clear();
  this->frames.push_back(frame);
}

/**
 * \brief Returns the coordinates of each frame of this pattern in the tileset
 * image file.
 * \return The coordinates of the frame(s) of this pattern.
 */
const std::vector<Rectangle>& TilePatternData::get_frames() const {
  return frames;
}

/**
 * \brief Sets the coordinates of each frame of this pattern in the tileset
 * image file.
 * \param frames The coordinates of the frame(s) of this pattern.
 */
void TilePatternData::set_frames(const std::vector<Rectangle>& frames) {

  Debug::check_assertion(!frames.empty(), "No frames");
  // TODO check number of elements
  this->frames = frames;
}

/**
 * \brief Creates an empty tileset.
 */
TilesetData::TilesetData() :
    background_color(Color::white),
    patterns() {

}

/**
 * \brief Returns the tileset's background color.
 * \return The background color.
 */
Color TilesetData::get_background_color() const {
  return background_color;
}

/**
 * \brief Sets the tileset's background color.
 * \param background_color The background color.
 */
void TilesetData::set_background_color(const Color& background_color) {
  this->background_color = background_color;
}

/**
 * \brief Returns the number of patterns in this tileset.
 * \return The number of patterns.
 */
int TilesetData::get_num_patterns() const {
  return patterns.size();
}

/**
 * \brief Returns all tile patterns of this tileset.
 * \return The tile pattern indexed by their id.
 */
const std::map<std::string, TilePatternData>& TilesetData::get_patterns() const {
  return patterns;
}

/**
 * \brief Returns whether there exists a tile pattern with the specified id.
 * \param pattern_id The id to test.
 * \return \c true if a tile pattern exists with this id in the tileset.
 */
bool TilesetData::exists(const std::string& pattern_id) const {
  return patterns.find(pattern_id) != patterns.end();
}

/**
 * \brief Returns the pattern with the specified id.
 * \param pattern_id A tile pattern id. It must exist.
 * \return The pattern with this id.
 * The object remains valid until tile patterns are added or removed.
 */
const TilePatternData& TilesetData::get_pattern(const std::string& pattern_id) const {

  const auto& it = patterns.find(pattern_id);
  Debug::check_assertion(it != patterns.end(),
    std::string("No such tile pattern: '") + pattern_id + "'");

  return it->second;
}

/**
 * \brief Returns the pattern with the specified id.
 *
 * Non-const version.
 *
 * \param pattern_id A tile pattern id. It must exist.
 * \return The pattern with this id.
 * The object remains valid until tile patterns are added or removed.
 */
TilePatternData& TilesetData::get_pattern(const std::string& pattern_id) {

  const auto& it = patterns.find(pattern_id);
  Debug::check_assertion(it != patterns.end(),
    std::string("No such tile pattern: '") + pattern_id + "'");

  return it->second;
}

/**
 * \brief Adds a pattern to the tileset.
 * \param pattern_id Id of the new pattern.
 * \param pattern The pattern to add.
 * \return \c true in case of success.
 */
bool TilesetData::add_pattern(
    const std::string& pattern_id, const TilePatternData& pattern) {

  const auto& result = patterns.emplace(pattern_id, pattern);
  if (!result.second) {
    // Insertion failed: the id already exists.
    return false;
  }

  return true;
}

/**
 * \brief Removes a pattern from the tileset.
 * \param pattern_id Id of the pattern to remove.
 * \return \c true in case of success.
 */
bool TilesetData::remove_pattern(const std::string& pattern_id) {

  return patterns.erase(pattern_id) > 0;
}

/**
 * \brief Changes the id of a pattern in the tileset.
 * \param old_pattern_id Old id of the pattern.
 * \param new_pattern_id New id to set.
 * \return \c true in case of success.
 * In case of failure, the old pattern is unchanged.
 */
bool TilesetData::set_pattern_id(
    const std::string& old_pattern_id, const std::string& new_pattern_id) {

  if (!exists(old_pattern_id)) {
    // No pattern was found with the old id.
    return false;
  }

  if (exists(new_pattern_id)) {
    // The new id is already used.
    return false;
  }

  TilePatternData pattern = get_pattern(old_pattern_id);
  remove_pattern(old_pattern_id);
  add_pattern(new_pattern_id, pattern);

  return true;
}

namespace {

/**
 * \brief Function called by Lua to set the background color of the tileset.
 *
 * - Argument 1 (table): background color (must be an array of 3 integers).
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int l_background_color(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_getfield(l, LUA_REGISTRYINDEX, "tileset");
    TilesetData& tileset = *static_cast<TilesetData*>(lua_touserdata(l, -1));
    lua_pop(l, 1);

    const Color& background_color = LuaTools::check_color(l, 1);

    tileset.set_background_color(background_color);

    return 0;
  });
}

/**
 * \brief Function called by Lua to add a tile pattern to the tileset.
 *
 * - Argument 1 (table): A table describing the tile pattern to create.
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int l_tile_pattern(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_getfield(l, LUA_REGISTRYINDEX, "tileset");
    TilesetData& tileset_data = *static_cast<TilesetData*>(lua_touserdata(l, -1));
    lua_pop(l, 1);

    TilePatternData pattern_data;

    const std::string& id = LuaTools::check_string_field(l, 1, "id");

    const Ground ground = LuaTools::check_enum_field<Ground>(
        l, 1, "ground", GroundInfo::get_ground_names()
    );
    pattern_data.set_ground(ground);

    const int default_layer = LuaTools::check_int_field(
        l, 1, "default_layer"
    );
    pattern_data.set_default_layer(default_layer);

    const TileScrolling scrolling = LuaTools::opt_enum_field<TileScrolling>(
        l, 1, "scrolling", TileScrolling::NONE
    );
    pattern_data.set_scrolling(scrolling);

    const TilePatternRepeatMode repeat_mode = LuaTools::opt_enum_field<TilePatternRepeatMode>(
        l, 1, "repeat_mode", TilePatternRepeatMode::ALL
    );
    pattern_data.set_repeat_mode(repeat_mode);

    const int width = LuaTools::check_int_field(l, 1, "width");
    const int height = LuaTools::check_int_field(l, 1, "height");
    // Start with the maximum number of frames.
    std::vector<Rectangle> frames(4, Rectangle(0, 0, width, height));

    int i = 0, j = 0;
    lua_settop(l, 1);
    lua_getfield(l, 1, "x");
    if (lua_isnumber(l, 2)) {
      // Single frame.
      frames[0].set_x(LuaTools::check_int(l, 2));
      i = 1;
    }
    else {
      // Multi-frame.
      lua_pushnil(l);
      while (lua_next(l, 2) != 0 && i < 4) {
        frames[i].set_x(LuaTools::check_int(l, 4));
        ++i;
        lua_pop(l, 1);
      }
    }
    lua_pop(l, 1);
    Debug::check_assertion(lua_gettop(l) == 1, "Invalid stack when parsing tile pattern");

    lua_getfield(l, 1, "y");
    if (lua_isnumber(l, 2)) {
      // Single frame.
      frames[0].set_y(LuaTools::check_int(l, 2));
      j = 1;
    }
    else {
      // Multi-frame.
      lua_pushnil(l);
      while (lua_next(l, 2) != 0 && j < 4) {
        frames[j].set_y(LuaTools::check_int(l, 4));
        ++j;
        lua_pop(l, 1);
      }
    }
    lua_pop(l, 1);
    Debug::check_assertion(lua_gettop(l) == 1, "Invalid stack when parsing tile pattern");

    // Check data.
    if (i != 1 && i != 3 && i != 4) {
      LuaTools::arg_error(l, 1, "Invalid number of frames for x");
    }
    if (j != 1 && j != 3 && j != 4) {
      LuaTools::arg_error(l, 1, "Invalid number of frames for y");
    }
    if (i != j) {
      LuaTools::arg_error(l, 1, "The length of x and y must match");
    }
    frames.resize(i);
    pattern_data.set_frames(frames);

    tileset_data.add_pattern(id, pattern_data);

    return 0;
  });
}

}  // Anonymous namespace.

/**
 * \copydoc LuaData::import_from_lua
 */
bool TilesetData::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "tileset");
  lua_register(l, "background_color", l_background_color);
  lua_register(l, "tile_pattern", l_tile_pattern);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Failed to load tileset: ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool TilesetData::export_to_lua(std::ostream& out) const {

  std::string last_pattern_id;
  // Background color.
  uint8_t r, g, b, a;
  background_color.get_components(r, g, b, a);
  out << "background_color{ "
      << static_cast<int>(r)
      << ", "
      << static_cast<int>(g)
      << ", "
      << static_cast<int>(b)
      << " }\n";

  // Tile patterns.
  for (const auto kvp : patterns) {
    const std::string& id = kvp.first;
    const TilePatternData& pattern = kvp.second;
    last_pattern_id = id;

    const Rectangle& first_frame = pattern.get_frame();
    int width = first_frame.get_width();
    int height = first_frame.get_height();
    std::ostringstream x;
    std::ostringstream y;
    if (!pattern.is_multi_frame()) {
      x << first_frame.get_x();
      y << first_frame.get_y();
    }
    else {
      x << "{ ";
      y << "{ ";
      bool first = true;
      for (const Rectangle& frame : pattern.get_frames()) {
        if (first) {
          first = false;
        }
        else {
          x << ", ";
          y << ", ";
        }
        x << frame.get_x();
        y << frame.get_y();
      }
      x << " }";
      y << " }";
    }

    const std::string& ground_name = GroundInfo::get_ground_name(pattern.get_ground());
    int layer_index = static_cast<int>(pattern.get_default_layer());

    out << "tile_pattern{\n"
        << "  id = \"" << id << "\",\n"
        << "  ground = \"" << ground_name << "\",\n"
        << "  default_layer = " << layer_index << ",\n"
        << "  x = " << x.str() << ",\n"
        << "  y = " << y.str() << ",\n"
        << "  width = " << width << ",\n"
        << "  height = " << height << ",\n";
    if (pattern.get_scrolling() != TileScrolling::NONE) {
      const std::string& scolling_name = enum_to_name(pattern.get_scrolling());
      out << "  scrolling = \"" << scolling_name << "\",\n";
    }
    if (pattern.get_repeat_mode() != TilePatternRepeatMode::ALL) {
      const std::string& repeat_mode_name = enum_to_name(pattern.get_repeat_mode());
      out << "  repeat_mode = \"" << repeat_mode_name << "\",\n";
    }
    out << "}\n\n";
  }

  return true;
}

}

