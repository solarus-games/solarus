/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lua/Script.h"
#include "movements/PixelMovement.h"
#include "movements/PathMovement.h"
#include "movements/RandomMovement.h"
#include "movements/RandomPathMovement.h"
#include "movements/PathFindingMovement.h"
#include "movements/TemporalMovement.h"
#include "movements/CircleMovement.h"
#include "movements/JumpMovement.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Music.h"
#include "lowlevel/Debug.h"
#include "lowlevel/Geometry.h"
#include "Timer.h"
#include "Sprite.h"
#include "Game.h"
#include "Map.h"
#include <lua.hpp>
#include <sstream>
#include <cmath>

/**
 * @brief Includes a script into the current Lua context.
 *
 * - Argument 1 (string): file name of the script without extension,
 *   relative to the data directory of the quest
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_include(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &file_name = luaL_checkstring(l, 1);

  script->load(file_name);

  return 0;
}

/**
 * @brief Plays a sound.
 *
 * - Argument 1 (string): name of the sound
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_play_sound(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const SoundId &sound_id = luaL_checkstring(l, 1);

  Sound::play(sound_id);

  return 0;
}

/**
 * @brief Plays a music.
 *
 * - Argument 1 (string): name of the music (possibly "none" or "same")
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_play_music(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const MusicId &music_id = luaL_checkstring(l, 1);
  Music::play(music_id);

  return 0;
}

/**
 * @brief Starts a timer to run a Lua function after a delay.
 *
 * - Argument 1 (integer): the timer duration in milliseconds
 * - Argument 2 (string): name of the Lua function to call when the timer is finished
 * (no argument, no return value)
 * - Argument 3 (boolean): plays a sound until the timer expires
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_timer_start(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  uint32_t duration = luaL_checkinteger(l, 1);
  const std::string &callback_name = luaL_checkstring(l, 2);
  bool with_sound = lua_toboolean(l, 3) != 0;

  Timer *timer = new Timer(duration, callback_name, with_sound);
  if (script->is_new_timer_suspended()) {
    timer->set_suspended(true);
  }
  script->add_timer(timer);

  return 0;
}

/**
 * @brief Stops an existing timer in the script.
 *
 * - Argument 1 (string): name of the Lua function that is supposed to be called
 * when the timer finishes
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_timer_stop(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string &callback_name = luaL_checkstring(l, 1);

  script->remove_timer(callback_name);

  return 0;
}

/**
 * @brief Stops all timers of the script.
 * @param l the Lua context that is calling this function
 */
int Script::main_api_timer_stop_all(lua_State *l) {

  Script *script;
  called_by_script(l, 0, &script);

  script->remove_all_timers();

  return 0;
}

/**
 * @brief Creates a sprite that will be stored by your script.
 *
 * - Argument 1 (string): the sprite name (i.e. the name of its animation set)
 * - Return value (string): a handle to the sprite created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_sprite_create(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  const std::string& animation_set_id = luaL_checkstring(l, 1);

  Sprite* sprite = new Sprite(animation_set_id);
  int sprite_handle = script->create_sprite_handle(*sprite);
  script->unassigned_sprites[sprite_handle] = sprite;
  lua_pushinteger(l, sprite_handle);

  return 1;
}

/**
 * @brief Returns the current animation of a sprite.
 *
 * - Argument 1 (sprite): a sprite
 * - Return value (string): name of the current animation
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_sprite_get_animation(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int sprite_handle = luaL_checkinteger(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_handle);
  const std::string animation_name = sprite.get_current_animation();
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
int Script::main_api_sprite_set_animation(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  const std::string &animation_name = luaL_checkstring(l, 2);

  Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_get_direction(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int sprite_handle = luaL_checkinteger(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_set_direction(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  int direction = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_get_frame(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int sprite_handle = luaL_checkinteger(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_set_frame(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  int frame = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_get_frame_delay(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int sprite_handle = luaL_checkinteger(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_set_frame_delay(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  uint32_t delay = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_is_paused(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int sprite_handle = luaL_checkinteger(l, 1);

  const Sprite &sprite = script->get_sprite(sprite_handle);
  lua_pushboolean(l, sprite.is_paused() ? 1 : 0);

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
int Script::main_api_sprite_set_paused(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  bool paused = lua_toboolean(l, 2) != 0;

  Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_set_animation_ignore_suspend(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  bool ignore_suspend = lua_toboolean(l, 2) != 0;

  Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_fade(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  int direction = luaL_checkinteger(l, 2);

  Sprite &sprite = script->get_sprite(sprite_handle);
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
int Script::main_api_sprite_synchronize(lua_State *l) {


  Script* script;
  called_by_script(l, 2, &script);
  int sprite_handle = luaL_checkinteger(l, 1);
  Sprite& sprite = script->get_sprite(sprite_handle);

  if (!lua_isnil(l, 2)) {
    int reference_sprite_handle = luaL_checkinteger(l, 2);
    Sprite& reference_sprite = script->get_sprite(reference_sprite_handle);
    sprite.set_synchronized_to(&reference_sprite);
  }
  else {
    sprite.set_synchronized_to(NULL);
  }

  return 0;
}

/**
 * @brief Creates a movement of type PixelMovement that will be accessible from the script.
 *
 * - Argument 1 (string): the pixel-by-pixel trajectory of the movement
 * - Argument 2 (int): the delay in millisecond between each step of the trajectory
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_pixel_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &trajectory = luaL_checkstring(l, 1);
  uint32_t delay = luaL_checkinteger(l, 2);

  Movement *movement = new PixelMovement(trajectory, delay, false, false);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type RandomMovement that will be accessible from the script.
 *
 * - Argument 1 (int): the speed of the movement in pixels per second
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_random_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int speed = luaL_checkinteger(l, 1);

  Movement *movement = new RandomMovement(speed, 0);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type PathMovement that will be accessible from the script.
 *
 * - Argument 1 (string): the square-by-square trajectory of the movement
 * - Argument 2 (int): the speed in pixels per second
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_path_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  const std::string &path = luaL_checkstring(l, 1);
  int speed = luaL_checkinteger(l, 2);

  Movement *movement = new PathMovement(path, speed, false, false, false);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type RandomPathMovement that will be accessible from the script.
 *
 * - Argument 1 (int): the speed in pixels per second
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_random_path_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int speed = luaL_checkinteger(l, 1);

  Movement *movement = new RandomPathMovement(speed);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type PathFindingMovement that will be accessible from the script.
 *
 * The movement will compute a path to the hero.
 * - Argument 1 (int): the speed in pixels per second
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_path_finding_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 1, &script);
  int speed = luaL_checkinteger(l, 1);

  PathFindingMovement *movement = new PathFindingMovement(&script->get_game().get_hero(), speed);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type RectilinearMovement that will be accessible from the script.
 *
 * - Argument 1 (int): the speed in pixels per second
 * - Argument 2 (float): angle of the speed vector in radians
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_rectilinear_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int speed = luaL_checkinteger(l, 1);
  double angle = luaL_checknumber(l, 2);

  RectilinearMovement *movement = new RectilinearMovement(false);
  movement->set_speed(speed);
  movement->set_angle(angle);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type TemporalMovement that will be accessible from the script.
 *
 * - Argument 1 (int): the speed in pixels per second
 * - Argument 2 (float): angle of the speed vector in radians
 * - Argument 3 (int): the duration of the movement in milliseconds
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_temporal_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  int speed = luaL_checkinteger(l, 1);
  double angle = luaL_checknumber(l, 2);
  uint32_t duration = luaL_checkinteger(l, 3);

  TemporalMovement *movement = new TemporalMovement(speed, angle, duration);
  movement->set_speed(speed);
  if (speed != 0) {
    movement->set_angle(angle);
  }
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type CircleMovement that will be accessible from the script.
 *
 * - Argument 1 (integer): type of the center entity
 * (must match the enumeration EntityType)
 * - Argument 2 (string): name of the center entity
 * - Argument 3 (int): the radius of the circle in pixels
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_circle_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  int center_type = luaL_checkinteger(l, 1);
  const std::string& center_name = luaL_checkstring(l, 2);
  int radius = luaL_checkinteger(l, 3);

  MapEntity* center_entity = script->get_map().get_entities().
      get_entity(EntityType(center_type), center_name);

  CircleMovement *movement = new CircleMovement();
  movement->set_center(center_entity);
  movement->set_radius(radius);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Creates a movement of type JumpMovement that will be accessible from the script.
 *
 * - Argument 1 (int): direction of the jump (0 to 7)
 * - Argument 2 (int): length of the jump in pixels
 * - Return value (movement): a handle to the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_jump_movement_create(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int direction8 = luaL_checkinteger(l, 1);
  int length = luaL_checkinteger(l, 2);

  Movement *movement = new JumpMovement(direction8, length, 0, false);
  int movement_handle = script->create_movement_handle(*movement);
  lua_pushinteger(l, movement_handle);

  return 1;
}

/**
 * @brief Returns the value of a property of a movement.
 *
 * - Argument 1 (movement): a movement
 * - Argument 2 (string): key of the property to get (the keys accepted depend of the movement type)
 * - Return value (string): the value of this property (warning: always a string)
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_movement_get_property(lua_State *l) {

  Script *script;
  called_by_script(l, 2, &script);
  int movement_handle = luaL_checkinteger(l, 1);
  const std::string &key = luaL_checkstring(l, 2);

  Movement &movement = script->get_movement(movement_handle);
  const std::string &value = movement.get_property(key);
  lua_pushstring(l, value.c_str());

  return 1;
}

/**
 * @brief Sets a property of a movement.
 *
 * - Argument 1 (movement): a movement
 * - Argument 2 (string): key of the property to set (the keys accepted depend of the movement type)
 * - Argument 3 (string, integer or boolean): the new value of this property
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_movement_set_property(lua_State *l) {

  Script *script;
  called_by_script(l, 3, &script);
  int movement_handle = luaL_checkinteger(l, 1);
  const std::string &key = luaL_checkstring(l, 2);

  std::string value;
  if (lua_isstring(l, 3)) {
    value = lua_tostring(l, 3);
  }
  else if (lua_isnumber(l, 3)) {
    double v = lua_tointeger(l, 3);
    std::ostringstream oss;
    if (std::fabs(v - (int) v) < 1e-6) {
      oss << (int) v;
    }
    else {
      oss << v;
    }
    value = oss.str();
  }
  else if (lua_isboolean(l, 3)) {
    value = lua_toboolean(l, 3) ? "1" : "0";
  }
  else {
    Debug::die("Invalid type of value in movement_set_property");
  }

  Movement &movement = script->get_movement(movement_handle);
  movement.set_property(key, value);

  return 0;
}

/**
 * @brief Returns whether there would be a collision if the entity controlled by a movement
 * object was translated with the given parameters.
 *
 * - Argument 1 (movement): a movement
 * - Argument 2 (integer): x translation in pixels
 * - Argument 3 (integer): y translation in pixels
 * - Return value (boolean): true if this translation would make the entity overlap obstacles
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_movement_test_obstacles(lua_State *l) {

  Script* script;
  called_by_script(l, 3, &script);
  int movement_handle = luaL_checkinteger(l, 1);
  int dx = luaL_checkinteger(l, 2);
  int dy = luaL_checkinteger(l, 3);

  Movement& movement = script->get_movement(movement_handle);
  bool result = movement.test_collision_with_obstacles(dx, dy);
  lua_pushboolean(l, result);

  return 1;
}

/**
 * @brief Returns the angle between the specified vector and the x axis.
 *
 * - Argument 1 (integer): x coordinate of the first point of the vector
 * - Argument 2 (integer): y coordinate of the first point of the vector
 * - Argument 3 (integer): x coordinate of the second point of the vector
 * - Argument 4 (integer): y coordinate of the second point of the vector
 * - Return value (float): the angle between the vector and the x axis in radians
 *
 * @param l the Lua context that is calling this function
 */
int Script::main_api_get_angle(lua_State *l) {

  Script* script;
  called_by_script(l, 4, &script);
  int x1 = luaL_checkinteger(l, 1);
  int y1 = luaL_checkinteger(l, 2);
  int x2 = luaL_checkinteger(l, 3);
  int y2 = luaL_checkinteger(l, 4);

  double angle = Geometry::get_angle(x1, y1, x2, y2);
  lua_pushnumber(l, angle);

  return 1;
}

