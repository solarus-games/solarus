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
#ifndef SOLARUS_SPRITE_DATA_H
#define SOLARUS_SPRITE_DATA_H

#include "solarus/core/Common.h"
#include "solarus/core/Rectangle.h"
#include "solarus/lua/LuaData.h"
#include <deque>
#include <iosfwd>
#include <map>
#include <string>
#include <vector>

namespace Solarus {

class Point;
class Size;

/**
 * \brief Stores the properties of a sprite animation direction.
 */
class SOLARUS_API SpriteAnimationDirectionData {

  public:

    SpriteAnimationDirectionData();
    SpriteAnimationDirectionData(const Point& xy, const Size& size);
    SpriteAnimationDirectionData(
        const Point& xy, const Size& size, const Point& origin,
        int num_frames, int num_columns = 1);

    Point get_xy() const;
    void set_xy(const Point& xy);

    Size get_size() const;
    void set_size(const Size& size);

    Point get_origin() const;
    void set_origin(const Point& origin);

    Rectangle get_bounding_box() const;

    bool is_multi_frame() const;
    int get_num_frames() const;
    void set_num_frames(int num_frames);

    int get_num_columns() const;
    void set_num_columns(int num_columns);

    Rectangle get_frame() const;
    std::vector<Rectangle> get_all_frames() const;

  private:

    Point xy;         /**< Coordinates of the first frame. */
    Size size;        /**< Size of frames. */
    Point origin;     /**< Coordinates of the origin of frames. */
    int num_frames;   /**< Number of frames. */
    int num_columns;  /**< Number of columns. */

};

/**
 * \brief Stores the properties of a sprite animation.
 */
class SOLARUS_API SpriteAnimationData {

  public:

    SpriteAnimationData();
    SpriteAnimationData(const std::string& src_image,
        std::deque<SpriteAnimationDirectionData>& directions,
        uint32_t frame_delay, int loop_on_frame);

    bool src_image_is_tileset() const;
    std::string get_src_image() const;
    void set_src_image(const std::string& src_image);

    uint32_t get_frame_delay() const;
    void set_frame_delay(const uint32_t& frame_delay);

    int get_loop_on_frame() const;
    void set_loop_on_frame(int loop_on_frame);

    int get_num_directions() const;
    const SpriteAnimationDirectionData& get_direction(int direction_nb) const;
    SpriteAnimationDirectionData& get_direction(int direction_nb);
    const std::deque<SpriteAnimationDirectionData>& get_directions() const;
    void set_directions(const std::deque<SpriteAnimationDirectionData>& directions);
    void add_direction(const SpriteAnimationDirectionData& direction);
    bool remove_direction(int direction_nb);
    bool move_direction(int direction_nb, int new_direction_nb);

  private:

    std::string src_image;  /**< Name of the source image. */
    uint32_t frame_delay;   /**< Delay in milliseconds between each frame. */
    int loop_on_frame;      /**< Number of the frame to loop on. */

    std::deque<SpriteAnimationDirectionData>
        directions;         /**< List of directions. */

};

/**
 * \brief Stores the content of a sprite data file.
 */
class SOLARUS_API SpriteData : public LuaData {

  public:

    SpriteData();

    int get_num_animations() const;
    const std::map<std::string, SpriteAnimationData>& get_animations() const;
    bool has_animation(const std::string& animation_name) const;
    const SpriteAnimationData& get_animation(
        const std::string& animation_name) const;
    SpriteAnimationData& get_animation(const std::string& animation_name);
    bool add_animation(
        const std::string& animation_name, const SpriteAnimationData& animation);
    bool remove_animation(const std::string& animation_name);
    bool set_animation_name(
        const std::string& old_animation_name, const std::string& new_animation_name);

    std::string get_default_animation_name() const;
    bool set_default_animation_name(const std::string& animation_name);

    virtual bool import_from_lua(lua_State* l) override;
    virtual bool export_to_lua(std::ostream& out) const override;

  private:

    static int l_animation(lua_State* l);
    void export_animation_to_lua(
        const std::string &animation_name, std::ostream& out) const;

    std::map<std::string, SpriteAnimationData>
        animations;                      /**< The sprite animations indexed by their name. */
    std::string default_animation_name;  /**< Name of the default animation. */

};

}

#endif
