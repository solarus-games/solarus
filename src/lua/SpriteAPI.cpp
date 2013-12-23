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
#include <lua.hpp>
#include "lua/LuaContext.h"
#include "Sprite.h"
#include <sstream>

namespace solarus {

const std::string LuaContext::sprite_module_name = "sol.sprite";

/**
 * \brief Initializes the sprite features provided to Lua.
 */
void LuaContext::register_sprite_module() {

  static const luaL_Reg methods[] = {
      { "create", sprite_api_create },
      { "get_animation", sprite_api_get_animation },
      { "set_animation", sprite_api_set_animation },
      { "get_direction", sprite_api_get_direction },
      { "set_direction", sprite_api_set_direction },
      { "get_frame", sprite_api_get_frame },
      { "set_frame", sprite_api_set_frame },
      { "get_frame_delay", sprite_api_get_frame_delay },
      { "set_frame_delay", sprite_api_set_frame_delay },
      { "is_paused", sprite_api_is_paused },
      { "set_paused", sprite_api_set_paused },
      { "set_ignore_suspend", sprite_api_set_ignore_suspend },
      { "synchronize", sprite_api_synchronize },
      { "draw", drawable_api_draw },
      { "draw_region", drawable_api_draw_region },
      { "fade_in", drawable_api_fade_in },
      { "fade_out", drawable_api_fade_out },
      { "get_xy", drawable_api_get_xy },
      { "set_xy", drawable_api_set_xy },
      { "get_movement", drawable_api_get_movement },
      { "stop_movement", drawable_api_stop_movement },
      { NULL, NULL }
  };
  static const luaL_Reg metamethods[] = {
      { "__gc", drawable_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };
  register_type(sprite_module_name, methods, metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type sprite.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a sprite.
 */
bool LuaContext::is_sprite(lua_State* l, int index) {
  return is_userdata(l, index, sprite_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * sprite and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the sprite
 */
Sprite& LuaContext::check_sprite(lua_State* l, int index) {
  return static_cast<Sprite&>(check_userdata(l, index, sprite_module_name));
}

/**
 * \brief Pushes a sprite userdata onto the stack.
 * \param l a Lua context
 * \param sprite a sprite
 */
void LuaContext::push_sprite(lua_State* l, Sprite& sprite) {

  sprite.set_lua_context(&get_lua_context(l));  // To make callbacks work.
  push_userdata(l, sprite);
}

/**
 * \brief Implementation of sol.sprite.create().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_create(lua_State* l) {

  const std::string& animation_set_id = luaL_checkstring(l, 1);

  // TODO if the file does not exist, make a Lua error instead of an assertion error.
  Sprite* sprite = new Sprite(animation_set_id);
  get_lua_context(l).add_drawable(sprite);

  push_sprite(l, *sprite);
  return 1;
}

/**
 * \brief Implementation of sprite:get_animation().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_get_animation(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  const std::string& animation_name = sprite.get_current_animation();
  push_string(l, animation_name);

  return 1;
}

/**
 * \brief Implementation of sprite:set_animation().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_set_animation(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);

  const std::string& animation_name = luaL_checkstring(l, 2);
  if (!sprite.has_animation(animation_name)) {
    arg_error(l, 2,
        std::string("Animation '") + animation_name
        + "' does not exist in sprite '" + sprite.get_animation_set_id() + "'"
    );
  }

  sprite.set_current_animation(animation_name);
  sprite.restart_animation();

  return 0;
}

/**
 * \brief Implementation of sprite:get_direction().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_get_direction(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  lua_pushinteger(l, sprite.get_current_direction());
  return 1;
}

/**
 * \brief Implementation of sprite:set_direction().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_set_direction(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  int direction = luaL_checkint(l, 2);

  if (direction < 0 || direction >= sprite.get_nb_directions()) {
    std::ostringstream oss;
    oss << "Illegal direction " << direction
        << " for sprite '" + sprite.get_animation_set_id()
        << "' in animation '" + sprite.get_current_animation() + "'";
    arg_error(l, 2, oss.str());
  }
  sprite.set_current_direction(direction);

  return 0;
}

/**
 * \brief Implementation of sprite:get_frame().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_get_frame(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  lua_pushinteger(l, sprite.get_current_frame());
  return 1;
}

/**
 * \brief Implementation of sprite:set_frame().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_set_frame(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  int frame = luaL_checkint(l, 2);

  if (frame < 0 || frame >= sprite.get_nb_frames()) {
    std::ostringstream oss;
    oss << "Illegal frame " << frame
        << " for sprite '" << sprite.get_animation_set_id()
        << "' in direction " << sprite.get_current_direction()
        << " of animation '" << sprite.get_current_animation() << "'";
    arg_error(l, 2, oss.str());
  }
  sprite.set_current_frame(frame);

  return 0;
}

/**
 * \brief Implementation of sprite:get_frame_delay().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_get_frame_delay(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  uint32_t frame_delay = sprite.get_frame_delay();
  if (frame_delay == 0) {
    lua_pushnil(l);
  }
  else {
    lua_pushinteger(l, frame_delay);
  }

  return 1;
}

/**
 * \brief Implementation of sprite:set_frame_delay().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_set_frame_delay(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  uint32_t delay = 0;
  if (!lua_isnil(l, 2)) {
    delay = uint32_t(luaL_checkint(l, 2));
  }

  sprite.set_frame_delay(delay);

  return 0;
}

/**
 * \brief Implementation of sprite:is_paused().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_is_paused(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  lua_pushboolean(l, sprite.is_paused());

  return 1;
}

/**
 * \brief Implementation of sprite:set_paused().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_set_paused(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  bool paused = true;  // true if unspecified.
  if (lua_gettop(l) >= 2) {
    paused = lua_toboolean(l, 2);
  }

  sprite.set_paused(paused);

  return 0;
}

/**
 * \brief Implementation of sprite:set_ignore_suspend().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_set_ignore_suspend(lua_State *l) {

  Sprite& sprite = check_sprite(l, 1);
  bool ignore_suspend = true;  // true if unspecified.
  if (lua_gettop(l) >= 2) {
    ignore_suspend = lua_toboolean(l, 2);
  }

  sprite.set_ignore_suspend(ignore_suspend);

  return 0;
}

/**
 * \brief Implementation of sprite:synchronize().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::sprite_api_synchronize(lua_State *l) {

  Sprite& sprite = check_sprite(l, 1);

  if (!lua_isnil(l, 2)) {
    Sprite& reference_sprite = check_sprite(l, 2);
    sprite.set_synchronized_to(&reference_sprite);
  }
  else {
    sprite.set_synchronized_to(NULL);
  }

  return 0;
}

/**
 * \brief Calls the on_animation_finished() method of a Lua sprite.
 *
 * Does nothing if the method is not defined.
 *
 * \param sprite A sprite whose animation has just finished.
 * \param animation Name of the animation finished.
 */
void LuaContext::sprite_on_animation_finished(Sprite& sprite,
    const std::string& animation) {

  if (!userdata_has_field(sprite, "on_animation_finished")) {
    return;
  }

  push_sprite(l, sprite);
  on_animation_finished(animation);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_animation_changed() method of a Lua sprite.
 *
 * Does nothing if the method is not defined.
 *
 * \param sprite A sprite whose animation has just changed.
 * \param animation Name of the new animation.
 */
void LuaContext::sprite_on_animation_changed(
    Sprite& sprite, const std::string& animation) {

  if (!userdata_has_field(sprite, "on_animation_changed")) {
    return;
  }

  push_sprite(l, sprite);
  on_animation_changed(animation);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_direction_changed() method of a Lua sprite.
 *
 * Does nothing if the method is not defined.
 *
 * \param sprite A sprite whose direction has just changed.
 * \param animation Name of the current animation.
 * \param direction The new direction.
 */
void LuaContext::sprite_on_direction_changed(Sprite& sprite,
    const std::string& animation, int direction) {

  if (!userdata_has_field(sprite, "on_direction_changed")) {
    return;
  }

  push_sprite(l, sprite);
  on_direction_changed(animation, direction);
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_frame_changed() method of a Lua sprite.
 *
 * Does nothing if the method is not defined.
 *
 * \param sprite A sprite whose frame has just changed.
 * \param animation Name of the current animation.
 * \param frame The new frame.
 */
void LuaContext::sprite_on_frame_changed(Sprite& sprite,
    const std::string& animation, int frame) {

  if (!userdata_has_field(sprite, "on_frame_changed")) {
    return;
  }

  push_sprite(l, sprite);
  on_frame_changed(animation, frame);
  lua_pop(l, 1);
}

}

