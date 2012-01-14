/*
 * Copyright (C) 2009-2011 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lua/Script.h"
#include "movements/Movement.h"
#include "Sprite.h"

const char* Script::sprite_module_name = "sol.sprite";

/**
 * @brief Initializes the sprite features provided to Lua.
 */
void Script::initialize_sprite_module() {

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
      { "fade", sprite_api_fade },
      { "synchronize", sprite_api_synchronize },
      { "start_movement", sprite_api_start_movement },
      { "stop_movement", sprite_api_stop_movement },
      { NULL, NULL }
  };

  static const luaL_Reg metamethods[] = {
      { "__gc", sprite_meta_gc },
      { NULL, NULL }
  };

  // create a table and fill it with the methods
  luaL_openlib(l, sprite_module_name, methods, 0);
                                  // sol.sprite

  // create the metatable for the type, add it to the Lua registry
  luaL_newmetatable(l, sprite_module_name);
                                  // sol.sprite mt
  // fill the metatable
  luaL_openlib(l, NULL, metamethods, 0);
                                  // sol.sprite mt
  lua_pushvalue(l, -2);
                                  // sol.sprite mt sol.sprite
  // metatable.__index = sol.sprite
  lua_setfield(l, -2, "__index");
                                  // sol.sprite mt

  // also use the metatable to store the transitions and movements of sprites
  lua_newtable(l);
                                  // sol.sprite mt movements
  lua_setfield(l, -2, "movements");
                                  // sol.sprite mt
  lua_pop(l, 2);
                                  // --
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * sprite and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the sprite
 */
Sprite& Script::check_sprite(lua_State* l, int index) {

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

  Sprite** sprite = (Sprite**) luaL_checkudata(l, index, sprite_module_name);
  return **sprite;
}

/**
 * @brief Pushes a sprite userdata onto the stack.
 * @param sprite a sprite
 */
void Script::push_sprite(lua_State* l, Sprite& sprite) {

                                  // ...
  Sprite** block_adress = (Sprite**) lua_newuserdata(l, sizeof(Sprite*));
  *block_adress = &sprite;
                                  // ... sprite
  luaL_getmetatable(l, sprite_module_name);
                                  // ... sprite mt
  lua_setmetatable(l, -2);
                                  // ... sprite
}

/**
 * @brief Creates and returns a sprite.
 *
 * - Argument 1 (string): the animation set for the new sprite
 * - Return value (sprite): the sprite created
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::sprite_api_create(lua_State* l) {

  const std::string& animation_set_id = luaL_checkstring(l, 1);
  Sprite* sprite = new Sprite(animation_set_id);
  get_script(l).sprites_created.insert(sprite);
  push_sprite(l, *sprite);

  return 1;
}

/**
 * @brief Finalizes a sprite.
 *
 * - Argument 1: a sprite
 *
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::sprite_meta_gc(lua_State* l) {

  Script& script = get_script(l);

  Sprite* sprite= *((Sprite**) luaL_checkudata(l, 1, sprite_module_name));
  if (script.sprites_created.count(sprite) > 0) {
    delete sprite;
    script.sprites_created.erase(sprite);
  }
  // otherwise don't touch this instance, it belongs to C

  return 0;
}

/**
 * @brief Returns the current animation of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Return value (string): name of the current animation
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_get_animation(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);
  const std::string& animation_name = sprite.get_current_animation();
  lua_pushstring(l, animation_name.c_str());

  return 1;
}

/**
 * @brief Sets the current animation of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (string): name of the animation to set
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_set_animation(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  const std::string& animation_name = luaL_checkstring(l, 2);
  sprite.set_current_animation(animation_name);
  sprite.restart_animation();

  return 0;
}

/**
 * @brief Returns the current direction of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Return value (integer): direction of the sprite
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_get_direction(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);
  lua_pushinteger(l, sprite.get_current_direction());

  return 1;
}

/**
 * @brief Sets the current direction of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (integer): direction to set
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_set_direction(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  int direction = luaL_checkinteger(l, 2);

  sprite.set_current_direction(direction);

  return 0;
}

/**
 * @brief Returns the current frame of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Return value (integer): index of the current frame of the animation
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_get_frame(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);
  lua_pushinteger(l, sprite.get_current_frame());

  return 1;
}

/**
 * @brief Sets the current frame of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (integer): index of the frame to set in the animation
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_set_frame(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  int frame = luaL_checkinteger(l, 2);
  sprite.set_current_frame(frame);

  return 0;
}

/**
 * @brief Returns the delay between two frames of the animation of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Return value: the delay between two frames in milliseconds
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_get_frame_delay(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);
  lua_pushinteger(l, sprite.get_frame_delay());

  return 1;
}

/**
 * @brief Sets the delay between two frames of the animation of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (integer): the new delay in milliseconds
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_set_frame_delay(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  uint32_t delay = luaL_checkinteger(l, 2);
  sprite.set_frame_delay(delay);

  return 0;
}

/**
 * @brief Returns whether the animation of a sprite is paused.
 *
 * - Argument 1 (sprite): a sprite
 * - Return value (boolean): true if the animation is currently paused
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_is_paused(lua_State* l) {

  const Sprite& sprite = check_sprite(l, 1);
  lua_pushboolean(l, sprite.is_paused());

  return 1;
}

/**
 * @brief Pauses or resumes the animation of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (boolean): true to pause the animation, false to resume it
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_set_paused(lua_State* l) {

  Sprite& sprite = check_sprite(l, 1);
  bool paused = lua_toboolean(l, 2);
  sprite.set_paused(paused);

  return 0;
}

/**
 * @brief Sets whether the animation of a sprite should continue even when
 * the sprite receives a set_suspended(true) call.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (boolean): true to continue the animation when the sprite is suspended
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_set_ignore_suspend(lua_State *l) {

  Sprite& sprite = check_sprite(l, 1);
  bool ignore_suspend = lua_toboolean(l, 2);
  sprite.set_ignore_suspend(ignore_suspend);

  return 0;
}

/**
 * @brief Starts a fade-in or a fade-out effect on a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (integer): direction of the effect: 0 for fade-in, 1 for fade-out
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_fade(lua_State *l) {

  Sprite& sprite = check_sprite(l, 1);
  int direction = luaL_checkinteger(l, 2);
  sprite.start_fading(direction);

  return 0;
}

/**
 * @brief Synchronizes the frames of a sprite with the frames on a reference sprite
 * when the name of their current animation is the same.
 * From now on, when both sprites have the same current animation,
 * the first sprite will stop changing frames with time and it will take the same
 * frames as the reference sprite instead.
 *
 * - Argument 1 (sprite): the sprite to synchronize
 * - Argument 2 (sprite): the reference sprite, or nil to stop any previous synchronization
 *
 * @param l the Lua context that is calling this function
 */
int Script::sprite_api_synchronize(lua_State *l) {

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
 * @brief Starts a movement on a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Argument 2 (movement): the movement to apply
 * - Optional argument 3 (function): a Lua function to be called when the
 * movement finishes
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::sprite_api_start_movement(lua_State* l) {

  /*
  Script& script = get_script(l);

  int callback = LUA_REFNIL;

  Sprite& sprite = check_sprite(l, 1);
  Movement& movement = check_movement(l, 2);

  // the next argument (if any) is the callback
  if (lua_gettop(l) >= 3) {
    // store the callback into the registry
    lua_settop(l, 3);
    callback = luaL_ref(l, LUA_REGISTRYINDEX);
  }

  script.start_sprite_movement(sprite, movement, callback);
  */

  return 0;
}

/**
 * @brief Stops the movement (if any) of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 *
 * @param l the Lua context that is calling this function
 * @return the number of values to return to Lua
 */
int Script::sprite_api_stop_movement(lua_State* l) {

  /*
  Script& script = get_script(l);

  Sprite& sprite = check_sprite(l, 1);
  script.stop_sprite_movement(sprite);
  */

  return 0;
}
