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

const std::string Script::movement_module_name = "sol.movement";
const std::string Script::straight_movement_module_name = "sol.movement.straight";
const std::string Script::target_movement_module_name = "sol.movement.target";
const std::string Script::random_movement_module_name = "sol.movement.random";
const std::string Script::path_movement_module_name = "sol.movement.path";
const std::string Script::random_path_movement_module_name = "sol.movement.random_path";
const std::string Script::path_finding_movement_module_name = "sol.movement.path_finding";
const std::string Script::circle_movement_module_name = "sol.movement.circle";
const std::string Script::jump_movement_module_name = "sol.movement.jump";
const std::string Script::pixel_movement_module_name = "sol.movement.pixel";

/**
 * @brief Initializes the movement features provided to Lua.
 */
void Script::register_movement_module() {

  // sol.movement
  static const luaL_Reg movement_functions[] = {
      { "create", movement_api_create },
      { NULL, NULL }
  };
  register_functions(movement_module_name, movement_functions);

  // methods common to all movement types
  static const luaL_Reg common_methods[] = {
      { "create", movement_api_create },
      { "get_ignore_obstacles", movement_api_get_ignore_obstacles },
      { "set_ignore_obstacles", movement_api_set_ignore_obstacles },
      { "get_direction4", movement_api_get_direction4 },
      { NULL, NULL }
  };
  static const luaL_Reg common_metamethods[] = {
      { "__gc", userdata_meta_gc },
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
  register_functions(straight_movement_module_name, common_methods);
  register_type(straight_movement_module_name, straight_movement_methods,
      common_metamethods);

  // random movement
  static const luaL_Reg random_movement_methods[] = {
      { "get_speed", random_movement_api_get_speed },
      { "set_speed", random_movement_api_set_speed },
      { "get_angle", random_movement_api_get_angle },
      { "get_max_distance", random_movement_api_get_max_distance },
      { "set_max_distance", random_movement_api_set_max_distance },
      { NULL, NULL }
  };
  register_functions(random_movement_module_name, common_methods);
  register_type(random_movement_module_name, random_movement_methods,
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
  register_functions(target_movement_module_name, common_methods);
  register_type(target_movement_module_name, target_movement_methods,
      common_metamethods);

  // path movement
  static const luaL_Reg path_movement_methods[] = {
      { "get_path", path_movement_api_get_path },
      { "set_path", path_movement_api_set_path },
      { "get_speed", path_movement_api_get_speed },
      { "set_speed", path_movement_api_set_speed },
      { "get_snap_to_grid", path_movement_api_get_snap_to_grid },
      { "set_snap_to_grid", path_movement_api_set_snap_to_grid },
      { NULL, NULL }
  };
  register_functions(path_movement_module_name, common_methods);
  register_type(path_movement_module_name, path_movement_methods,
      common_metamethods);

  // random path movement
  static const luaL_Reg random_path_movement_methods[] = {
      { "get_speed", random_path_movement_api_get_speed },
      { "set_speed", random_path_movement_api_set_speed },
      { NULL, NULL }
  };
  register_functions(random_path_movement_module_name, common_methods);
  register_type(random_path_movement_module_name, random_path_movement_methods,
      common_metamethods);

  // path finding movement
  static const luaL_Reg path_finding_movement_methods[] = {
      { "set_target", path_finding_movement_api_set_target },
      { "get_speed", path_finding_movement_api_get_speed },
      { "set_speed", path_finding_movement_api_set_speed },
      { NULL, NULL }
  };
  register_functions(path_finding_movement_module_name, common_methods);
  register_type(path_finding_movement_module_name, path_finding_movement_methods,
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
  register_functions(circle_movement_module_name, common_methods);
  register_type(circle_movement_module_name, circle_movement_methods,
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
  register_functions(jump_movement_module_name, common_methods);
  register_type(jump_movement_module_name, jump_movement_methods,
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
  register_functions(pixel_movement_module_name, common_methods);
  register_type(pixel_movement_module_name, pixel_movement_methods,
      common_metamethods);

}

/**
 * @brief Checks that the userdata at the specified index of the stack is a
 * movement and returns it.
 * @param l a Lua context
 * @param index an index in the stack
 * @return the sprite
 */
Movement& Script::check_movement(lua_State* l, int index) {
  return static_cast<Movement&>(check_userdata(l, index, movement_module_name));
}

/**
 * @brief Pushes a movement userdata onto the stack.
 * @param l a Lua context
 * @param movement a movement
 */
void Script::push_movement(lua_State* l, Movement& movement) {
  push_userdata(l, movement);
}

/**
 * @brief TODO
 * @param l the Lua context that is calling this function
 * @return number of values to return to Lua
 */
int Script::movement_api_create(lua_State* l) {

  /*
  Script& script = get_script(l);

  const std::string& trajectory = luaL_checkstring(l, 1);
  uint32_t delay = luaL_checkinteger(l, 2);

  Movement* movement = new PixelMovement(trajectory, delay, false, false);
  script.set_created(movement);
  movement->set_creator_script(&script);
  push_movement(l, *movement);
  */

  return 1;
}

