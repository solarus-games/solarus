/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/CurrentQuest.h"
#include "solarus/core/Geometry.h"
#include "solarus/core/MainLoop.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/QuestDatabase.h"
#include "solarus/core/QuestProperties.h"
#include "solarus/core/Settings.h"
#include "solarus/core/System.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <lua.hpp>

namespace Solarus {

/**
 * Name of the Lua table representing the main module of Solarus.
 */
const std::string LuaContext::main_module_name = "sol.main";

/**
 * \brief Initializes the main features provided to Lua.
 */
void LuaContext::register_main_module() {

  std::vector<luaL_Reg> functions = {
      { "get_solarus_version", main_api_get_solarus_version },
      { "get_quest_format", main_api_get_quest_format },
      { "load_file", main_api_load_file },
      { "do_file", main_api_do_file },
      { "reset", main_api_reset },
      { "exit", main_api_exit },
      { "get_elapsed_time", main_api_get_elapsed_time },
      { "get_quest_write_dir", main_api_get_quest_write_dir },
      { "set_quest_write_dir", main_api_set_quest_write_dir },
      { "load_settings", main_api_load_settings },
      { "save_settings", main_api_save_settings },
      { "get_distance", main_api_get_distance },
      { "get_angle", main_api_get_angle },
      { "get_type", main_api_get_type },
      { "get_metatable", main_api_get_metatable },
      { "get_os", main_api_get_os }
  };

  if (CurrentQuest::is_format_at_least({ 1, 6 })) {
    functions.insert(functions.end(), {
        { "get_quest_version", main_api_get_quest_version },
        { "get_resource_ids", main_api_get_resource_ids }
    });
  }
  register_functions(main_module_name, functions);

  // Store sol.main in the registry to access it safely
  // from C++ (and also slightly faster).
  // After that, the engine will never rely on the existence of a global
  // value called "sol". The user can therefore do whatever he wants, including
  // renaming the sol global table to something else in the unlikely case where
  // another Lua library called "sol" is required, or if he simply does not
  // like the name "sol".

                                  // ...
  lua_getglobal(l, "sol");
                                  // ... sol
  lua_getfield(l, -1, "main");
                                  // ... sol main
  lua_setfield(l, LUA_REGISTRYINDEX, main_module_name.c_str());
                                  // ... sol
  lua_pop(l, 1);
                                  // ...
}

/**
 * \brief Pushes the sol.main table onto the stack.
 * \param l A Lua state.
 */
void LuaContext::push_main(lua_State* l) {

  lua_getfield(l, LUA_REGISTRYINDEX, main_module_name.c_str());
}

/**
 * \brief Implementation of sol.main.get_solarus_version().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_solarus_version(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& solarus_version = SOLARUS_VERSION;

    push_string(l, solarus_version);
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_quest_version().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_quest_version(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& quest_version = CurrentQuest::get_properties().get_quest_version();

    if (quest_version.empty()) {
      lua_pushnil(l);
    }
    else {
      push_string(l, quest_version);
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_quest_format().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_quest_format(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& quest_format = CurrentQuest::get_properties().get_solarus_version();

    push_string(l, quest_format);
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.load_file().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_load_file(lua_State *l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& file_name = LuaTools::check_string(l, 1);

    if (!load_file(l, file_name)) {
      lua_pushnil(l);
    }

    return 1;
  });
}

/**
 * \brief Implementation of sol.main.do_file().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_do_file(lua_State *l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& file_name = LuaTools::check_string(l, 1);

    do_file(l, file_name);

    return 0;
  });
}

/**
 * \brief Implementation of sol.main.reset().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_reset(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    get_lua_context(l).get_main_loop().set_resetting();

    return 0;
  });
}

/**
 * \brief Implementation of sol.main.exit().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_exit(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    get_lua_context(l).get_main_loop().set_exiting();

    return 0;
  });
}

/**
 * \brief Implementation of sol.main.get_elapsed_time().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_elapsed_time(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    uint32_t elapsed_time = System::now();

    lua_pushinteger(l, elapsed_time);
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_quest_write_dir().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_get_quest_write_dir(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& quest_write_dir = QuestFiles::get_quest_write_dir();

    if (quest_write_dir.empty()) {
      lua_pushnil(l);
    }
    else {
      push_string(l, quest_write_dir);
    }
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.set_quest_write_dir().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_set_quest_write_dir(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& quest_write_dir = LuaTools::opt_string(l, 1, "");

    QuestFiles::set_quest_write_dir(quest_write_dir);

    return 0;
  });
}

/**
 * \brief Implementation of sol.main.load_settings().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_load_settings(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    std::string file_name = LuaTools::opt_string(l, 1, "settings.dat");

    if (QuestFiles::get_quest_write_dir().empty()) {
      LuaTools::error(l, "Cannot load settings: no write directory was specified in quest.dat");
    }

    bool success = false;
    if (QuestFiles::data_file_exists(file_name)) {
      Settings settings;
      success = settings.load(file_name);
      if (success) {
        settings.apply_to_quest();
      }
    }

    lua_pushboolean(l, success);
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.save_settings().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_save_settings(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    std::string file_name = LuaTools::opt_string(l, 1, "settings.dat");

    if (QuestFiles::get_quest_write_dir().empty()) {
      LuaTools::error(l, "Cannot save settings: no write directory was specified in quest.dat");
    }

    Settings settings;
    settings.set_from_quest();
    bool success = settings.save(file_name);

    lua_pushboolean(l, success);
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_distance().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_get_distance(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int x1 = LuaTools::check_int(l, 1);
    int y1 = LuaTools::check_int(l, 2);
    int x2 = LuaTools::check_int(l, 3);
    int y2 = LuaTools::check_int(l, 4);

    int distance = (int) Geometry::get_distance(x1, y1, x2, y2);

    lua_pushinteger(l, distance);
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_angle().
 * \param l the Lua context that is calling this function
 * \return number of values to return to Lua
 */
int LuaContext::main_api_get_angle(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    int x1 = LuaTools::check_int(l, 1);
    int y1 = LuaTools::check_int(l, 2);
    int x2 = LuaTools::check_int(l, 3);
    int y2 = LuaTools::check_int(l, 4);

    double angle = Geometry::get_angle(x1, y1, x2, y2);

    lua_pushnumber(l, angle);
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_resource_ids().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_resource_ids(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    ResourceType resource_type = LuaTools::check_enum<ResourceType>(l, 1);
    const QuestDatabase::ResourceMap& elements = CurrentQuest::get_resources().get_elements(resource_type);

    // Build a Lua array containing the ids.
    lua_settop(l, 0);
    lua_newtable(l);
    int i = 1;
    for (const std::pair<std::string, std::string>& kvp : elements) {
      const std::string& id = kvp.first;
      push_string(l, id);
      lua_rawseti(l, 1, i);
      ++i;
    }

    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_type().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_type(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    luaL_checkany(l, 1);
    std::string module_name;
    if (!is_solarus_userdata(l, 1, module_name)) {
      // Return the same thing as the usual Lua type() function.
      std::string type_name = luaL_typename(l, 1);
      push_string(l, type_name);
    }
    else {
      // Remove the "sol." prefix.
      push_string(l, module_name.substr(4));
    }

    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_metatable().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_metatable(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& type_name = LuaTools::check_string(l, 1);

    luaL_getmetatable(l, (std::string("sol.") + type_name).c_str());
    return 1;
  });
}

/**
 * \brief Implementation of sol.main.get_os().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::main_api_get_os(lua_State* l) {

  const std::string& os = System::get_os();

  push_string(l, os);
  return 1;
}

/**
 * \brief Calls sol.main.on_started() if it exists.
 *
 * This function is called when the engine requests Lua to show an
 * initial screen, i.e. at the beginning of the program
 * or when the program is reset.
 */
void LuaContext::main_on_started() {

  push_main(l);
  on_started();
  lua_pop(l, 1);
}

/**
 * \brief Calls sol.main.on_finished() if it exists.
 *
 * This function is called when the program is reset or stopped.
 */
void LuaContext::main_on_finished() {

  push_main(l);
  on_finished();
  remove_timers(-1);  // Stop timers associated to sol.main.
  remove_menus(-1);  // Stop menus associated to sol.main.
  lua_pop(l, 1);
}

/**
 * \brief Calls sol.main.on_update() if it exists.
 *
 * This function is called at each cycle by the main loop.
 */
void LuaContext::main_on_update() {

  push_main(l);
  on_update();
  menus_on_update(-1);
  lua_pop(l, 1);
}

/**
 * \brief Calls sol.main.on_draw() if it exists.
 * \param dst_surface The destination surface.
 */
void LuaContext::main_on_draw(const SurfacePtr& dst_surface) {

  push_main(l);
  on_draw(dst_surface);
  menus_on_draw(-1, dst_surface);
  lua_pop(l, 1);
}

/**
 * \brief Notifies Lua that an input event has just occurred.
 *
 * The appropriate callback in sol.main is triggered if it exists.
 *
 * \param event The input event to handle.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::main_on_input(const InputEvent& event) {

  push_main(l);
  bool handled = on_input(event);
  if (!handled) {
    handled = menus_on_input(-1, event);
  }
  lua_pop(l, 1);
  return handled;
}

}

