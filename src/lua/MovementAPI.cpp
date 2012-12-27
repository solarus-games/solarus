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
#include <cmath>
#include <sstream>
#include "lua/LuaContext.h"
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
#include "MainLoop.h"
#include "Game.h"
#include "Map.h"

const std::string LuaContext::movement_module_name = "sol.movement";
const std::string LuaContext::movement_straight_module_name = "sol.movement.straight";
const std::string LuaContext::movement_target_module_name = "sol.movement.target";
const std::string LuaContext::movement_random_module_name = "sol.movement.random";
const std::string LuaContext::movement_path_module_name = "sol.movement.path";
const std::string LuaContext::movement_random_path_module_name = "sol.movement.random_path";
const std::string LuaContext::movement_path_finding_module_name = "sol.movement.path_finding";
const std::string LuaContext::movement_circle_module_name = "sol.movement.circle";
const std::string LuaContext::movement_jump_module_name = "sol.movement.jump";
const std::string LuaContext::movement_pixel_module_name = "sol.movement.pixel";

/**
 * @brief Initializes the movement features provided to Lua.
 */
void LuaContext::register_movement_module() {

  // sol.movement
  static const luaL_Reg movement_functions[] = {
      { "create", movement_api_create },
      { NULL, NULL }
  };
  register_functions(movement_module_name, movement_functions);

  // methods common to all movement types
  static const luaL_Reg common_methods[] = {
      { "get_ignore_obstacles", movement_api_get_ignore_obstacles },
      { "set_ignore_obstacles", movement_api_set_ignore_obstacles },
      { "get_direction4", movement_api_get_direction4 },
      { NULL, NULL }
  };
  static const luaL_Reg common_metamethods[] = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { NULL, NULL }
  };

  // straight movement
  static const luaL_Reg straight_movement_methods[] = {
      { "get_speed", straight_movement_api_get_speed },
      { "set_speed", straight_movement_api_set_speed },
      { "get_angle", straight_movement_api_get_angle },
      { "set_angle", straight_movement_api_set_angle },
      { "get_max_distance", straight_movement_api_get_max_distance },
      { "set_max_distance", straight_movement_api_set_max_distance },
      { "is_smooth", straight_movement_api_is_smooth },
      { "set_smooth", straight_movement_api_set_smooth },
      { NULL, NULL }
  };
  register_functions(movement_straight_module_name, common_methods);
  register_type(movement_straight_module_name, straight_movement_methods,
      common_metamethods);

  // random movement
  static const luaL_Reg random_movement_methods[] = {
      { "get_speed", random_movement_api_get_speed },
      { "set_speed", random_movement_api_set_speed },
      { "get_angle", random_movement_api_get_angle },
      { "get_max_distance", random_movement_api_get_max_distance },
      { "set_max_distance", random_movement_api_set_max_distance },
      { "is_smooth", random_movement_api_is_smooth },
      { "set_smooth", random_movement_api_set_smooth },
      { NULL, NULL }
  };
  register_functions(movement_random_module_name, common_methods);
  register_type(movement_random_module_name, random_movement_methods,
      common_metamethods);

  // target movement
  static const luaL_Reg target_movement_methods[] = {
      { "set_target", target_movement_api_set_target },
      { "get_speed", target_movement_api_get_speed },
      { "set_speed", target_movement_api_set_speed },
      { "get_angle", target_movement_api_get_angle },
      { "is_smooth", target_movement_api_is_smooth },
      { "set_smooth", target_movement_api_set_smooth },
      { NULL, NULL }
  };
  register_functions(movement_target_module_name, common_methods);
  register_type(movement_target_module_name, target_movement_methods,
      common_metamethods);

  // path movement
  static const luaL_Reg path_movement_methods[] = {
      { "get_path", path_movement_api_get_path },
      { "set_path", path_movement_api_set_path },
      { "get_speed", path_movement_api_get_speed },
      { "set_speed", path_movement_api_set_speed },
      { "get_loop", path_movement_api_get_loop },
      { "set_loop", path_movement_api_set_loop },
      { "get_snap_to_grid", path_movement_api_get_snap_to_grid },
      { "set_snap_to_grid", path_movement_api_set_snap_to_grid },
      { NULL, NULL }
  };
  register_functions(movement_path_module_name, common_methods);
  register_type(movement_path_module_name, path_movement_methods,
      common_metamethods);

  // random path movement
  static const luaL_Reg random_path_movement_methods[] = {
      { "get_speed", random_path_movement_api_get_speed },
      { "set_speed", random_path_movement_api_set_speed },
      { NULL, NULL }
  };
  register_functions(movement_random_path_module_name, common_methods);
  register_type(movement_random_path_module_name, random_path_movement_methods,
      common_metamethods);

  // path finding movement
  static const luaL_Reg path_finding_movement_methods[] = {
      { "set_target", path_finding_movement_api_set_target },
      { "get_speed", path_finding_movement_api_get_speed },
      { "set_speed", path_finding_movement_api_set_speed },
      { NULL, NULL }
  };
  register_functions(movement_path_finding_module_name, common_methods);
  register_type(movement_path_finding_module_name, path_finding_movement_methods,
      common_metamethods);

  // circle movement
  static const luaL_Reg circle_movement_methods[] = {
      { "set_center", circle_movement_api_set_center },
      { "get_radius", circle_movement_api_get_radius },
      { "set_radius", circle_movement_api_set_radius },
      { "get_radius_speed", circle_movement_api_get_radius_speed },
      { "set_radius_speed", circle_movement_api_set_radius_speed },
      { "is_clockwise", circle_movement_api_is_clockwise },
      { "set_clockwise", circle_movement_api_set_clockwise },
      { "get_initial_angle", circle_movement_api_get_initial_angle },
      { "set_initial_angle", circle_movement_api_set_initial_angle },
      { "get_angle_speed", circle_movement_api_get_angle_speed },
      { "set_angle_speed", circle_movement_api_set_angle_speed },
      { "get_max_rotations", circle_movement_api_get_max_rotations },
      { "set_max_rotations", circle_movement_api_set_max_rotations },
      { "get_duration", circle_movement_api_get_duration },
      { "set_duration", circle_movement_api_set_duration },
      { "get_loop_delay", circle_movement_api_get_loop_delay },
      { "set_loop_delay", circle_movement_api_set_loop_delay },
      { NULL, NULL }
  };
  register_functions(movement_circle_module_name, common_methods);
  register_type(movement_circle_module_name, circle_movement_methods,
      common_metamethods);

  // jump movement
  static const luaL_Reg jump_movement_methods[] = {
      { "get_direction8", jump_movement_api_get_direction8 },
      { "set_direction8", jump_movement_api_set_direction8 },
      { "get_distance", jump_movement_api_get_distance },
      { "set_distance", jump_movement_api_set_distance },
      { "get_speed", jump_movement_api_get_speed },
      { "set_speed", jump_movement_api_set_speed },
      { NULL, NULL }
  };
  register_functions(movement_jump_module_name, common_methods);
  register_type(movement_jump_module_name, jump_movement_methods,
      common_metamethods);

  // pixel movement
  static const luaL_Reg pixel_movement_methods[] = {
      { "get_trajectory", pixel_movement_api_get_trajectory },
      { "set_trajectory", pixel_movement_api_set_trajectory },
      { "get_loop", pixel_movement_api_get_loop },
      { "set_loop", pixel_movement_api_set_loop },
      { "get_delay", pixel_movement_api_get_delay },
      { "set_delay", pixel_movement_api_set_delay },
      { NULL, NULL }
  };
  register_functions(movement_pixel_module_name, common_methods);
  register_type(movement_pixel_module_name, pixel_movement_methods,
      common_metamethods);
}

/**
 * @brief Returns whether a value is a userdata of type movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a entity.
 */
bool LuaContext::is_movement(lua_State* l, int index) {
  return is_straight_movement(l, index)
      || is_random_movement(l, index)
      || is_target_movement(l, index)
      || is_path_movement(l, index)
      || is_random_path_movement(l, index)
      || is_path_finding_movement(l, index)
      || is_circle_movement(l, index)
      || is_jump_movement(l, index)
      || is_pixel_movement(l, index);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * movement (of any subtype) and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the sprite
 */
Movement& LuaContext::check_movement(lua_State* l, int index) {

  Movement** movement = NULL;

  if (is_movement(l, index)) {
    movement = static_cast<Movement**>(lua_touserdata(l, index));
  }
  else {
    luaL_typerror(l, index, "movement");
  }

  return **movement;
}

/**
 * @brief Pushes a movement userdata onto the stack.
 * @param l a Lua context
 * @param movement a movement
 */
void LuaContext::push_movement(lua_State* l, Movement& movement) {

  movement.set_lua_context(&get_lua_context(l));  // To make callbacks work.
  push_userdata(l, movement);
}

/**
 * @brief Implementation of \ref lua_api_movement_create.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::movement_api_create(lua_State* l) {

  LuaContext& lua_context = get_lua_context(l);
  const std::string& type = luaL_checkstring(l, 1);

  Movement* movement = NULL;
  if (type == "straight") {
    movement = new StraightMovement(false, true);
  }
  else if (type == "random") {
    movement = new RandomMovement(32);
  }
  else if (type == "target") {
    Game* game = lua_context.get_main_loop().get_game();
    if (game != NULL) {
      // If we are on a map, the default target is the hero.
      movement = new TargetMovement(
          &game->get_hero(), 96, false);
    }
    else {
      movement = new TargetMovement(0, 0, 32, false);
    }
  }
  else if (type == "path") {
    movement = new PathMovement("", 32, false, false, false);
  }
  else if (type == "random_path") {
    movement = new RandomPathMovement(32);
  }
  else if (type == "path_finding") {
    PathFindingMovement* path_finding_movement = new PathFindingMovement(32);
    Game* game = lua_context.get_main_loop().get_game();
    if (game != NULL) {
      // If we are on a map, the default target is the hero.
      path_finding_movement->set_target(game->get_hero());
    }
    movement = path_finding_movement;
  }
  else if (type == "circle") {
    movement = new CircleMovement(false);
  }
  else if (type == "jump") {
    movement = new JumpMovement(0, 0, 0, false);
  }
  else if (type == "pixel") {
    movement = new PixelMovement("", 30, false, false);
  }
  else {
    luaL_argerror(l, 1, "should be one of: "
        "\"straight\", "
        "\"random\", "
        "\"target\", "
        "\"path\", "
        "\"random_path\", "
        "\"path_finding\", "
        "\"circle\", "
        "\"jump\" or "
        "\"pixel\"");
  }

  push_movement(l, *movement);

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_movement_get_ignore_obstacles.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::movement_api_get_ignore_obstacles(lua_State* l) {

  Movement& movement = check_movement(l, 1);
  lua_pushboolean(l, movement.are_obstacles_ignored());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_movement_set_ignore_obstacles.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::movement_api_set_ignore_obstacles(lua_State* l) {

  Movement& movement = check_movement(l, 1);
  bool ignore_obstacles = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    ignore_obstacles = lua_toboolean(l, 2);
  }

  movement.set_ignore_obstacles(ignore_obstacles);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_movement_get_direction4.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::movement_api_get_direction4(lua_State* l) {

  Movement& movement = check_movement(l, 1);
  lua_pushinteger(l, movement.get_displayed_direction4());
  return 1;
}

/**
 * @brief Returns whether a value is a userdata of type straight movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a straight movement.
 */
bool LuaContext::is_straight_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_straight_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * straight movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
StraightMovement& LuaContext::check_straight_movement(lua_State* l, int index) {

  return static_cast<StraightMovement&>(
      check_userdata(l, index, movement_straight_module_name));
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_get_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_get_speed(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  lua_pushinteger(l, movement.get_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_set_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_speed(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  int speed = luaL_checkinteger(l, 2);
  movement.set_speed(speed);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_get_angle.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_get_angle(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  lua_pushnumber(l, movement.get_angle());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_set_angle.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_angle(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  double angle = luaL_checknumber(l, 2);
  movement.set_angle(angle);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_get_max_distance.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_get_max_distance(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  lua_pushinteger(l, movement.get_max_distance());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_set_max_distance.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_max_distance(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  int max_distance = luaL_checkinteger(l, 2);
  movement.set_max_distance(max_distance);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_is_smooth.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_is_smooth(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  lua_pushboolean(l, movement.is_smooth());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_straight_movement_set_smooth.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_smooth(lua_State* l) {

  StraightMovement& movement = check_straight_movement(l, 1);
  bool smooth = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    smooth = lua_toboolean(l, 2);
  }
  movement.set_smooth(smooth);

  return 0;
}

/**
 * @brief Returns whether a value is a userdata of type random movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a random movement.
 */
bool LuaContext::is_random_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_random_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * random movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
RandomMovement& LuaContext::check_random_movement(lua_State* l, int index) {
  return static_cast<RandomMovement&>(
      check_userdata(l, index, movement_random_module_name));
}

/**
 * @brief Implementation of \ref lua_api_random_movement_get_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_movement_api_get_speed(lua_State* l) {

  RandomMovement& movement = check_random_movement(l, 1);
  lua_pushinteger(l, movement.get_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_random_movement_set_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_movement_api_set_speed(lua_State* l) {

  RandomMovement& movement = check_random_movement(l, 1);
  int speed = luaL_checkinteger(l, 2);
  movement.set_speed(speed);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_random_movement_get_angle.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_movement_api_get_angle(lua_State* l) {

  RandomMovement& movement = check_random_movement(l, 1);
  lua_pushnumber(l, movement.get_angle());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_random_movement_get_max_distance.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_movement_api_get_max_distance(lua_State* l) {

  RandomMovement& movement = check_random_movement(l, 1);
  lua_pushinteger(l, movement.get_max_distance());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_random_movement_set_max_distance.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_movement_api_set_max_distance(lua_State* l) {

  RandomMovement& movement = check_random_movement(l, 1);
  int max_distance = luaL_checkinteger(l, 2);
  movement.set_max_distance(max_distance);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_random_movement_is_smooth.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_movement_api_is_smooth(lua_State* l) {

  RandomMovement& movement = check_random_movement(l, 1);
  lua_pushboolean(l, movement.is_smooth());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_random_movement_set_smooth.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_movement_api_set_smooth(lua_State* l) {

  RandomMovement& movement = check_random_movement(l, 1);
  bool smooth = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    smooth = lua_toboolean(l, 2);
  }
  movement.set_smooth(smooth);

  return 0;
}

/**
 * @brief Returns whether a value is a userdata of type target movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a target movement.
 */
bool LuaContext::is_target_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_target_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * target movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
TargetMovement& LuaContext::check_target_movement(lua_State* l, int index) {
  return static_cast<TargetMovement&>(
      check_userdata(l, index, movement_target_module_name));
}

/**
 * @brief Implementation of \ref lua_api_target_movement_set_target.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::target_movement_api_set_target(lua_State* l) {

  TargetMovement& movement = check_target_movement(l, 1);
  if (lua_isnumber(l, 3)) {
    // the target is a fixed point
    int x = luaL_checkinteger(l, 2);
    int y = luaL_checkinteger(l, 3);
    movement.set_target(x, y);
  }
  else {
    // the target is an entity
    MapEntity& target = check_entity(l, 2);
    movement.set_target(&target);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_target_movement_get_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::target_movement_api_get_speed(lua_State* l) {

  TargetMovement& movement = check_target_movement(l, 1);
  lua_pushinteger(l, movement.get_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_target_movement_set_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::target_movement_api_set_speed(lua_State* l) {

  TargetMovement& movement = check_target_movement(l, 1);
  int speed = luaL_checkinteger(l, 2);
  movement.set_moving_speed(speed);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_target_movement_get_angle.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::target_movement_api_get_angle(lua_State* l) {

  TargetMovement& movement = check_target_movement(l, 1);
  lua_pushnumber(l, movement.get_angle());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_target_movement_is_smooth.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::target_movement_api_is_smooth(lua_State* l) {

  TargetMovement& movement = check_target_movement(l, 1);
  lua_pushboolean(l, movement.is_smooth());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_target_movement_set_smooth.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::target_movement_api_set_smooth(lua_State* l) {

  TargetMovement& movement = check_target_movement(l, 1);
  bool smooth = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    smooth = lua_toboolean(l, 2);
  }
  movement.set_smooth(smooth);

  return 0;
}

/**
 * @brief Returns whether a value is a userdata of type path movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a path movement.
 */
bool LuaContext::is_path_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_path_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * path movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
PathMovement& LuaContext::check_path_movement(lua_State* l, int index) {
  return static_cast<PathMovement&>(
      check_userdata(l, index, movement_path_module_name));
}

/**
 * @brief Implementation of \ref lua_api_path_movement_get_path.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_path(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);

  const std::string& path = movement.get_path();
  // build a Lua array containing the path
  lua_settop(l, 1);
  lua_newtable(l);
  for (size_t i = 0; i < path.size(); i++) {
    int direction8 = path[i] - '0';
    lua_pushinteger(l, direction8);
    lua_rawseti(l, 2, i);
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_path_movement_set_path.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_path(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);
  luaL_checktype(l, 2, LUA_TTABLE);

  // build the path as a string from the Lua table
  std::string path = "";
  lua_pushnil(l); // first key
  while (lua_next(l, 2) != 0) {
    int direction8 = luaL_checkinteger(l, 4);
    path += ('0' + direction8);
    lua_pop(l, 1); // pop the value, let the key for the iteration
  }
  movement.set_path(path);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_path_movement_get_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_speed(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);
  lua_pushinteger(l, movement.get_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_path_movement_set_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_speed(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);
  int speed = luaL_checkinteger(l, 2);
  movement.set_speed(speed);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_path_movement_get_loop.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_loop(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);
  lua_pushboolean(l, movement.get_loop());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_path_movement_set_loop.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_loop(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);
  bool loop = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    loop = lua_toboolean(l, 2);
  }
  movement.set_loop(loop);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_path_movement_get_snap_to_grid.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_snap_to_grid(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);
  lua_pushboolean(l, movement.get_snap_to_grid());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_path_movement_set_snap_to_grid.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_snap_to_grid(lua_State* l) {

  PathMovement& movement = check_path_movement(l, 1);
  bool snap_to_grid = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    snap_to_grid = lua_toboolean(l, 2);
  }
  movement.set_snap_to_grid(snap_to_grid);

  return 0;
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * random path movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
RandomPathMovement& LuaContext::check_random_path_movement(lua_State* l, int index) {
  return static_cast<RandomPathMovement&>(
      check_userdata(l, index, movement_random_path_module_name));
}

/**
 * @brief Returns whether a value is a userdata of type random path movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a random path movement.
 */
bool LuaContext::is_random_path_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_random_path_module_name);
}

/**
 * @brief Implementation of \ref lua_api_random_path_movement_get_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_path_movement_api_get_speed(lua_State* l) {

  RandomPathMovement& movement = check_random_path_movement(l, 1);
  lua_pushinteger(l, movement.get_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_random_path_movement_set_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::random_path_movement_api_set_speed(lua_State* l) {

  RandomPathMovement& movement = check_random_path_movement(l, 1);
  int speed = luaL_checkinteger(l, 2);
  movement.set_speed(speed);
  return 0;
}

/**
 * @brief Returns whether a value is a userdata of type path finding movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a path finding  movement.
 */
bool LuaContext::is_path_finding_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_path_finding_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * path finding movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
PathFindingMovement& LuaContext::check_path_finding_movement(lua_State* l, int index) {
  return static_cast<PathFindingMovement&>(
      check_userdata(l, index, movement_path_finding_module_name));
}

/**
 * @brief Implementation of \ref lua_api_path_finding_movement_set_target.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_finding_movement_api_set_target(lua_State* l) {

  PathFindingMovement& movement = check_path_finding_movement(l, 1);
  MapEntity& target = check_entity(l, 2);

  movement.set_target(target);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_path_finding_movement_get_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_finding_movement_api_get_speed(lua_State* l) {

  PathFindingMovement& movement = check_path_finding_movement(l, 1);
  lua_pushinteger(l, movement.get_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_path_finding_movement_set_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::path_finding_movement_api_set_speed(lua_State* l) {

  PathFindingMovement& movement = check_path_finding_movement(l, 1);
  int speed = luaL_checkinteger(l, 2);
  movement.set_speed(speed);
  return 0;
}

/**
 * @brief Returns whether a value is a userdata of type circle movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a circle movement.
 */
bool LuaContext::is_circle_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_circle_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * circle movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
CircleMovement& LuaContext::check_circle_movement(lua_State* l, int index) {
  return static_cast<CircleMovement&>(
      check_userdata(l, index, movement_circle_module_name));
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_center.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_center(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  if (lua_isnumber(l, 3)) {
    // the center is a fixed point
    int x = luaL_checkinteger(l, 2);
    int y = luaL_checkinteger(l, 3);
    movement.set_center(Rectangle(x, y));
  }
  else {
    // the center is an entity

    MapEntity& center = check_entity(l, 2);
    int dx = luaL_optinteger(l, 3, 0);
    int dy = luaL_optinteger(l, 4, 0);
    movement.set_center(center, dx, dy);
  }

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_get_radius.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_radius(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushinteger(l, movement.get_radius());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_radius.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_radius(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  int radius = luaL_checkinteger(l, 2);
  movement.set_radius(radius);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_get_radius_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_radius_speed(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushinteger(l, movement.get_radius_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_radius_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_radius_speed(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  int radius_speed = luaL_checkinteger(l, 2);
  movement.set_radius_speed(radius_speed);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_is_clockwise.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_is_clockwise(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushboolean(l, movement.is_clockwise());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_clockwise.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_clockwise(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  bool clockwise = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    clockwise = lua_toboolean(l, 2);
  }
  movement.set_clockwise(clockwise);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_get_initial_angle.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_initial_angle(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushnumber(l, movement.get_initial_angle());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_initial_angle.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_initial_angle(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  double initial_angle = luaL_checknumber(l, 2);
  movement.set_initial_angle(initial_angle);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_get_angle_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_angle_speed(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushinteger(l, movement.get_angle_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_angle_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_angle_speed(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  int angle_speed = luaL_checkinteger(l, 2);
  movement.set_angle_speed(angle_speed);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_get_max_rotations.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_max_rotations(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushinteger(l, movement.get_max_rotations());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_max_rotations.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_max_rotations(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  int max_rotations = luaL_checkinteger(l, 2);
  movement.set_max_rotations(max_rotations);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_get_duration.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_duration(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushinteger(l, movement.get_duration());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_duration.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_duration(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  int duration = luaL_checkinteger(l, 2);
  movement.set_duration(duration);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_get_loop_delay.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_loop_delay(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  lua_pushinteger(l, movement.get_loop());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_circle_movement_set_loop_delay.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_loop_delay(lua_State* l) {

  CircleMovement& movement = check_circle_movement(l, 1);
  int loop_delay = luaL_checkinteger(l, 2);
  movement.set_loop(loop_delay);
  return 0;
}

/**
 * @brief Returns whether a value is a userdata of type jump movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a jump movement.
 */
bool LuaContext::is_jump_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_jump_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * jump movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
JumpMovement& LuaContext::check_jump_movement(lua_State* l, int index) {
  return static_cast<JumpMovement&>(
      check_userdata(l, index, movement_jump_module_name));
}

/**
 * @brief Implementation of \ref lua_api_jump_movement_get_direction8.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::jump_movement_api_get_direction8(lua_State* l) {

  JumpMovement& movement = check_jump_movement(l, 1);
  lua_pushinteger(l, movement.get_direction8());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_jump_movement_set_direction8.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::jump_movement_api_set_direction8(lua_State* l) {

  JumpMovement& movement = check_jump_movement(l, 1);
  int direction8 = luaL_checkinteger(l, 2);
  movement.set_direction8(direction8);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_jump_movement_get_distance.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::jump_movement_api_get_distance(lua_State* l) {

  JumpMovement& movement = check_jump_movement(l, 1);
  lua_pushinteger(l, movement.get_distance());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_jump_movement_set_distance.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::jump_movement_api_set_distance(lua_State* l) {

  JumpMovement& movement = check_jump_movement(l, 1);
  int distance = luaL_checkinteger(l, 2);
  movement.set_distance(distance);
  return 0;
}

/**
 * @brief Implementation of \ref lua_api_jump_movement_get_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::jump_movement_api_get_speed(lua_State* l) {

  JumpMovement& movement = check_jump_movement(l, 1);
  lua_pushinteger(l, movement.get_speed());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_jump_movement_set_speed.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::jump_movement_api_set_speed(lua_State* l) {

  JumpMovement& movement = check_jump_movement(l, 1);
  int speed = luaL_checkinteger(l, 2);
  movement.set_speed(speed);
  return 0;
}

/**
 * @brief Returns whether a value is a userdata of type pixel movement.
 * @param l A Lua context.
 * @param index An index in the stack.
 * @return true if the value at this index is a pixel movement.
 */
bool LuaContext::is_pixel_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_pixel_module_name);
}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * pixel movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the movement
 */
PixelMovement& LuaContext::check_pixel_movement(lua_State* l, int index) {
  return static_cast<PixelMovement&>(
      check_userdata(l, index, movement_pixel_module_name));
}

/**
 * @brief Implementation of \ref lua_api_pixel_movement_get_trajectory.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_get_trajectory(lua_State* l) {

  PixelMovement& movement = check_pixel_movement(l, 1);

  const std::list<Rectangle>& trajectory = movement.get_trajectory();
  // build a Lua array containing the trajectory
  lua_settop(l, 1);
  lua_newtable(l);
  int i = 0;
  std::list<Rectangle>::const_iterator it;
  for (it = trajectory.begin(); it != trajectory.end(); it++) {
    const Rectangle& xy = *it;
    lua_newtable(l);
    lua_pushinteger(l, xy.get_x());
    lua_rawseti(l, 3, 1);
    lua_pushinteger(l, xy.get_y());
    lua_rawseti(l, 3, 2);
    lua_rawseti(l, 2, i);
    i++;
  }

  return 1;
}

/**
 * @brief Implementation of \ref lua_api_pixel_movement_set_trajectory.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_set_trajectory(lua_State* l) {

  PixelMovement& movement = check_pixel_movement(l, 1);
  luaL_checktype(l, 2, LUA_TTABLE);

  // build the trajectory as a string from the Lua table
  std::list<Rectangle> trajectory;
  lua_pushnil(l); // first key
  while (lua_next(l, 2) != 0) {
    luaL_checktype(l, 4, LUA_TTABLE);
    lua_rawgeti(l, 4, 1);
    lua_rawgeti(l, 4, 2);
    int x = luaL_checkinteger(l, 5);
    int y = luaL_checkinteger(l, 6);
    trajectory.push_back(Rectangle(x, y));
    lua_settop(l, 3); // let the key for the iteration
  }
  movement.set_trajectory(trajectory);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_pixel_movement_get_loop.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_get_loop(lua_State* l) {

  PixelMovement& movement = check_pixel_movement(l, 1);
  lua_pushboolean(l, movement.get_loop());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_pixel_movement_set_loop.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_set_loop(lua_State* l) {

  PixelMovement& movement = check_pixel_movement(l, 1);
  bool loop = true; // true if unspecified
  if (lua_isboolean(l, 2)) {
    loop = lua_toboolean(l, 2);
  }
  movement.set_loop(loop);

  return 0;
}

/**
 * @brief Implementation of \ref lua_api_pixel_movement_get_delay.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_get_delay(lua_State* l) {

  PixelMovement& movement = check_pixel_movement(l, 1);
  lua_pushinteger(l, movement.get_delay());
  return 1;
}

/**
 * @brief Implementation of \ref lua_api_pixel_movement_set_delay.
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_set_delay(lua_State* l) {

  PixelMovement& movement = check_pixel_movement(l, 1);
  int delay = luaL_checkinteger(l, 2);
  movement.set_delay(delay);
  return 0;
}

/**
 * @brief Calls the on_position_changed() method of a Lua movement.
 * @param movement A movement.
 */
void LuaContext::movement_on_position_changed(Movement& movement) {

  push_movement(l, movement);
  on_position_changed();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_obstacle_reached() method of a Lua movement.
 * @param movement A movement.
 */
void LuaContext::movement_on_obstacle_reached(Movement& movement) {

  push_movement(l, movement);
  on_obstacle_reached();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_changed() method of a Lua movement.
 * @param movement A movement.
 */
void LuaContext::movement_on_changed(Movement& movement) {

  push_movement(l, movement);
  on_changed();
  lua_pop(l, 1);
}

/**
 * @brief Calls the on_finished() method of a Lua movement.
 * @param movement A movement.
 */
void LuaContext::movement_on_finished(Movement& movement) {

  push_movement(l, movement);
  on_finished();
  lua_pop(l, 1);
}

