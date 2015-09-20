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
#ifndef SOLARUS_TILESET_DATA_H
#define SOLARUS_TILESET_DATA_H

#include "solarus/Common.h"
#include "solarus/EnumInfo.h"
#include "solarus/entities/Ground.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lua/LuaData.h"
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace Solarus {

/**
 * \brief Kind of scrolling applied to a tile pattern.
 */
enum class TileScrolling {
    NONE,               /**< No scrolling. */
    PARALLAX,           /**< Parallax scrolling. */
    SELF                /**< Scrolling on itself. */
};

template <>
struct EnumInfoTraits<TileScrolling> {
  static const std::string pretty_name;

  static const EnumInfo<TileScrolling>::names_type names;
};

/**
 * \brief Kind of scrolling applied to a tile pattern.
 */
enum class TilePatternRepeatMode {
    ALL,                /**< Repeatable in both directions. */
    HORIZONTAL,         /**< Repeatable only horizontally. */
    VERTICAL,           /**< Repeatable only vertically. */
    NONE                /**< Not repeatable. */
};

template <>
struct EnumInfoTraits<TilePatternRepeatMode> {
  static const std::string pretty_name;

  static const EnumInfo<TilePatternRepeatMode>::names_type names;
};

/**
 * \brief Stores the properties of a tile pattern.
 */
class SOLARUS_API TilePatternData {

  public:

    TilePatternData();
    explicit TilePatternData(const Rectangle& frame);

    Ground get_ground() const;
    void set_ground(Ground ground);

    int get_default_layer() const;
    void set_default_layer(int default_layer);

    TileScrolling get_scrolling() const;
    void set_scrolling(TileScrolling scrolling);

    TilePatternRepeatMode get_repeat_mode() const;
    void set_repeat_mode(TilePatternRepeatMode repeat_mode);

    bool is_multi_frame() const;
    int get_num_frames() const;
    Rectangle get_frame() const;
    void set_frame(const Rectangle& frame);

    const std::vector<Rectangle>& get_frames() const;
    void set_frames(const std::vector<Rectangle>& frames);

  private:

    Ground ground;                     /**< Terrain of this pattern. */
    int default_layer;                 /**< Initial layer when creating a tile. */
    TileScrolling scrolling;           /**< Kind of scrolling if any. */
    TilePatternRepeatMode repeat_mode; /**< How this patterns intends to be repeated. */
    std::vector<Rectangle> frames;     /**< Coordinates of the pattern's frame(s).
                                        * - 1 element: one frame (no animation).
                                        * - 3 elements: three frames, animated with 0-1-2-0.
                                        * - 4 elements: three frames, animated with 0-1-2-1-0. */

};

/**
 * \brief Stores the content of a tileset data file.
 */
class SOLARUS_API TilesetData : public LuaData {

  public:

    TilesetData();

    Color get_background_color() const;
    void set_background_color(const Color& background_color);

    int get_num_patterns() const;
    const std::map<std::string, TilePatternData>& get_patterns() const;
    bool exists(const std::string& pattern_id) const;
    const TilePatternData& get_pattern(const std::string& pattern_id) const;
    TilePatternData& get_pattern(const std::string& pattern_id);
    bool add_pattern(
        const std::string& pattern_id, const TilePatternData& pattern);
    bool remove_pattern(const std::string& pattern_id);
    bool set_pattern_id(
        const std::string& old_pattern_id, const std::string& new_pattern_id);

    virtual bool import_from_lua(lua_State* l) override;
    virtual bool export_to_lua(std::ostream& out) const override;

  private:

    Color background_color;       /**< Background color of the tileset. */
    std::map<std::string, TilePatternData>
        patterns;                 /**< The tile patterns indexed by their id. */

};

}

#endif

