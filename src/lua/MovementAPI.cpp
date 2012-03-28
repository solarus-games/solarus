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

  static const luaL_Reg movement_methods[] = {
      { "create", movement_api_create },
      { "get_ignore_obstacles", movement_api_get_ignore_obstacles },
      { "set_ignore_obstacles", movement_api_set_ignore_obstacles },
      { "get_direction4", movement_api_get_direction4 },
      { NULL, NULL }
  };
  static const luaL_Reg movement_metamethods[] = {
      { "__gc", userdata_meta_gc },
      { NULL, NULL }
  };
  register_type(movement_module_name, movement_methods, movement_metamethods);

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
  register_functions(straight_movement_module_name, movement_methods);
  register_type(straight_movement_module_name, straight_movement_methods, movement_metamethods);
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

