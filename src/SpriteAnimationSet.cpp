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
#include "solarus/SpriteAnimationSet.h"
#include "solarus/SpriteAnimation.h"
#include "solarus/SpriteAnimationDirection.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lua/LuaTools.h"

namespace Solarus {

/**
 * \brief Loads the animations of a sprite from a file.
 * \param id Id of the sprite animation set to load
 * (name of a sprite definition file, without the ".dat" extension).
 */
SpriteAnimationSet::SpriteAnimationSet(const std::string& id):
  id(id) {

  load();
}

/**
 * \brief Attempts to load this animation set from its file.
 */
void SpriteAnimationSet::load() {

  Debug::check_assertion(animations.empty(),
      "Animation set already loaded");

  // Compute the file name.
  std::string file_name = std::string("sprites/") + id + ".dat";

  lua_State* l = luaL_newstate();
  const std::string& buffer = QuestFiles::data_file_read(file_name);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0) {
    Debug::error(std::string("Failed to load sprite file '") + file_name
        + "': " + lua_tostring(l, -1));
    lua_pop(l, 1);
  }
  else {
    lua_pushlightuserdata(l, this);
    lua_setfield(l, LUA_REGISTRYINDEX, "animation_set");
    lua_register(l, "animation", l_animation);
    if (lua_pcall(l, 0, 0, 0) != 0) {
      Debug::error(std::string("Failed to load sprite file '") + file_name
          + "': " + lua_tostring(l, -1));
      lua_pop(l, 1);
    }
  }
  lua_close(l);
}

/**
 * \brief Function called by the Lua data file to define an animation.
 *
 * - Argument 1 (table): properties of the animation.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int SpriteAnimationSet::l_animation(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    lua_getfield(l, LUA_REGISTRYINDEX, "animation_set");
    SpriteAnimationSet* animation_set = static_cast<SpriteAnimationSet*>(
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
    std::vector<SpriteAnimationDirection> directions;
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

      animation_set->max_size.width = std::max(frame_width, animation_set->max_size.width);
      animation_set->max_size.height = std::max(frame_height, animation_set->max_size.height);

      int num_rows;
      if (num_frames % num_columns == 0) {
        num_rows = num_frames / num_columns;
      }
      else {
        num_rows = (num_frames / num_columns) + 1;
      }

      std::vector<Rectangle> positions_in_src;
      int j = 0;  // Frame number.
      for (int r = 0; r < num_rows && j < num_frames; ++r) {
        for (int c = 0; c < num_columns && j < num_frames; ++c) {

          Rectangle position_in_src(
              x + c * frame_width,
              y + r * frame_height,
              frame_width,
              frame_height);
          positions_in_src.push_back(position_in_src);
          ++j;
        }
      }

      directions.emplace_back(
          positions_in_src,
          Point(origin_x, origin_y)
      );
    }

    if (animation_set->animations.find(animation_name) != animation_set->animations.end()) {
      LuaTools::error(l, std::string("Duplicate animation '") + animation_name
          + "' in sprite '" + animation_set->id + "'");
    }

    animation_set->animations.insert(std::make_pair(animation_name,
        SpriteAnimation(src_image, directions, frame_delay, frame_to_loop_on)
    ));

    // Set the first animation as the default one.
    if (animation_set->animations.size() == 1) {
      animation_set->default_animation_name = animation_name;
    }

    return 0;
  });
}

/**
 * \brief When the sprite is displayed on a map, sets the tileset.
 *
 * This function must be called if this sprite image depends on the map's tileset.
 *
 * \param tileset The tileset.
 */
void SpriteAnimationSet::set_tileset(Tileset& tileset) {

  for (auto& kvp: animations) {
    kvp.second.set_tileset(tileset);
  }
}

/**
 * \brief Returns whether this animation set has an animation with the specified name.
 * \param animation_name an animation name
 * \return true if this animation exists
 */
bool SpriteAnimationSet::has_animation(
    const std::string& animation_name) const {
  return animations.find(animation_name) != animations.end();
}

/**
 * \brief Returns an animation.
 * \param animation_name Name of the animation to get.
 * \return The specified animation.
 */
const SpriteAnimation& SpriteAnimationSet::get_animation(
    const std::string& animation_name) const {

  Debug::check_assertion(has_animation(animation_name),
      std::string("No animation '") + animation_name
      + "' in animation set '" + id + "'"
  );

  return animations.find(animation_name)->second;
}

/**
 * \brief Returns an animation.
 * \param animation_name Name of the animation to get.
 * \return The specified animation.
 */
SpriteAnimation& SpriteAnimationSet::get_animation(
    const std::string& animation_name) {

  Debug::check_assertion(has_animation(animation_name),
      std::string("No animation '") + animation_name
      + "' in animation set '" + id + "'"
  );

  return animations.find(animation_name)->second;
}

/**
 * \brief Returns the name of the default animation, i.e. the first one.
 * \return the name of the default animation
 */
const std::string& SpriteAnimationSet::get_default_animation() const {
  return default_animation_name;
}

/**
 * \brief Enables the pixel-perfect collision detection for these animations.
 */
void SpriteAnimationSet::enable_pixel_collisions() {

  if (!are_pixel_collisions_enabled()) {

    for (auto& kvp: animations) {
      kvp.second.enable_pixel_collisions();
    }
  }
}

/**
 * \brief Returns whether the pixel-perfect collisions are enabled for these animations.
 * \return true if the pixel-perfect collisions are enabled
 */
bool SpriteAnimationSet::are_pixel_collisions_enabled() const {

  if (animations.empty()) {
    return false;
  }

  return animations.begin()->second.are_pixel_collisions_enabled();
}

/**
 * \brief Returns a size big enough to contain any frame of this animation set.
 * \return The maximum size of a frame in this animation set.
 */
const Size& SpriteAnimationSet::get_max_size() const {
  return max_size;
}

}

