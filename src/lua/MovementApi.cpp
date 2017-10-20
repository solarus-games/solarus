/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Entities.h"
#include "solarus/entities/Hero.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/ExportableToLua.h"
#include "solarus/lua/ExportableToLuaPtr.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/movements/CircleMovement.h"
#include "solarus/movements/JumpMovement.h"
#include "solarus/movements/PathFindingMovement.h"
#include "solarus/movements/PathMovement.h"
#include "solarus/movements/PixelMovement.h"
#include "solarus/movements/RandomMovement.h"
#include "solarus/movements/RandomPathMovement.h"
#include "solarus/movements/TargetMovement.h"
#include "solarus/Drawable.h"
#include "solarus/Game.h"
#include "solarus/MainLoop.h"
#include "solarus/Map.h"

namespace Solarus {

/**
 * Name of the Lua table representing the movement module.
 */
const std::string LuaContext::movement_module_name = "sol.movement";

/**
 * Name of the Lua table representing the straight movement module.
 */
const std::string LuaContext::movement_straight_module_name = "sol.straight_movement";

/**
 * Name of the Lua table representing the target movement module.
 */
const std::string LuaContext::movement_target_module_name = "sol.target_movement";

/**
 * Name of the Lua table representing the random movement module.
 */
const std::string LuaContext::movement_random_module_name = "sol.random_movement";

/**
 * Name of the Lua table representing the path movement module.
 */
const std::string LuaContext::movement_path_module_name = "sol.path_movement";

/**
 * Name of the Lua table representing the random path movement module.
 */
const std::string LuaContext::movement_random_path_module_name = "sol.random_path_movement";

/**
 * Name of the Lua table representing the path finding movement module.
 */
const std::string LuaContext::movement_path_finding_module_name = "sol.path_finding_movement";

/**
 * Name of the Lua table representing the circle movement module.
 */
const std::string LuaContext::movement_circle_module_name = "sol.circle_movement";

/**
 * Name of the Lua table representing the jump movement module.
 */
const std::string LuaContext::movement_jump_module_name = "sol.jump_movement";

/**
 * Name of the Lua table representing the pixel movement module.
 */
const std::string LuaContext::movement_pixel_module_name = "sol.pixel_movement";

/**
 * \brief Initializes the movement features provided to Lua.
 */
void LuaContext::register_movement_module() {

  // Functions of sol.movement.
  static const luaL_Reg movement_functions[] = {
      { "create", movement_api_create },
      { nullptr, nullptr }
  };

  // Methods common to all movement types.
#define MOVEMENT_COMMON_METHODS\
      { "get_xy", movement_api_get_xy },\
      { "set_xy", movement_api_set_xy },\
      { "start", movement_api_start },\
      { "stop", movement_api_stop },\
      { "get_ignore_obstacles", movement_api_get_ignore_obstacles },\
      { "set_ignore_obstacles", movement_api_set_ignore_obstacles },\
      { "get_direction4", movement_api_get_direction4 }

  // Metamethods of all movement types.
  static const luaL_Reg metamethods[] = {
      { "__gc", userdata_meta_gc },
      { "__newindex", userdata_meta_newindex_as_table },
      { "__index", userdata_meta_index_as_table },
      { nullptr, nullptr }
  };

  // Methods of the movement type.
  static const luaL_Reg movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { nullptr, nullptr }
  };

  register_type(
      movement_module_name,
      movement_functions,
      movement_methods,
      metamethods
  );

  // Straight movement.
  static const luaL_Reg straight_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "get_speed", straight_movement_api_get_speed },
      { "set_speed", straight_movement_api_set_speed },
      { "get_angle", straight_movement_api_get_angle },
      { "set_angle", straight_movement_api_set_angle },
      { "get_max_distance", straight_movement_api_get_max_distance },
      { "set_max_distance", straight_movement_api_set_max_distance },
      { "is_smooth", straight_movement_api_is_smooth },
      { "set_smooth", straight_movement_api_set_smooth },
      { nullptr, nullptr }
  };
  register_type(
      movement_straight_module_name,
      nullptr,
      straight_movement_methods,
      metamethods
  );

  // Random movement.
  static const luaL_Reg random_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "get_speed", random_movement_api_get_speed },
      { "set_speed", random_movement_api_set_speed },
      { "get_angle", random_movement_api_get_angle },
      { "get_max_distance", random_movement_api_get_max_distance },
      { "set_max_distance", random_movement_api_set_max_distance },
      { "is_smooth", random_movement_api_is_smooth },
      { "set_smooth", random_movement_api_set_smooth },
      { nullptr, nullptr }
  };
  register_type(
      movement_random_module_name,
      nullptr,
      random_movement_methods,
      metamethods
  );

  // Target movement.
  static const luaL_Reg target_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "set_target", target_movement_api_set_target },
      { "get_speed", target_movement_api_get_speed },
      { "set_speed", target_movement_api_set_speed },
      { "get_angle", target_movement_api_get_angle },
      { "is_smooth", target_movement_api_is_smooth },
      { "set_smooth", target_movement_api_set_smooth },
      { nullptr, nullptr }
  };
  register_type(
      movement_target_module_name,
      nullptr,
      target_movement_methods,
      metamethods
  );

  // Path movement.
  static const luaL_Reg path_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "get_path", path_movement_api_get_path },
      { "set_path", path_movement_api_set_path },
      { "get_speed", path_movement_api_get_speed },
      { "set_speed", path_movement_api_set_speed },
      { "get_loop", path_movement_api_get_loop },
      { "set_loop", path_movement_api_set_loop },
      { "get_snap_to_grid", path_movement_api_get_snap_to_grid },
      { "set_snap_to_grid", path_movement_api_set_snap_to_grid },
      { nullptr, nullptr }
  };
  register_type(
      movement_path_module_name,
      nullptr,
      path_movement_methods,
      metamethods
  );

  // Random path movement.
  static const luaL_Reg random_path_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "get_speed", random_path_movement_api_get_speed },
      { "set_speed", random_path_movement_api_set_speed },
      { nullptr, nullptr }
  };
  register_type(
      movement_random_path_module_name,
      nullptr,
      random_path_movement_methods,
      metamethods
  );

  // Path finding movement.
  static const luaL_Reg path_finding_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "set_target", path_finding_movement_api_set_target },
      { "get_speed", path_finding_movement_api_get_speed },
      { "set_speed", path_finding_movement_api_set_speed },
      { nullptr, nullptr }
  };
  register_type(
      movement_path_finding_module_name,
      nullptr,
      path_finding_movement_methods,
      metamethods
  );

  // Circle movement.
  static const luaL_Reg circle_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
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
      { nullptr, nullptr }
  };
  register_type(
      movement_circle_module_name,
      nullptr,
      circle_movement_methods,
      metamethods
  );

  // Jump movement.
  static const luaL_Reg jump_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "get_direction8", jump_movement_api_get_direction8 },
      { "set_direction8", jump_movement_api_set_direction8 },
      { "get_distance", jump_movement_api_get_distance },
      { "set_distance", jump_movement_api_set_distance },
      { "get_speed", jump_movement_api_get_speed },
      { "set_speed", jump_movement_api_set_speed },
      { nullptr, nullptr }
  };
  register_type(
      movement_jump_module_name,
      nullptr,
      jump_movement_methods,
      metamethods
  );

  // Pixel movement.
  static const luaL_Reg pixel_movement_methods[] = {
      MOVEMENT_COMMON_METHODS,
      { "get_trajectory", pixel_movement_api_get_trajectory },
      { "set_trajectory", pixel_movement_api_set_trajectory },
      { "get_loop", pixel_movement_api_get_loop },
      { "set_loop", pixel_movement_api_set_loop },
      { "get_delay", pixel_movement_api_get_delay },
      { "set_delay", pixel_movement_api_set_delay },
      { nullptr, nullptr }
  };
  register_type(
      movement_pixel_module_name,
      nullptr,
      pixel_movement_methods,
      metamethods
  );

  // Create the table that will store the movements applied to x,y points.
                                  // ...
  lua_newtable(l);
                                  // ... movements
  lua_newtable(l);
                                  // ... movements meta
  lua_pushstring(l, "v");
                                  // ... movements meta "v"
  lua_setfield(l, -2, "__mode");
                                  // ... movements meta
  lua_setmetatable(l, -2);
                                  // ... movements
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                  // ...
}

/**
 * \brief Returns whether a value is a userdata of type movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a entity.
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
 * \brief Checks that the userdata at the specified index of the stack is a
 * movement (of any subtype) and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the sprite
 */
std::shared_ptr<Movement> LuaContext::check_movement(lua_State* l, int index) {

  if (is_movement(l, index)) {
    const ExportableToLuaPtr& userdata = *(static_cast<ExportableToLuaPtr*>(
      lua_touserdata(l, index)
    ));
    return std::static_pointer_cast<Movement>(userdata);
  }
  else {
    LuaTools::type_error(l, index, "movement");
    throw;
  }
}

/**
 * \brief Pushes a movement userdata onto the stack.
 * \param l a Lua context
 * \param movement a movement
 */
void LuaContext::push_movement(lua_State* l, Movement& movement) {

  push_userdata(l, movement);
}

/**
 * \brief Starts moving an x,y point.
 *
 * The point is a Lua table with two fields x and y.
 * Fields x and y may be initially missing.
 *
 * \param movement The movement to apply to the points.
 * \param point_index Index of the x,y table in the Lua stack.
 */
void LuaContext::start_movement_on_point(
    const std::shared_ptr<Movement>& movement, int point_index
) {
  int x = 0;
  int y = 0;
                                  // ...
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                  // ... movements
  push_movement(l, *movement);
                                  // ... movements movement
  lua_pushvalue(l, point_index);
                                  // ... movements movement xy
  lua_getfield(l, -1, "x");
                                  // ... movements movement xy x/nil
  if (lua_isnil(l, -1)) {
                                  // ... movements movement xy nil
    lua_pop(l, 1);
                                  // ... movements movement xy
    lua_pushinteger(l, 0);
                                  // ... movements movement xy 0
    lua_setfield(l, -2, "x");
                                  // ... movements movement xy
  }
  else {
                                  // ... movements movement xy x
    x = LuaTools::check_int(l, -1);
    lua_pop(l, 1);
                                  // ... movements movement xy
  }
  lua_getfield(l, -1, "y");
                                  // ... movements movement xy y/nil
  if (lua_isnil(l, -1)) {
                                  // ... movements movement xy nil
    lua_pop(l, 1);
                                  // ... movements movement xy
    lua_pushinteger(l, 0);
                                  // ... movements movement xy 0
    lua_setfield(l, -2, "y");
                                  // ... movements movement xy
    movement->set_y(0);
  }
  else {
                                  // ... movements movement xy y
    y = LuaTools::check_int(l, -1);
    lua_pop(l, 1);
                                  // ... movements movement xy
  }

  lua_settable(l, -3);
                                  // ... movements
  lua_pop(l, 1);
                                  // ...
  movement->set_xy(x, y);
}

/**
 * \brief Stops moving an x,y point.
 * \param movement The movement to stop.
 */
void LuaContext::stop_movement_on_point(const std::shared_ptr<Movement>& movement) {

                                  // ...
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                  // ... movements
  push_movement(l, *movement);
                                  // ... movements movement
  lua_pushnil(l);
                                  // ... movements movement nil
  lua_settable(l, -3);
                                  // ... movements
  lua_pop(l, 1);
                                  // ...
}

/**
 * \brief Updates all movements applied to x,y points.
 *
 * Movements applied to map entities or drawables are already updated
 * by the entity or the drawable.
 * This may change in the future in order to unify the handling of movements.
 */
void LuaContext::update_movements() {

  lua_getfield(l, LUA_REGISTRYINDEX, "sol.movements_on_points");
  lua_pushnil(l);  // First key.
  while (lua_next(l, -2)) {
    Movement& movement = *check_movement(l, -2);
    movement.update();
    lua_pop(l, 1);  // Pop the value, keep the key for next iteration.
  }
  lua_pop(l, 1);  // Pop the movements table.
}

/**
 * \brief Implementation of sol.movement.create().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_create(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaContext& lua_context = get_lua_context(l);
    const std::string& type = LuaTools::check_string(l, 1);

    std::shared_ptr<Movement> movement;
    if (type == "straight") {
      std::shared_ptr<StraightMovement> straight_movement =
          std::make_shared<StraightMovement>(false, true);
      straight_movement->set_speed(32);
      movement = straight_movement;
    }
    else if (type == "random") {
      movement = std::make_shared<RandomMovement>(32);
    }
    else if (type == "target") {
      Game* game = lua_context.get_main_loop().get_game();
      if (game != nullptr) {
        // If we are on a map, the default target is the hero.
        movement = std::make_shared<TargetMovement>(
            game->get_hero(), 0, 0, 96, false
        );
      }
      else {
        movement = std::make_shared<TargetMovement>(
            nullptr, 0, 0, 32, false
        );
      }
    }
    else if (type == "path") {
      movement = std::make_shared<PathMovement>(
          "", 32, false, false, false
      );
    }
    else if (type == "random_path") {
      movement = std::make_shared<RandomPathMovement>(32);
    }
    else if (type == "path_finding") {
      std::shared_ptr<PathFindingMovement> path_finding_movement =
          std::make_shared<PathFindingMovement>(32);
      Game* game = lua_context.get_main_loop().get_game();
      if (game != nullptr) {
        // If we are on a map, the default target is the hero.
        path_finding_movement->set_target(game->get_hero());
      }
      movement = path_finding_movement;
    }
    else if (type == "circle") {
      movement = std::make_shared<CircleMovement>(false);
    }
    else if (type == "jump") {
      movement = std::make_shared<JumpMovement>(0, 0, 0, false);
    }
    else if (type == "pixel") {
      movement = std::make_shared<PixelMovement>("", 30, false, false);
    }
    else {
      LuaTools::arg_error(l, 1, "should be one of: "
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
  });
}

/**
 * \brief Implementation of movement:get_xy().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_get_xy(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Movement& movement = *check_movement(l, 1);

    const Point& xy = movement.get_xy();
    lua_pushinteger(l, xy.x);
    lua_pushinteger(l, xy.y);
    return 2;
  });
}

/**
 * \brief Implementation of movement:set_xy().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_set_xy(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    Movement& movement = *check_movement(l, 1);
    int x = LuaTools::check_int(l, 2);
    int y = LuaTools::check_int(l, 3);

    movement.set_xy(x, y);

    return 0;
  });
}

/**
 * \brief Implementation of movement:start().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_start(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaContext& lua_context = get_lua_context(l);

    std::shared_ptr<Movement> movement = check_movement(l, 1);
    movement_api_stop(l);  // First, stop any previous movement.

    ScopedLuaRef callback_ref = LuaTools::opt_function(l, 3);

    if (lua_type(l, 2) == LUA_TTABLE) {
      lua_context.start_movement_on_point(movement, 2);
    }
    else if (is_entity(l, 2)) {
      Entity& entity = *check_entity(l, 2);
      entity.clear_movement();
      entity.set_movement(movement);
    }
    else if (is_drawable(l, 2)) {
      Drawable& drawable = *check_drawable(l, 2);
      drawable.start_movement(movement);
    }
    else {
      LuaTools::type_error(l, 2, "table, entity or drawable");
    }
    movement->set_finished_callback(callback_ref);

    return 0;
  });
}

/**
 * \brief Implementation of movement:stop().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_stop(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    LuaContext& lua_context = get_lua_context(l);

    std::shared_ptr<Movement> movement = check_movement(l, 1);

    Entity* entity = movement->get_entity();
    if (entity != nullptr) {
      // The object controlled is a map entity.
      entity->clear_movement();
    }
    else {
      Drawable* drawable = movement->get_drawable();
      if (drawable != nullptr) {
        // The object controlled is a drawable.
        drawable->stop_movement();
      }
      else {
        // The object controlled is a point.
        lua_context.stop_movement_on_point(movement);
      }
    }

    return 0;
  });
}

/**
 * \brief Implementation of movement:get_ignore_obstacles().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_get_ignore_obstacles(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    std::shared_ptr<Movement> movement = check_movement(l, 1);

    lua_pushboolean(l, movement->are_obstacles_ignored());
    return 1;
  });
}

/**
 * \brief Implementation of movement:set_ignore_obstacles().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_set_ignore_obstacles(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    std::shared_ptr<Movement> movement = check_movement(l, 1);
    bool ignore_obstacles = LuaTools::opt_boolean(l, 2, true);

    movement->set_ignore_obstacles(ignore_obstacles);

    return 0;
  });
}

/**
 * \brief Implementation of movement:get_direction4().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::movement_api_get_direction4(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    std::shared_ptr<Movement> movement = check_movement(l, 1);
    lua_pushinteger(l, movement->get_displayed_direction4());
    return 1;
  });
}

/**
 * \brief Returns whether a value is a userdata of type straight movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a straight movement.
 */
bool LuaContext::is_straight_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_straight_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * straight movement and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the movement
 */
std::shared_ptr<StraightMovement> LuaContext::check_straight_movement(lua_State* l, int index) {

  return std::static_pointer_cast<StraightMovement>(check_userdata(
      l, index, movement_straight_module_name
  ));
}

/**
 * \brief Implementation of straight_movement:get_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_get_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const StraightMovement& movement = *check_straight_movement(l, 1);
    lua_pushinteger(l, movement.get_speed());
    return 1;
  });
}

/**
 * \brief Implementation of straight_movement:set_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_speed(lua_State* l) {

  StraightMovement& movement = *check_straight_movement(l, 1);
  int speed = LuaTools::check_int(l, 2);
  movement.set_speed(speed);
  return 0;
}

/**
 * \brief Implementation of straight_movement:get_angle().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_get_angle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const StraightMovement& movement = *check_straight_movement(l, 1);
    lua_pushnumber(l, movement.get_angle());
    return 1;
  });
}

/**
 * \brief Implementation of straight_movement:set_angle().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_angle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    StraightMovement& movement = *check_straight_movement(l, 1);
    double angle = LuaTools::check_number(l, 2);
    movement.set_angle(angle);
    return 0;
  });
}

/**
 * \brief Implementation of straight_movement:get_max_distance().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_get_max_distance(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const StraightMovement& movement = *check_straight_movement(l, 1);
    lua_pushinteger(l, movement.get_max_distance());
    return 1;
  });
}

/**
 * \brief Implementation of straight_movement:set_max_distance().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_max_distance(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    StraightMovement& movement = *check_straight_movement(l, 1);
    int max_distance = LuaTools::check_int(l, 2);
    movement.set_max_distance(max_distance);
    return 0;
  });
}

/**
 * \brief Implementation of straight_movement:is_smooth().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_is_smooth(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const StraightMovement& movement = *check_straight_movement(l, 1);
    lua_pushboolean(l, movement.is_smooth());
    return 1;
  });
}

/**
 * \brief Implementation of straight_movement:set_smooth().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::straight_movement_api_set_smooth(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    StraightMovement& movement = *check_straight_movement(l, 1);
    bool smooth = LuaTools::opt_boolean(l, 2, true);
    movement.set_smooth(smooth);

    return 0;
  });
}

/**
 * \brief Returns whether a value is a userdata of type random movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a random movement.
 */
bool LuaContext::is_random_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_random_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * random movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<RandomMovement> LuaContext::check_random_movement(lua_State* l, int index) {
  return std::static_pointer_cast<RandomMovement>(check_userdata(
      l, index, movement_random_module_name
  ));
}

/**
 * \brief Implementation of random_movement:get_speed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::random_movement_api_get_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const RandomMovement& movement = *check_random_movement(l, 1);
    lua_pushinteger(l, movement.get_speed());
    return 1;
  });
}

/**
 * \brief Implementation of random_movement:set_speed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::random_movement_api_set_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    RandomMovement& movement = *check_random_movement(l, 1);
    int speed = LuaTools::check_int(l, 2);
    movement.set_normal_speed(speed);
    return 0;
  });
}

/**
 * \brief Implementation of random_movement:get_angle().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::random_movement_api_get_angle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const RandomMovement& movement = *check_random_movement(l, 1);
    lua_pushnumber(l, movement.get_angle());
    return 1;
  });
}

/**
 * \brief Implementation of random_movement:get_max_distance().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::random_movement_api_get_max_distance(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const RandomMovement& movement = *check_random_movement(l, 1);
    lua_pushinteger(l, movement.get_max_radius());
    return 1;
  });
}

/**
 * \brief Implementation of random_movement:set_max_distance().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::random_movement_api_set_max_distance(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    RandomMovement& movement = *check_random_movement(l, 1);
    int max_radius = LuaTools::check_int(l, 2);
    movement.set_max_radius(max_radius);
    return 0;
  });
}

/**
 * \brief Implementation of random_movement:is_smooth().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::random_movement_api_is_smooth(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const RandomMovement& movement = *check_random_movement(l, 1);
    lua_pushboolean(l, movement.is_smooth());
    return 1;
  });
}

/**
 * \brief Implementation of random_movement:set_smooth().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::random_movement_api_set_smooth(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    RandomMovement& movement = *check_random_movement(l, 1);
    bool smooth = LuaTools::opt_boolean(l, 2, true);
    movement.set_smooth(smooth);

    return 0;
  });
}

/**
 * \brief Returns whether a value is a userdata of type target movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a target movement.
 */
bool LuaContext::is_target_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_target_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * target movement and returns it.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return The movement.
 */
std::shared_ptr<TargetMovement> LuaContext::check_target_movement(lua_State* l, int index) {
  return std::static_pointer_cast<TargetMovement>(check_userdata(
      l, index, movement_target_module_name
  ));
}

/**
 * \brief Implementation of target_movement:set_target().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::target_movement_api_set_target(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    TargetMovement& movement = *check_target_movement(l, 1);
    if (lua_isnumber(l, 2)) {
      // The target is a fixed point.
      int x = LuaTools::check_int(l, 2);
      int y = LuaTools::check_int(l, 3);
      movement.set_target(nullptr, Point(x, y));
    }
    else {
      // the target is an entity, possibly with an offset.
      EntityPtr target = check_entity(l, 2);
      int x = 0;
      int y = 0;
      if (lua_isnumber(l, 3)) {
        // There is an offset.
        x = LuaTools::check_int(l, 3);
        y = LuaTools::check_int(l, 4);
      }
      movement.set_target(target, Point(x, y));
    }

    return 0;
  });
}

/**
 * \brief Implementation of target_movement:get_speed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::target_movement_api_get_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TargetMovement& movement = *check_target_movement(l, 1);
    lua_pushinteger(l, movement.get_speed());
    return 1;
  });
}

/**
 * \brief Implementation of target_movement:set_speed().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::target_movement_api_set_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    TargetMovement& movement = *check_target_movement(l, 1);
    int speed = LuaTools::check_int(l, 2);
    movement.set_moving_speed(speed);
    return 0;
  });
}

/**
 * \brief Implementation of target_movement:get_angle().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::target_movement_api_get_angle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TargetMovement& movement = *check_target_movement(l, 1);
    lua_pushnumber(l, movement.get_angle());
    return 1;
  });
}

/**
 * \brief Implementation of target_movement:is_smooth().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::target_movement_api_is_smooth(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const TargetMovement& movement = *check_target_movement(l, 1);
    lua_pushboolean(l, movement.is_smooth());
    return 1;
  });
}

/**
 * \brief Implementation of target_movement:set_smooth().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::target_movement_api_set_smooth(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    TargetMovement& movement = *check_target_movement(l, 1);
    bool smooth = LuaTools::opt_boolean(l, 2, true);
    movement.set_smooth(smooth);

    return 0;
  });
}

/**
 * \brief Returns whether a value is a userdata of type path movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a path movement.
 */
bool LuaContext::is_path_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_path_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * path movement and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the movement
 */
std::shared_ptr<PathMovement> LuaContext::check_path_movement(lua_State* l, int index) {
  return std::static_pointer_cast<PathMovement>(check_userdata(
      l, index, movement_path_module_name
  ));
}

/**
 * \brief Implementation of path_movement:get_path().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_path(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PathMovement& movement = *check_path_movement(l, 1);

    const std::string& path = movement.get_path();
    // build a Lua array containing the path
    lua_settop(l, 1);
    lua_newtable(l);
    for (size_t i = 0; i < path.size(); i++) {
      int direction8 = path[i] - '0';
      lua_pushinteger(l, direction8);
      lua_rawseti(l, 2, int(i));
    }

    return 1;
  });
}

/**
 * \brief Implementation of path_movement:set_path().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_path(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PathMovement& movement = *check_path_movement(l, 1);
    LuaTools::check_type(l, 2, LUA_TTABLE);

    // build the path as a string from the Lua table
    std::string path = "";
    lua_pushnil(l); // first key
    while (lua_next(l, 2) != 0) {
      int direction8 = LuaTools::check_int(l, 4);
      path += ('0' + direction8);
      lua_pop(l, 1); // pop the value, let the key for the iteration
    }
    movement.set_path(path);

    return 0;
  });
}

/**
 * \brief Implementation of path_movement:get_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PathMovement& movement = *check_path_movement(l, 1);
    lua_pushinteger(l, movement.get_speed());
    return 1;
  });
}

/**
 * \brief Implementation of path_movement:set_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PathMovement& movement = *check_path_movement(l, 1);
    int speed = LuaTools::check_int(l, 2);
    movement.set_speed(speed);
    return 0;
  });
}

/**
 * \brief Implementation of path_movement:get_loop().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_loop(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PathMovement& movement = *check_path_movement(l, 1);
    lua_pushboolean(l, movement.get_loop());
    return 1;
  });
}

/**
 * \brief Implementation of path_movement:set_loop().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_loop(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PathMovement& movement = *check_path_movement(l, 1);
    bool loop = LuaTools::opt_boolean(l, 2, true);

    movement.set_loop(loop);

    return 0;
  });
}

/**
 * \brief Implementation of path_movement:get_snap_to_grid().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_get_snap_to_grid(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PathMovement& movement = *check_path_movement(l, 1);
    lua_pushboolean(l, movement.get_snap_to_grid());
    return 1;
  });
}

/**
 * \brief Implementation of path_movement:set_snap_to_grid().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_movement_api_set_snap_to_grid(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PathMovement& movement = *check_path_movement(l, 1);
    bool snap_to_grid = LuaTools::opt_boolean(l, 2, true);

    movement.set_snap_to_grid(snap_to_grid);

    return 0;
  });
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * random path movement and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the movement
 */
std::shared_ptr<RandomPathMovement> LuaContext::check_random_path_movement(lua_State* l, int index) {
  return std::static_pointer_cast<RandomPathMovement>(check_userdata(
      l, index, movement_random_path_module_name
  ));
}

/**
 * \brief Returns whether a value is a userdata of type random path movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a random path movement.
 */
bool LuaContext::is_random_path_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_random_path_module_name);
}

/**
 * \brief Implementation of random_path_movement:get_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::random_path_movement_api_get_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const RandomPathMovement& movement = *check_random_path_movement(l, 1);
    lua_pushinteger(l, movement.get_speed());
    return 1;
  });
}

/**
 * \brief Implementation of random_path_movement:set_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::random_path_movement_api_set_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    RandomPathMovement& movement = *check_random_path_movement(l, 1);
    int speed = LuaTools::check_int(l, 2);
    movement.set_speed(speed);
    return 0;
  });
}

/**
 * \brief Returns whether a value is a userdata of type path finding movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a path finding  movement.
 */
bool LuaContext::is_path_finding_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_path_finding_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * path finding movement and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the movement
 */
std::shared_ptr<PathFindingMovement> LuaContext::check_path_finding_movement(lua_State* l, int index) {
  return std::static_pointer_cast<PathFindingMovement>(check_userdata(
      l, index, movement_path_finding_module_name
  ));
}

/**
 * \brief Implementation of path_finding_movement:set_target().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_finding_movement_api_set_target(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PathFindingMovement& movement = *check_path_finding_movement(l, 1);
    EntityPtr target = check_entity(l, 2);

    movement.set_target(target);

    return 0;
  });
}

/**
 * \brief Implementation of path_finding_movement:get_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_finding_movement_api_get_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PathFindingMovement& movement = *check_path_finding_movement(l, 1);
    lua_pushinteger(l, movement.get_speed());
    return 1;
  });
}

/**
 * \brief Implementation of path_finding_movement:set_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::path_finding_movement_api_set_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PathFindingMovement& movement = *check_path_finding_movement(l, 1);
    int speed = LuaTools::check_int(l, 2);
    movement.set_speed(speed);
    return 0;
  });
}

/**
 * \brief Returns whether a value is a userdata of type circle movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a circle movement.
 */
bool LuaContext::is_circle_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_circle_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * circle movement and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the movement
 */
std::shared_ptr<CircleMovement> LuaContext::check_circle_movement(lua_State* l, int index) {
  return std::static_pointer_cast<CircleMovement>(check_userdata(
      l, index, movement_circle_module_name
  ));
}

/**
 * \brief Implementation of circle_movement:set_center().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_center(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    if (lua_isnumber(l, 2)) {
      // the center is a fixed point
      int x = LuaTools::check_int(l, 2);
      int y = LuaTools::check_int(l, 3);
      movement.set_center(Point(x, y));
    }
    else {
      // the center is an entity

      EntityPtr center = check_entity(l, 2);
      int dx = LuaTools::opt_int(l, 3, 0);
      int dy = LuaTools::opt_int(l, 4, 0);
      movement.set_center(center, dx, dy);
    }

    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:get_radius().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_radius(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushinteger(l, movement.get_radius());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_radius().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_radius(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    int radius = LuaTools::check_int(l, 2);
    movement.set_radius(radius);
    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:get_radius_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_radius_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushinteger(l, movement.get_radius_speed());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_radius_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_radius_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    int radius_speed = LuaTools::check_int(l, 2);
    movement.set_radius_speed(radius_speed);
    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:is_clockwise().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_is_clockwise(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushboolean(l, movement.is_clockwise());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_clockwise().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_clockwise(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    bool clockwise = LuaTools::opt_boolean(l, 2, true);

    movement.set_clockwise(clockwise);

    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:get_initial_angle().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_initial_angle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushinteger(l, movement.get_initial_angle());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_initial_angle().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_initial_angle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    int initial_angle = LuaTools::check_int(l, 2);
    movement.set_initial_angle(initial_angle);
    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:get_angle_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_angle_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushinteger(l, movement.get_angle_speed());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_angle_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_angle_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    int angle_speed = LuaTools::check_int(l, 2);
    movement.set_angle_speed(angle_speed);
    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:get_max_rotations().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_max_rotations(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushinteger(l, movement.get_max_rotations());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_max_rotations().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_max_rotations(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    int max_rotations = LuaTools::check_int(l, 2);
    movement.set_max_rotations(max_rotations);
    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:get_duration().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_duration(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushinteger(l, movement.get_duration());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_duration().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_duration(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    int duration = LuaTools::check_int(l, 2);
    movement.set_duration(duration);
    return 0;
  });
}

/**
 * \brief Implementation of circle_movement:get_loop_delay().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_get_loop_delay(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const CircleMovement& movement = *check_circle_movement(l, 1);
    lua_pushinteger(l, movement.get_loop());
    return 1;
  });
}

/**
 * \brief Implementation of circle_movement:set_loop_delay().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::circle_movement_api_set_loop_delay(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    CircleMovement& movement = *check_circle_movement(l, 1);
    int loop_delay = LuaTools::check_int(l, 2);
    movement.set_loop(loop_delay);
    return 0;
  });
}

/**
 * \brief Returns whether a value is a userdata of type jump movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a jump movement.
 */
bool LuaContext::is_jump_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_jump_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * jump movement and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the movement
 */
std::shared_ptr<JumpMovement> LuaContext::check_jump_movement(lua_State* l, int index) {
  return std::static_pointer_cast<JumpMovement>(check_userdata(
      l, index, movement_jump_module_name
  ));
}

/**
 * \brief Implementation of jump_movement:get_direction8().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::jump_movement_api_get_direction8(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const JumpMovement& movement = *check_jump_movement(l, 1);
    lua_pushinteger(l, movement.get_direction8());
    return 1;
  });
}

/**
 * \brief Implementation of jump_movement:set_direction8().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::jump_movement_api_set_direction8(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    JumpMovement& movement = *check_jump_movement(l, 1);
    int direction8 = LuaTools::check_int(l, 2);
    movement.set_direction8(direction8);
    return 0;
  });
}

/**
 * \brief Implementation of jump_movement:get_distance().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::jump_movement_api_get_distance(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const JumpMovement& movement = *check_jump_movement(l, 1);
    lua_pushinteger(l, movement.get_distance());
    return 1;
  });
}

/**
 * \brief Implementation of jump_movement:set_distance().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::jump_movement_api_set_distance(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    JumpMovement& movement = *check_jump_movement(l, 1);
    int distance = LuaTools::check_int(l, 2);
    movement.set_distance(distance);
    return 0;
  });
}

/**
 * \brief Implementation of jump_movement:get_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::jump_movement_api_get_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const JumpMovement& movement = *check_jump_movement(l, 1);
    lua_pushinteger(l, movement.get_speed());
    return 1;
  });
}

/**
 * \brief Implementation of jump_movement:set_speed().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::jump_movement_api_set_speed(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    JumpMovement& movement = *check_jump_movement(l, 1);
    int speed = LuaTools::check_int(l, 2);
    movement.set_speed(speed);
    return 0;
  });
}

/**
 * \brief Returns whether a value is a userdata of type pixel movement.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return true if the value at this index is a pixel movement.
 */
bool LuaContext::is_pixel_movement(lua_State* l, int index) {
  return is_userdata(l, index, movement_pixel_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * pixel movement and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return the movement
 */
std::shared_ptr<PixelMovement> LuaContext::check_pixel_movement(lua_State* l, int index) {
  return std::static_pointer_cast<PixelMovement>(check_userdata(
      l, index, movement_pixel_module_name
  ));
}

/**
 * \brief Implementation of pixel_movement:get_trajectory().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_get_trajectory(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PixelMovement& movement = *check_pixel_movement(l, 1);

    const std::list<Point>& trajectory = movement.get_trajectory();
    // build a Lua array containing the trajectory
    lua_settop(l, 1);
    lua_newtable(l);
    int i = 0;
    for (const Point& xy: trajectory) {
      lua_newtable(l);
      lua_pushinteger(l, xy.x);
      lua_rawseti(l, 3, 1);
      lua_pushinteger(l, xy.y);
      lua_rawseti(l, 3, 2);
      lua_rawseti(l, 2, i);
      ++i;
    }

    return 1;
  });
}

/**
 * \brief Implementation of pixel_movement:set_trajectory().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_set_trajectory(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PixelMovement& movement = *check_pixel_movement(l, 1);
    LuaTools::check_type(l, 2, LUA_TTABLE);

    // build the trajectory as a string from the Lua table
    std::list<Point> trajectory;
    lua_pushnil(l); // first key
    while (lua_next(l, 2) != 0) {
      LuaTools::check_type(l, 4, LUA_TTABLE);
      lua_rawgeti(l, 4, 1);
      lua_rawgeti(l, 4, 2);
      int x = LuaTools::check_int(l, 5);
      int y = LuaTools::check_int(l, 6);
      trajectory.emplace_back(x, y);
      lua_settop(l, 3); // let the key for the iteration
    }
    movement.set_trajectory(trajectory);

    return 0;
  });
}

/**
 * \brief Implementation of pixel_movement:get_loop().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_get_loop(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PixelMovement& movement = *check_pixel_movement(l, 1);
    lua_pushboolean(l, movement.get_loop());
    return 1;
  });
}

/**
 * \brief Implementation of pixel_movement:set_loop().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_set_loop(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PixelMovement& movement = *check_pixel_movement(l, 1);
    bool loop = LuaTools::opt_boolean(l, 2, true);

    movement.set_loop(loop);

    return 0;
  });
}

/**
 * \brief Implementation of pixel_movement:get_delay().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_get_delay(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const PixelMovement& movement = *check_pixel_movement(l, 1);
    lua_pushinteger(l, movement.get_delay());
    return 1;
  });
}

/**
 * \brief Implementation of pixel_movement:set_delay().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::pixel_movement_api_set_delay(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    PixelMovement& movement = *check_pixel_movement(l, 1);
    uint32_t delay = uint32_t(LuaTools::check_int(l, 2));
    movement.set_delay(delay);
    return 0;
  });
}

/**
 * \brief Calls the on_position_changed() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 * \param xy The new coordinates.
 */
void LuaContext::movement_on_position_changed(
    Movement& movement, const Point& xy) {

                                  // ...
  push_movement(l, movement);
                                  // ... movement
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.movements_on_points");
                                  // ... movement movements
  lua_pushvalue(l, -2);
                                  // ... movement movements movement
  lua_gettable(l, -2);
                                  // ... movement movements xy/nil
  if (!lua_isnil(l, -1)) {
                                  // ... movement movements xy
    lua_pushinteger(l, xy.x);
                                  // ... movement movements xy x
    lua_setfield(l, -2, "x");
                                  // ... movement movements xy
    lua_pushinteger(l, xy.y);
                                  // ... movement movements xy y
    lua_setfield(l, -2, "y");
                                  // ... movement movements xy
  }
  lua_pop(l, 2);
                                  // ... movement
  if (userdata_has_field(movement, "on_position_changed")) {
    on_position_changed(xy);
  }
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_obstacle_reached() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 */
void LuaContext::movement_on_obstacle_reached(Movement& movement) {

  if (!userdata_has_field(movement, "on_obstacle_reached")) {
    return;
  }

  push_movement(l, movement);
  on_obstacle_reached();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_changed() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 */
void LuaContext::movement_on_changed(Movement& movement) {

  if (!userdata_has_field(movement, "on_changed")) {
    return;
  }

  push_movement(l, movement);
  on_changed();
  lua_pop(l, 1);
}

/**
 * \brief Calls the on_finished() method of a Lua movement.
 *
 * Does nothing if the method is not defined.
 *
 * \param movement A movement.
 */
void LuaContext::movement_on_finished(Movement& movement) {

  if (!userdata_has_field(movement, "on_finished")) {
    return;
  }

  push_movement(l, movement);
  on_finished();
  lua_pop(l, 1);
}

}

