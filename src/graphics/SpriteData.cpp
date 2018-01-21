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
#include "solarus/core/Debug.h"
#include "solarus/graphics/SpriteData.h"
#include "solarus/lua/LuaTools.h"
#include <algorithm>
#include <ostream>
#include <utility>

namespace Solarus {

/**
 * \brief Creates a default single-frame sprite animation direction.
 */
SpriteAnimationDirectionData::SpriteAnimationDirectionData() :
  xy(0, 0),
  size(16, 16),
  origin(0, 0),
  num_frames(1),
  num_columns(1) {
}

/**
 * \brief Creates a single-frame sprite animation direction.
 * \param xy Coordinates of the single frame.
 * \param size Size of the single frame.
 */
SpriteAnimationDirectionData::SpriteAnimationDirectionData(
    const Point& xy, const Size& size) :
  xy(xy),
  size(size),
  origin(0, 0),
  num_frames(1),
  num_columns(1) {
}

/**
 * \brief Creates a sprite animation direction.
 * \param xy Coordinates of the first frame.
 * \param size Size of the frames.
 * \param origin Origin of the frames.
 * \param num_frames Numbers of the frames.
 * \param num_columns Numbers of columns.
 */
SpriteAnimationDirectionData::SpriteAnimationDirectionData(
    const Point& xy, const Size& size, const Point& origin,
    int num_frames, int num_columns) :
  xy(xy),
  size(size),
  origin(origin) {

  set_num_frames(num_frames);
  set_num_columns(num_columns);
}

/**
 * \brief Returns the coordinates of the first frame of this direction.
 * \return The coordinates of the first frame.
 */
Point SpriteAnimationDirectionData::get_xy() const {
  return xy;
}

/**
 * \brief Changes the coordinates of the first frame of this direction.
 * \param xy New coordinates of the first frame.
 */
void SpriteAnimationDirectionData::set_xy(const Point& xy) {
  this->xy = xy;
}

/**
 * \brief Returns the size of frames of this direction.
 * \return The size of frames.
 */
Size SpriteAnimationDirectionData::get_size() const {
  return size;
}

/**
 * \brief Changes the size of frames of this direction.
 * \param size The new size of frames.
 */
void SpriteAnimationDirectionData::set_size(const Size& size) {
  this->size = size;
}

/**
 * \brief Returns the origin point of frames of this direction.
 * \return The origin point of frames.
 */
Point SpriteAnimationDirectionData::get_origin() const {
  return origin;
}

/**
 * \brief Changes the origin point of frames of this direction.
 * \param origin The origin point of frames.
 */
void SpriteAnimationDirectionData::set_origin(const Point& origin) {
  this->origin = origin;
}

/**
 * \brief Returns the bounding box of frames of this direction.
 *
 * This is a rectangle of size get_size() and whose top-left corner has
 * coordinates -get_origin().
 *
 * \return The bounding box.
 */
Rectangle SpriteAnimationDirectionData::get_bounding_box() const {

  return Rectangle(-get_origin(), get_size());
}

/**
 * \brief Returns whether this is a multi-frame direction.
 * \return \c true if the direction has more than one frame.
 */
bool SpriteAnimationDirectionData::is_multi_frame() const {
  return num_frames > 1;
}

/**
 * \brief Returns the number of frames of this direction.
 * \return The number of frames.
 */
int SpriteAnimationDirectionData::get_num_frames() const {
  return num_frames;
}

/**
 * \brief Changes the number of frames of this direction.
 * \param num_frames The new number of frames.
 */
void SpriteAnimationDirectionData::set_num_frames(int num_frames) {
  this->num_frames = std::max(num_frames, 1);
}

/**
 * \brief Returns the number of columns of this direction.
 * \return The number of columns.
 */
int SpriteAnimationDirectionData::get_num_columns() const {
  return num_columns;
}

/**
 * \brief Changes the number of columns of this direction.
 * \param num_columns The new number of columns.
 */
void SpriteAnimationDirectionData::set_num_columns(int num_columns) {
  this->num_columns = std::max(num_columns, 1);
}

/**
 * @brief Returns the frame corresponding to the first frame of this direction.
 * @return The first frame.
 */
Rectangle SpriteAnimationDirectionData::get_frame() const {
  return Rectangle(xy, size);
}

/**
 * \brief Returns all frames of this direction.
 * \return All frames.
 */
std::vector<Rectangle> SpriteAnimationDirectionData::get_all_frames() const {

  std::vector<Rectangle> frames;
  int frame_number = 0;

  int num_rows = num_frames / num_columns;
  if (num_frames % num_columns != 0) {
    num_rows++;
  }

  for (int row = 0; row < num_rows && frame_number < num_frames; ++row) {
    for (int col = 0; col < num_columns && frame_number < num_frames; ++col) {

      frames.emplace_back(
        xy.x + col * size.width,
        xy.y + row * size.height,
        size.width,
        size.height
      );
      ++frame_number;
    }
  }

  return frames;
}

/**
 * \brief Creates a default empty sprite animation.
 */
SpriteAnimationData::SpriteAnimationData() :
  src_image(""),
  frame_delay(0),
  loop_on_frame(0),
  directions() {
}

/**
 * \brief Creates a sprite animation.
 * \param src_image The source image of this animation.
 * \param directions Directions of this animation.
 * \param frame_delay Delay in millisecond between two frames.
 * \param loop_on_frame Frame to loop on after the last frame (or -1 to make no loop).
 */
SpriteAnimationData::SpriteAnimationData(
    const std::string& src_image,
    std::deque<SpriteAnimationDirectionData> &directions,
    uint32_t frame_delay, int loop_on_frame) :
  src_image(src_image),
  frame_delay(frame_delay),
  loop_on_frame(loop_on_frame),
  directions(directions) {
}

/**
 * \brief Returns whether the source image is a tileset.
 * \return \c true if the source image is a tileset.
 */
bool SpriteAnimationData::src_image_is_tileset() const {
  return src_image == "tileset";
}

/**
 * \brief Returns the source image of this animation.
 * \return The source image.
 */
std::string SpriteAnimationData::get_src_image() const {
  return src_image;
}

/**
 * \brief Changes the source image of this animation.
 * \param src_image The new source image.
 */
void SpriteAnimationData::set_src_image(const std::string& src_image) {
  this->src_image = src_image;
}

/**
 * \brief Returns the frame delay of this animation.
 * \return The frame delay.
 */
uint32_t SpriteAnimationData::get_frame_delay() const {
  return frame_delay;
}

/**
 * \brief Changes the frame delay of this animation.
 * \param frame_delay The new frame delay.
 */
void SpriteAnimationData::set_frame_delay(const uint32_t& frame_delay) {
  this->frame_delay = frame_delay;
}

/**
 * \brief Returns the number of frame to loop on.
 * \return The frame to loop on.
 */
int SpriteAnimationData::get_loop_on_frame() const {
  return loop_on_frame;
}

/**
 * \brief Changes the number of frame to loop on.
 * \param loop_on_frame The number of frame to loop on (or -1 to make no loop).
 */
void SpriteAnimationData::set_loop_on_frame(int loop_on_frame) {
  this->loop_on_frame = std::max(loop_on_frame, -1);
}

/**
 * \brief Returns the number of directions in this animation.
 * \return The number of directions.
 */
int SpriteAnimationData::get_num_directions() const {
  return directions.size();
}

/**
 * \brief Returns a direction.
 * \param direction_nb The number of a direction.
 * \return The direction at the specified number.
 * The object remains valid until directions are added, removed or moved.
 */
const SpriteAnimationDirectionData&
  SpriteAnimationData::get_direction(int direction_nb) const {

  int size = directions.size();
  Debug::check_assertion(direction_nb >= 0 && direction_nb < size,
    "No such direction");

  return directions[direction_nb];
}

/**
 * \brief Returns a direction.
 *
 * Non-const version.
 *
 * \param direction_nb The number of a direction.
 * \return The direction at the specified number.
 * The object remains valid until directions are added, removed or moved.
 */
SpriteAnimationDirectionData&
  SpriteAnimationData::get_direction(int direction_nb) {

  int size = directions.size();
  Debug::check_assertion(direction_nb >= 0 && direction_nb < size,
    "No such direction");

  return directions[direction_nb];
}

/**
 * \brief Returns all directions of this animation.
 * \return The directions.
 */
const std::deque<SpriteAnimationDirectionData>&
  SpriteAnimationData::get_directions() const {
  return directions;
}

/**
 * \brief Changes all directions of this animation.
 * \param directions New all directions.
 */
void SpriteAnimationData::set_directions(
    const std::deque<SpriteAnimationDirectionData>& directions) {
  this->directions = directions;
}

/**
 * \brief Adds a direction to the animation.
 * \param direction The direction to add.
 */
void SpriteAnimationData::add_direction(
    const SpriteAnimationDirectionData& direction) {
  directions.push_back(direction);
}

/**
 * \brief Removes a direction from this animation.
 * \param direction_nb Number of the direction to remove.
 * \return \c true in case of success.
 */
bool SpriteAnimationData::remove_direction(int direction_nb) {

  int size = directions.size();
  if (direction_nb >= size) {
    return false;
  }

  directions.erase(directions.begin() + direction_nb);
  return true;
}

/**
 * @brief Moves a direction of this animation to a specified position.
 * @param direction_nb Number of direction to move.
 * @param new_direction_nb The new position of the direction.
 * @return \c true in case of success.
 */
bool SpriteAnimationData::move_direction(int direction_nb, int new_direction_nb) {

  int size = directions.size();

  if (direction_nb < 0 || direction_nb >= size) {
    return false;
  }

  new_direction_nb = std::min(std::max(new_direction_nb, 0), size - 1);

  if (direction_nb == new_direction_nb) {
    return true;
  }

  // insert to new position
  auto new_pos = directions.begin() + new_direction_nb;
  if (direction_nb < new_direction_nb) {
    new_pos++;
  }
  directions.insert(new_pos, directions[direction_nb]);

  // remove from last position
  auto last_pos = directions.begin() + direction_nb;
  if (direction_nb > new_direction_nb) {
    last_pos++;
  }
  directions.erase(last_pos);

  return true;
}

/**
 * \brief Creates an empty sprite.
 */
SpriteData::SpriteData() {
}

/**
 * \brief Returns the number of animations in this sprite.
 * \return The number of animations.
 */
int SpriteData::get_num_animations() const {
  return animations.size();
}

/**
 * \brief Returns all animations of this sprite.
 * \return The animations indexed by their name.
 */
const std::map<std::string, SpriteAnimationData>&
  SpriteData::get_animations() const {
  return animations;
}

/**
 * \brief Returns whether there exists an animation with the specified name.
 * \param animation_name The name to test.
 * \return \c true if an animation exists with this name in this sprite.
 */
bool SpriteData::has_animation(const std::string& animation_name) const {
  return animations.find(animation_name) != animations.end();
}

/**
 * \brief Returns the animation with the specified name.
 * \param animation_name An animation name. It must exist.
 * \return The animation with this name.
 * The object remains valid until animations are added or removed.
 */
const SpriteAnimationData& SpriteData::get_animation(
    const std::string& animation_name) const {

  const auto& it = animations.find(animation_name);
  Debug::check_assertion(it != animations.end(),
    std::string("No such animation: '") + animation_name + "'");

  return it->second;
}

/**
 * \brief Returns the animation with the specified name.
 *
 * Non-const version.
 *
 * \param animation_name An animation name. It must exist.
 * \return The animation with this name.
 * The object remains valid until animations are added or removed.
 */
SpriteAnimationData& SpriteData::get_animation(
    const std::string& animation_name) {

  const auto& it = animations.find(animation_name);
  Debug::check_assertion(it != animations.end(),
    std::string("No such animation: '") + animation_name + "'");

  return it->second;
}

/**
 * \brief Adds an animation to this sprite.
 * \param animation_name Name of the animation to add.
 * \param animation The animation to add.
 * \return \c true in case of success.
 */
bool SpriteData::add_animation(
    const std::string& animation_name, const SpriteAnimationData& animation)  {

  const auto& result = animations.emplace(animation_name, animation);
  if (!result.second) {
    // Insertion failed: the name already exists.
    return false;
  }

  if (animations.size() == 1) {
    default_animation_name = animation_name;
  }

  return true;
}

/**
 * \brief Removes an animation from this sprite.
 * \param animation_name Name of the animation to remove.
 * \return \c true in case of success.
 */
bool SpriteData::remove_animation(const std::string& animation_name) {

  bool removed = animations.erase(animation_name) > 0;
  if (removed && default_animation_name == animation_name) {
    if (animations.size() > 0) {
      const auto& it = animations.cbegin();
      default_animation_name = it->first;
    } else {
      default_animation_name = "";
    }
  }
  return removed;
}

/**
 * \brief Changes the name of an animation in the sprite.
 * \param old_animation_name Old name of the animation.
 * \param new_animation_name New name to set.
 * \return \c true in case of success.
 * In case of failure, the old animation is unchanged.
 */
bool SpriteData::set_animation_name(
    const std::string& old_animation_name,
    const std::string& new_animation_name) {

  if (!has_animation(old_animation_name)) {
    // No animation was found with the old name.
    return false;
  }

  if (has_animation(new_animation_name)) {
    // The new name is already used.
    return false;
  }

  if (default_animation_name == old_animation_name) {
    default_animation_name = new_animation_name;
  }

  SpriteAnimationData animation = get_animation(old_animation_name);
  remove_animation(old_animation_name);
  add_animation(new_animation_name, animation);

  return true;
}

/**
 * \brief Returns the default animation name.
 * \return The default animation name.
 */
std::string SpriteData::get_default_animation_name() const {
  return default_animation_name;
}

/**
 * \brief Changes the default animation.
 * \param animation_name Name of the new default animation.
 * \return \c true in case of success.
 * In case of failure, the default animation is unchanged.
 */
bool SpriteData::set_default_animation_name(const std::string& animation_name) {

  if (!has_animation(animation_name)) {
    return false;
  }

  default_animation_name = animation_name;
  return true;
}

/**
 * \brief Function called by the Lua data file to define an animation.
 *
 * - Argument 1 (table): properties of the animation.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int SpriteData::l_animation(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_getfield(l, LUA_REGISTRYINDEX, "sprite");
    SpriteData* sprite = static_cast<SpriteData*>(
        lua_touserdata(l, -1));
    lua_pop(l, 1);

    LuaTools::check_type(l, 1, LUA_TTABLE);

    std::string animation_name = LuaTools::check_string_field(l, 1, "name");
    std::string src_image = LuaTools::check_string_field(l, 1, "src_image");
    uint32_t frame_delay = (uint32_t) LuaTools::opt_int_field(l, 1, "frame_delay", 0);
    int frame_to_loop_on = LuaTools::opt_int_field(l, 1, "frame_to_loop_on", -1);

    if (frame_to_loop_on < -1) {
      LuaTools::arg_error(l, 1,
          "Bad field 'frame_to_loop_on' (must be a positive number or -1)"
      );
    }

    lua_settop(l, 1);
    lua_getfield(l, 1, "directions");
    if (lua_type(l, 2) != LUA_TTABLE) {
      LuaTools::arg_error(l, 1,
          std::string("Bad field 'directions' (table expected, got ")
      + luaL_typename(l, -1) + ")");
    }

    // Traverse the directions table.
    std::deque<SpriteAnimationDirectionData> directions;
    int i = 1;
    lua_rawgeti(l, -1, i);
    while (!lua_isnil(l, -1)) {
      ++i;

      if (lua_type(l, -1) != LUA_TTABLE) {
        LuaTools::arg_error(l, 1,
            std::string("Bad field 'directions' (expected table, got ")
                + luaL_typename(l, -1)
        );
      }

      int x = LuaTools::check_int_field(l, -1, "x");
      int y = LuaTools::check_int_field(l, -1, "y");
      int frame_width = LuaTools::check_int_field(l, -1, "frame_width");
      int frame_height = LuaTools::check_int_field(l, -1, "frame_height");
      int origin_x = LuaTools::opt_int_field(l, -1, "origin_x", 0);
      int origin_y = LuaTools::opt_int_field(l, -1, "origin_y", 0);
      int num_frames = LuaTools::opt_int_field(l, -1, "num_frames", 1);
      int num_columns = LuaTools::opt_int_field(l, -1, "num_columns", num_frames);

      if (num_columns < 1 || num_columns > num_frames) {
        LuaTools::arg_error(l, 1,
            "Bad field 'num_columns': must be between 1 and the number of frames");
      }

      if (frame_to_loop_on >= num_frames) {
        LuaTools::arg_error(l, 1,
            "Bad field 'frame_to_loop_on': exceeds the number of frames");
      }

      lua_pop(l, 1);
      lua_rawgeti(l, -1, i);

      directions.emplace_back(
            Point(x, y), Size(frame_width, frame_height),
            Point(origin_x, origin_y), num_frames, num_columns);
    }

    if (sprite->animations.find(animation_name) != sprite->animations.end()) {
      LuaTools::error(l, std::string("Duplicate animation '") + animation_name);
    }

    sprite->animations.emplace(animation_name,
      SpriteAnimationData(src_image, directions, frame_delay, frame_to_loop_on)
    );

    // Set the first animation as the default one.
    if (sprite->animations.size() == 1) {
      sprite->default_animation_name = animation_name;
    }

    return 0;
  });
}

/**
 * \copydoc LuaData::import_from_lua
 */
bool SpriteData::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "sprite");
  lua_register(l, "animation", l_animation);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Failed to load sprite: ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool SpriteData::export_to_lua(std::ostream& out) const {

  export_animation_to_lua(default_animation_name, out);

  for (const auto& kvp : animations) {
    const std::string& name = kvp.first;
    if (name != default_animation_name) {
      export_animation_to_lua(name, out);
    }
  }
  return true;
}

/**
 * \brief Saves an animation data as Lua into a stream.
 * \param animation_name The name of animation to save.
 * \param out The stream to write.
 */
void SpriteData::export_animation_to_lua(
    const std::string& animation_name, std::ostream& out) const {

  if (!has_animation(animation_name)) {
    return;
  }

  SpriteAnimationData animation = animations.at(animation_name);

  out << "animation{\n"
      << "  name = \"" << escape_string(animation_name) << "\",\n"
      << "  src_image = \"" << escape_string(animation.get_src_image()) << "\",\n";

  if (animation.get_frame_delay() > 0) {
    out << "  frame_delay = " << animation.get_frame_delay() << ",\n";
    if (animation.get_loop_on_frame() >= 0) {
      out << "  frame_to_loop_on = " << animation.get_loop_on_frame() << ",\n";
    }
  }

  out << "  directions = {\n";

  for (const auto& direction: animation.get_directions()) {

    Point xy = direction.get_xy();
    Size size = direction.get_size();
    Point origin = direction.get_origin();
    int num_frames = direction.get_num_frames();
    int num_columns = direction.get_num_columns();

    out << "    { "
        << "x = " << xy.x << ", "
        << "y = " << xy.y << ", "
        << "frame_width = " << size.width << ", "
        << "frame_height = " << size.height << ", "
        << "origin_x = " << origin.x << ", "
        << "origin_y = " << origin.y;

    if (num_frames > 1) {
      out << ", num_frames = " << num_frames;
      if (num_columns > 0 && num_columns < num_frames) {
        out << ", num_columns = " << num_columns;
      }
    }

    out << " },\n";
  }
  out << "  },\n";
  out << "}\n";
}

}
