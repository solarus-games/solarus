/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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

const std::string LuaContext::sprite_module_name = "sol.sprite";

/**
 * @brief Initializes the sprite features provided to Lua.
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
      { "fade_in", drawable_api_fade_in },
      { "fade_out", drawable_api_fade_out },
      { "start_movement", drawable_api_start_movement },
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
 * @brief Returns whether a value is a userdata of type sprite.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a sprite.
 */
bool LuaContext::is_sprite(lua_State* l, int index) {
  return is_userdata(l, index, sprite_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * sprite and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the sprite
 */
Sprite& LuaContext::check_sprite(lua_State* l, int index) {
  return static_cast<Sprite&>(check_userdata(l, index, sprite_module_name));
}

/**
 * @brief Pushes a sprite userdata onto the stack.
 * @param l a Lua context
 * @param sprite a sprite
 */
void LuaContext::push_sprite(lua_State* l, Sprite& sprite) {

  sprite.set_lua_context(&get_lua_context(l));  // To make callbacks work.
  push_userdata(l, sprite);
}

/**
 * @brief Implementation of \ref lua_api_sprite_create.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
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
 * @brief Implementation of \ref lua_api_sprite_get_animation.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_get_animation(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  const std::string& animation_name = sprite.get_current_animation();
  push_string(l, animation_name);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_sprite_set_animation.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_set_animation(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);

  const std::string& animation_name = luaL_checkstring(l, 2);
  sprite.set_current_animation(animation_name);
  sprite.restart_animation();

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_sprite_get_direction.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_get_direction(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  lua_pushinteger(l, sprite.get_current_direction());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_sprite_set_direction.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_set_direction(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  int direction = int(luaL_checkinteger(l, 2));

  sprite.set_current_direction(direction);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_sprite_get_frame.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_get_frame(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  lua_pushinteger(l, sprite.get_current_frame());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_sprite_set_frame.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_set_frame(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);

  int frame = int(luaL_checkinteger(l, 2));
  sprite.set_current_frame(frame);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_sprite_get_frame_delay.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_get_frame_delay(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  lua_pushinteger(l, sprite.get_frame_delay());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_sprite_set_frame_delay.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_set_frame_delay(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);

  uint32_t delay = int(luaL_checkinteger(l, 2));
  sprite.set_frame_delay(delay);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_sprite_is_paused.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_is_paused(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);

  lua_pushboolean(l, sprite.is_paused());

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_sprite_set_paused.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_set_paused(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  bool paused = true;  // true if unspecified.
  if (lua_isboolean(l, 2)) {
    paused = lua_toboolean(l, 2);
  }

  sprite.set_paused(paused);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_sprite_set_ignore_suspend.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::sprite_api_set_ignore_suspend(lua_State *l) {

  Sprite& sprite = check_sprite(l, 1);
  bool ignore_suspend = true;  // true if unspecified.
  if (lua_isboolean(l, 2)) {
    ignore_suspend = lua_toboolean(l, 2);
  }

  sprite.set_ignore_suspend(ignore_suspend);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_sprite_synchronize.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
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
 * @brief Calls the on_animation_finished() method of a Lua sprite.
 * @param sprite A sprite whose animation has just finished.
 * @param animation Name of the animation finished.
 */
void LuaContext::sprite_on_animation_finished(Sprite& sprite,
    const std::string& animation) {

  push_sprite(l, sprite);
  on_animation_finished(animation);
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_frame_changed() method of a Lua sprite.
 * @param sprite A sprite whose frame has just changed.
 * @param animation Name of the current animation.
 * @param animation Name of the sprite animation.
 * @param frame The new frame.
 */
void LuaContext::sprite_on_frame_changed(Sprite& sprite,
    const std::string& animation, int frame) {

  push_sprite(l, sprite);
  on_frame_changed(animation, frame);
  lua_pop(l, 1);
}

