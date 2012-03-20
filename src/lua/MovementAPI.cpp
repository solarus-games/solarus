/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-engine.org
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
#include <cmath>
#include <sstream>
#include "lua/Script.h"
#include "movements/PixelMovement.h"
#include "movements/PathMovement.h"
#include "movements/RandomMovement.h"
#include "movements/RandomPathMovement.h"
#include "movements/PathFindingMovement.h"
#include "movements/TargetMovement.h"
#include "movements/CircleMovement.h"
#include "movements/JumpMovement.h"
#include "lowlevel/Debug.h"
#include "entities/Hero.h"
#include "entities/MapEntities.h"
#include "Game.h"
#include "Map.h"

const char* Script::movement_module_name = "sol.movement";

/**
 * @brief Initializes the movement features provided to Lua.
 */
void Script::initialize_movement_module() {

  static const luaL_Reg methods[] = {
      { "pixel_movement_create", movement_api_pixel_movement_create },
      { "random_movement_create", movement_api_random_movement_create },
      { "path_movement_create", movement_api_path_movement_create },
      { "random_path_movement_create", movement_api_random_path_movement_create },
      { "path_finding_movement_create", movement_api_path_finding_movement_create },
      { "target_movement_create", movement_api_target_movement_create },
      { "straight_movement_create", movement_api_straight_movement_create },
      { "circle_movement_create", movement_api_circle_movement_create },
      { "jump_movement_create", movement_api_jump_movement_create },
      { "get_property", movement_api_get_property },
      { "set_property", movement_api_set_property },
      { NULL, NULL }
  };

  static const luaL_Reg metamethods[] = {
      { "__gc", movement_meta_gc },
      { NULL, NULL }
  };

  // create a table and fill it with the methods
  luaL_register(l, movement_module_name, methods);
                                  // sol.movement

  // create the metatable for the type, add it to the Lua registry
  luaL_newmetatable(l, movement_module_name);
                                  // sol.movement mt
  // fill the metatable
  luaL_register(l, NULL, metamethods);
                                  // sol.movement mt
  lua_pushvalue(l, -2);
                                  // sol.movement mt sol.movement
  // metatable.__index = sol.movement
  lua_setfield(l, -2, "__index");
                                  // sol.movement mt
  lua_pop(l, 2);
                                  // --
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the sprite
 */
Movement& Script::check_movement(lua_State* l, int index) {

  if (index < 0) {
    // ensure a positive index
    index = lua_gettop(l) + index + 1;
  }

  Movement** movement = (Movement**) luaL_checkudata(l, index, movement_module_name);
  return **movement;
}

/**
 * @brief Pushes a movement userdata onto the stack.
 * @param l a Lua context
 * @param movement a movement
 */
void Script::push_movement(lua_State* l, Movement& movement) {

  Script& script = get_script(l);

  script.increment_refcount(&movement);
                                  // ...
  Movement** block_adress = (Movement**) lua_newuserdata(l, sizeof(Movement*));
  *block_adress = &movement;
                                  // ... movement
  luaL_getmetatable(l, movement_module_name);
                                  // ... movement mt
  lua_setmetatable(l, -2);
                                  // ... movement
}

/**
 * @brief Finalizes a movement.
 *
 * - Argument 1: a movement
 *
 * @param l a Lua state
 * @return number of values to return to Lua
 */
int Script::movement_meta_gc(lua_State* l) {

  Script& script = get_script(l);

  Movement* movement = *((Movement**) luaL_checkudata(l, 1, movement_module_name));
  script.decrement_refcount(movement);

  return 0;
}

/**
 * @brief Creates a movement of type PixelMovement.
 *
 * - Argument 1 (string): the pixel-by-pixel trajectory of the movement
 * - Argument 2 (int): the delay in milliseconds between each step of the trajectory
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_pixel_movement_create(lua_State* l) {

  Script& script = get_script(l);

  const std::string& trajectory = luaL_checkstring(l, 1);
  uint32_t delay = luaL_checkinteger(l, 2);

  Movement* movement = new PixelMovement(trajectory, delay, false, false);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type RandomMovement.
 *
 * - Argument 1 (int): the speed of the movement in pixels per second
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_random_movement_create(lua_State* l) {

  Script& script = get_script(l);

  int speed = luaL_checkinteger(l, 1);

  Movement* movement = new RandomMovement(speed, 0);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type PathMovement.
 *
 * - Argument 1 (string): the square-by-square trajectory of the movement
 * - Argument 2 (int): the speed in pixels per second
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_path_movement_create(lua_State* l) {

  Script& script = get_script(l);

  const std::string& path = luaL_checkstring(l, 1);
  int speed = luaL_checkinteger(l, 2);

  Movement* movement = new PathMovement(path, speed, false, false, false);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type RandomPathMovement.
 *
 * - Argument 1 (int): the speed in pixels per second
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_random_path_movement_create(lua_State* l) {

  Script& script = get_script(l);

  int speed = luaL_checkinteger(l, 1);

  Movement* movement = new RandomPathMovement(speed);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type PathFindingMovement.
 *
 * The movement will compute a path to the hero.
 * - Argument 1 (int): the speed in pixels per second
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_path_finding_movement_create(lua_State* l) {

  Script& script = get_script(l);

  int speed = luaL_checkinteger(l, 1);

  Movement* movement = new PathFindingMovement(script.get_game().get_hero(), speed);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type TargetMovement.
 *
 * By default, the target is the hero. Alternatively, you can target
 * arbitrary coordinates.
 *
 * - Argument 1 (int): the speed in pixels per second
 * - Optional argument 2 (int): x coordinate of the point to target
 * (if unspecified, the target will be the hero)
 * - Optional argument 3 (int): y coordinate of the point to target
 * (if unspecified, the target will be the hero)
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_target_movement_create(lua_State* l) {

  Script& script = get_script(l);

  int speed = luaL_checkinteger(l, 1);

  TargetMovement* movement;
  if (lua_gettop(l) == 3) {
    // target a fixed point
    int x = luaL_checkinteger(l, 2);
    int y = luaL_checkinteger(l, 3);
    movement = new TargetMovement(x, y, speed);
  }
  else {
    // by default, target the hero
    Hero& hero = script.get_game().get_hero();
    movement = new TargetMovement(&hero, speed);
  }
  movement->set_ignore_obstacles(false);
  movement->set_speed(speed);

  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type StraightMovement.
 *
 * - Argument 1 (int): the speed in pixels per second
 * - Argument 2 (float): angle of the speed vector in radians
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_straight_movement_create(lua_State* l) {

  Script& script = get_script(l);

  int speed = luaL_checkinteger(l, 1);
  double angle = luaL_checknumber(l, 2);

  StraightMovement* movement = new StraightMovement(false, false);
  movement->set_speed(speed);
  movement->set_angle(angle);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type CircleMovement that will be accessible from the script.
 *
 * - Argument 1 (integer): type of the center entity
 * (must match the enumeration EntityType)
 * - Argument 2 (string): name of the center entity
 * - Argument 3 (int): the radius of the circle in pixels
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_circle_movement_create(lua_State* l) {

  Script& script = get_script(l);

  int center_type = luaL_checkinteger(l, 1);
  const std::string& center_name = luaL_checkstring(l, 2);
  int radius = luaL_checkinteger(l, 3);

  MapEntity* center_entity = script.get_map().get_entities().
      get_entity(EntityType(center_type), center_name);

  CircleMovement* movement = new CircleMovement(true);
  movement->set_center(center_entity);
  movement->set_radius(radius);

  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Creates a movement of type JumpMovement that will be accessible from the script.
 *
 * - Argument 1 (int): direction of the jump (0 to 7)
 * - Argument 2 (int): length of the jump in pixels
 * - Return value (movement): the movement created
 *
 * @param l the Lua context that is calling this function
 */
int Script::movement_api_jump_movement_create(lua_State* l) {

  Script& script = get_script(l);

  int direction8 = luaL_checkinteger(l, 1);
  int length = luaL_checkinteger(l, 2);

  Movement* movement = new JumpMovement(direction8, length, 0, false);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);

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
int Script::movement_api_get_property(lua_State* l) {

  Movement& movement = check_movement(l, 1);
  const std::string& key = luaL_checkstring(l, 2);

  const std::string& value = movement.get_property(key);
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
int Script::movement_api_set_property(lua_State* l) {

  Movement& movement = check_movement(l, 1);
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

  movement.set_property(key, value);

  return 0;
}

