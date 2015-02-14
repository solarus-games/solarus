/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Block.h"
#include "solarus/entities/Chest.h"
#include "solarus/entities/CustomEntity.h"
#include "solarus/entities/Destination.h"
#include "solarus/entities/Door.h"
#include "solarus/entities/Enemy.h"
#include "solarus/entities/GroundInfo.h"
#include "solarus/entities/Npc.h"
#include "solarus/entities/Pickable.h"
#include "solarus/entities/Sensor.h"
#include "solarus/entities/ShopTreasure.h"
#include "solarus/entities/Switch.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lua/ExportableToLuaPtr.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/Equipment.h"
#include "solarus/EquipmentItem.h"
#include "solarus/Map.h"
#include "solarus/Timer.h"
#include "solarus/Treasure.h"
#include <sstream>
#include <iostream>

namespace Solarus {

std::map<lua_State*, LuaContext*> LuaContext::lua_contexts;

/**
 * \brief Creates a Lua context.
 * \param main_loop The Solarus main loop manager.
 */
LuaContext::LuaContext(MainLoop& main_loop):
  l(nullptr),
  main_loop(main_loop) {

}

/**
 * \brief Destroys this Lua context.
 */
LuaContext::~LuaContext() {

  this->exit();
}

/**
 * \brief Returns the LuaContext object that encapsulates a Lua state.
 * \param l A Lua state.
 * \return The LuaContext object encapsulating this Lua state.
 */
LuaContext& LuaContext::get_lua_context(lua_State* l) {

  auto it = lua_contexts.find(l);

  Debug::check_assertion(it != lua_contexts.end(),
      "This Lua state does not belong to a LuaContext object");

  return *it->second;
}

/**
 * \brief Returns the internal Lua state encapsulated by this LuaContext object.
 * \return The internal Lua state.
 */
lua_State* LuaContext::get_internal_state() {
  return l;
}

/**
 * \brief Returns the Solarus main loop object.
 * \return The main loop manager.
 */
MainLoop& LuaContext::get_main_loop() {
  return main_loop;
}

/**
 * \brief Initializes Lua.
 */
void LuaContext::initialize() {

  // Create an execution context.
  l = luaL_newstate();
  lua_atpanic(l, l_panic);
  luaL_openlibs(l);

  // Associate this LuaContext object to the lua_State pointer.
  lua_contexts[l] = this;

  // Create a table that will keep track of all userdata.
                                  // --
  lua_newtable(l);
                                  // all_udata
  lua_newtable(l);
                                  // all_udata meta
  lua_pushstring(l, "v");
                                  // all_udata meta "v"
  lua_setfield(l, -2, "__mode");
                                  // all_udata meta
  lua_setmetatable(l, -2);
                                  // all_udata
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.all_userdata");
                                  // --

  // Allow userdata to be indexable if they want.
  lua_newtable(l);
                                  // udata_tables
  lua_setfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                  // --

  // Create the sol table that will contain the whole Solarus API.
  lua_newtable(l);
  lua_setglobal(l, "sol");

  // Register the C++ functions and types accessible by Lua.
  register_modules();

  // Make require() able to load Lua files even from the
  // data.solarus or data.solarus.zip archive.
                                  // --
  lua_getglobal(l, "sol");
                                  // -- sol
  lua_pushcfunction(l, l_loader);
                                  // -- sol loader
  lua_setfield(l, -2, "loader");
                                  // -- sol
  luaL_dostring(l, "table.insert(package.loaders, 2, sol.loader)");
                                  // -- sol
  lua_pushnil(l);
                                  // -- sol nil
  lua_setfield(l, -2, "loader");
                                  // -- sol
  lua_pop(l, 1);
                                  // --

  Debug::check_assertion(lua_gettop(l) == 0, "Lua stack is not empty after initialization");

  // Execute the main file.
  do_file_if_exists(l, "main");
  main_on_started();
}

/**
 * \brief Cleans Lua.
 */
void LuaContext::exit() {

  if (l != nullptr) {
    // Call sol.main.on_finished() if it exists.
    main_on_finished();

    // Destroy unfinished objects.
    destroy_menus();
    destroy_timers();
    destroy_drawables();

    // Finalize Lua.
    lua_close(l);
    lua_contexts.erase(l);
    l = nullptr;
  }
}

/**
 * \brief Updates the Lua world.
 *
 * This function is called at each cycle.
 * sol.main.on_update() is called if it exists.
 */
void LuaContext::update() {

  // Make sure the stack does not leak.
  Debug::check_assertion(lua_gettop(l) == 0,
      "Non-empty stack before LuaContext::update()"
  );

  update_drawables();
  update_movements();
  update_menus();
  update_timers();

  // Call sol.main.on_update().
  main_on_update();

  Debug::check_assertion(lua_gettop(l) == 0,
      "Non-empty stack after LuaContext::update()"
  );
}

/**
 * \brief Notifies Lua that an input event has just occurred.
 *
 * The appropriate callback in sol.main is notified.
 *
 * \param event The input event to handle.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::notify_input(const InputEvent& event) {

  Debug::check_assertion(lua_gettop(l) == 0,
      "Non-empty stack before LuaContext::notify_input()"
  );

  // Call the appropriate callback in sol.main (if it exists).
  const bool handled = main_on_input(event);

  Debug::check_assertion(lua_gettop(l) == 0,
      "Non-empty stack after LuaContext::notify_input()"
  );

  return handled;
}

/**
 * \brief Notifies Lua that a map has just been started.
 *
 * The Lua file of this map is automatically loaded.
 *
 * \param map The map started.
 * \param destination The destination point used if it's a normal one,
 * nullptr otherwise.
 */
void LuaContext::run_map(Map& map, Destination* destination) {

  // Compute the file name, depending on the id of the map.
  std::string file_name = std::string("maps/") + map.get_id();

  // Load the map's code.
  load_file(l, file_name);
                                  // map_fun

  // Set a special environment to access map entities like global variables.
  lua_newtable(l);
                                  // map_fun env
  lua_newtable(l);
                                  // map_fun env env_mt
  push_map(l, map);
                                  // map_fun env env_mt map
  // Set our special __index function that gets entities on-demand.
  lua_pushcclosure(l, l_get_map_entity_or_global, 1);
                                  // map_fun env env_mt __index
  lua_setfield(l, -2, "__index");
                                  // map_fun env env_mt
  // We are changing the environment, so we need to also define __newindex
  // with its usual setting (the global table).
  lua_pushvalue(l, LUA_GLOBALSINDEX);
                                  // map_fun env env_mt _G
  lua_setfield(l, -2, "__newindex");
                                  // map_fun env env_mt
  lua_setmetatable(l, -2);
                                  // map_fun env
  lua_setfenv(l, -2);
                                  // map_fun

  // Run the map's code with the map userdata as parameter.
  push_map(l, map);
  call_function(1, 0, file_name.c_str());

  // Call the map:on_started() callback.
  map_on_started(map, destination);
}

/**
 * \brief Notifies Lua that a map has just been suspended or resumed.
 * \param map A map.
 * \param suspended \c true if it is suspended, false if it is resumed.
 */
void LuaContext::notify_map_suspended(Map& map, bool suspended) {

  notify_timers_map_suspended(suspended);   // Notify timers.
  map_on_suspended(map, suspended);  // Call map:on_suspended()
}

/**
 * \brief Notifies the Lua world that an equipment item has just been created.
 *
 * The Lua file of this item is automatically loaded.
 *
 * \param item The item.
 */
void LuaContext::run_item(EquipmentItem& item) {

  // Compute the file name, depending on the id of the equipment item.
  std::string file_name = std::string("items/") + item.get_name();

  // Load the item's code.
  if (load_file_if_exists(l, file_name)) {

    // Run it with the item userdata as parameter.
    push_item(l, item);
    call_function(1, 0, file_name.c_str());

    // Call the item:on_created() callback.
    item_on_created(item);
  }
}

/**
 * \brief Notifies the Lua world that an enemy has just been added to the map.
 *
 * The Lua file of this enemy is automatically loaded.
 *
 * \param enemy The enemy.
 */
void LuaContext::run_enemy(Enemy& enemy) {

  // Compute the file name, depending on enemy's breed.
  std::string file_name = std::string("enemies/") + enemy.get_breed();

  // Load the enemy's code.
  if (load_file_if_exists(l, file_name)) {

    // Run it with the enemy userdata as parameter.
    push_enemy(l, enemy);
    call_function(1, 0, file_name.c_str());
  }

  // TODO parse Lua only once for each breed.
}

/**
 * \brief Notifies the Lua world that a custom entity has just been added to
 * the map.
 *
 * The Lua file of this entity if any is automatically loaded.
 *
 * \param custom_entity The custom entity.
 */
void LuaContext::run_custom_entity(CustomEntity& custom_entity) {

  const std::string& model = custom_entity.get_model();

  if (model.empty()) {
    // No Lua model file to run for this entity.
    return;
  }

  // Compute the file name depending on the model.
  std::string file_name = std::string("entities/") + model;

  // Load the entity's code.
  if (load_file_if_exists(l, file_name)) {

    // Run it with the entity userdata as parameter.
    push_custom_entity(l, custom_entity);
    call_function(1, 0, file_name.c_str());
  }

  // TODO parse Lua only once for each model.
}

/**
 * \brief Notifies Lua that the sequence started by a call to
 * map:move_camera() has reached its target.
 * \param map The current map.
 */
void LuaContext::notify_camera_reached_target(Map& map) {

  // Set a timer to execute the function.
  lua_settop(l, 0);
  push_map(l, map);
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.camera_delay_before");
  lua_pushcfunction(l, l_camera_do_callback);
  timer_api_start(l);
  const TimerPtr& timer = check_timer(l, -1);
  timer->set_suspended_with_map(false);
  lua_settop(l, 0);
}

/**
 * \brief Notifies Lua that a dialog starts.
 * \param game The game.
 * \param dialog The dialog that is becoming active.
 * \param info_ref Lua ref to an optional info parameter to pass to
 * Lua, or an empty ref.
 * \return true if Lua handles the dialog, false otherwise.
 */
bool LuaContext::notify_dialog_started(
    Game& game,
    const Dialog& dialog,
    const ScopedLuaRef& info_ref
) {
  return game_on_dialog_started(game, dialog, info_ref);
}

/**
 * \brief Notifies Lua that a dialog is finished.
 * \param game The game.
 * \param dialog The dialog that was active.
 * \param callback_ref Lua ref of the function to call, or an empty ref.
 * \param status_ref Lua ref to a status value to pass to the callback.
 * "skipped" means that the dialog was canceled by the user.
 */
void LuaContext::notify_dialog_finished(
    Game& game,
    const Dialog& dialog,
    const ScopedLuaRef& callback_ref,
    const ScopedLuaRef& status_ref) {

  game_on_dialog_finished(game, dialog);

  // Execute the callback after game:on_dialog_finished()
  // because the callback may start another dialog.
  if (!callback_ref.is_empty()) {
    push_ref(l, callback_ref);
    if (!status_ref.is_empty()) {
      push_ref(l, status_ref);
    }
    else {
      // No status.
      lua_pushnil(l);
    }
    call_function(1, 0, "dialog callback");
  }
}

/**
 * \brief Creates a reference to the Lua value on top of the stack and pops
 * this value.
 * \return The reference created, wrapped in an object that manages its
 * lifetime.
 */
ScopedLuaRef LuaContext::create_ref() {

  return LuaTools::create_ref(l);
}

/**
 * \brief Pushes onto the stack a Lua value from its reference.
 *
 * Pushes nil if the reference is empty.
 *
 * \param l A Lua state.
 * \param ref Reference to a Lua value.
 */
void LuaContext::push_ref(lua_State* l, const ScopedLuaRef& ref) {

  if (ref.is_empty()) {
    lua_pushnil(l);
    return;
  }

  Debug::check_assertion(ref.get_lua_state() == l, "Wrong Lua state");
  ref.push();
}

/**
 * \brief Returns whether a userdata has an entry with the specified key.
 *
 * Userdata can have entries like tables thanks to special __index and
 * __newindex metamethods.
 *
 * Version with const char*, better for performance if you don't have an
 * std::string representation of the key.
 *
 * \param userdata A userdata.
 * \param key String key to test.
 * \return \c true if this key exists on the userdata.
 */
bool LuaContext::userdata_has_field(
    const ExportableToLua& userdata, const char* key) const {

  // TODO since this function now also checks the metatable, check that
  // doing the work below instead of just returning true is still useful
  // for performance.
  // If not, kill this function.

  // First check the metatable of the type.
  if (userdata_has_metafield(userdata, key)) {
    return true;
  }

  // Check the userdata itself then.
  if (!userdata.is_with_lua_table()) {
    return false;
  }

  const auto& it = userdata_fields.find(&userdata);
  if (it == userdata_fields.end()) {
    return false;
  }

  return it->second.find(key) != it->second.end();
}

/**
 * \brief Returns whether a userdata has an entry with the specified key.
 *
 * Userdata can have entries like tables thanks to special __index and
 * __newindex metamethods.
 *
 * Version with std::string, better for performance if you already have an
 * std::string representation of the key.
 *
 * \param userdata A userdata.
 * \param key String key to test.
 * \return \c true if this key exists on the userdata.
 */
bool LuaContext::userdata_has_field(
    const ExportableToLua& userdata, const std::string& key) const {

  // First check the metatable of the type.
  if (userdata_has_metafield(userdata, key.c_str())) {
    return true;
  }

  // Check the userdata itself then.
  if (!userdata.is_with_lua_table()) {
    return false;
  }

  const auto& it = userdata_fields.find(&userdata);
  if (it == userdata_fields.end()) {
    return false;
  }

  return it->second.find(key) != it->second.end();
}

/**
 * \brief Returns whether the metatable of a userdata has the specified field.
 * \param userdata A userdata.
 * \param key String key to test.
 * \return \c true if this key exists on the userdata's metatable.
 */
bool LuaContext::userdata_has_metafield(
    const ExportableToLua& userdata, const char* key) const {

  // We avoid to push the userdata for performance.
  // Maybe the userdata does not even exist in the Lua side.
                                  // ...
  luaL_getmetatable(l, userdata.get_lua_type_name().c_str());
                                  // ... meta
  lua_pushstring(l, key);
                                  // ... meta key
  lua_rawget(l, -2);
                                  // ... meta field/nil
  const bool found = !lua_isnil(l, -1);
  lua_pop(l, 2);
                                  // ...
  return found;
}

/**
 * \brief Gets a method of the object on top of the stack.
 *
 * This is equivalent to find_method(-1, function_name).
 *
 * \param function_name Name of the function to find in the object.
 * This is not an const std::string& but a const char* on purpose to avoid
 * costly conversions as this function is called very often.
 * \return true if the function was found.
 */
bool LuaContext::find_method(const char* function_name) {

  return find_method(-1, function_name);
}

/**
 * \brief Gets a method of an object.
 *
 * If the method exists, the method and the object are both pushed
 * so that you can call the method immediately with the object as first parameter.
 * If the method is not found, the stack is left unchanged.
 *
 * \param index Index of the object in the stack.
 * \param function_name Name of the function to find in the object.
 * This is not an const std::string& but a const char* on purpose to avoid
 * costly conversions as this function is called very often.
 *
 * \return true if the function was found.
 */
bool LuaContext::find_method(int index, const char* function_name) {

  index = LuaTools::get_positive_index(l, index);
                                  // ... object ...
  lua_getfield(l, index, function_name);
                                  // ... object ... method/?

  bool exists = lua_isfunction(l, -1);
  if (exists) {
                                  // ... object ... method
    lua_pushvalue(l, index);
                                  // ... object ... method object
  }
  else {
    // Restore the stack.
    lua_pop(l, 1);
                                  // ... object ...
  }

  return exists;
}

/**
 * \brief Calls the Lua function with its arguments on top of the stack.
 *
 * This function is like lua_pcall, except that it additionaly handles the
 * error message if an error occurs in the Lua code (the error is printed).
 * This function leaves the results on the stack if there is no error,
 * and leaves nothing on the stack in case of error.
 *
 * \param nb_arguments number of arguments placed on the Lua stack above the
 * function to call
 * \param nb_results number of results expected (you get them on the stack if
 * there is no error)
 * \param function_name A name describing the Lua function (only used to print
 * the error message if any).
 * This is not an const std::string& but a const char* on purpose to avoid
 * costly conversions as this function is called very often.
 * \return true in case of success
 */
bool LuaContext::call_function(
    int nb_arguments,
    int nb_results,
    const char* function_name
) {
  return LuaTools::call_function(l, nb_arguments, nb_results, function_name);
}

/**
 * \brief Opens a script and lets it on top of the stack as a function.
 * \param l A Lua state.
 * \param script_name File name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::load_file(lua_State* l, const std::string& script_name) {

  if (!load_file_if_exists(l, script_name)) {
    Debug::die(std::string("Cannot find script file '") + script_name + "'");
  }
}

/**
 * \brief Opens a script if it exists and lets it on top of the stack as a
 * function.
 *
 * If the file does not exist, the stack is left intact and false is returned.
 *
 * \param l A Lua state.
 * \param script_name File name of the script with or without extension,
 * relative to the data directory.
 * \return true if the file exists and was loaded.
 */
bool LuaContext::load_file_if_exists(lua_State* l, const std::string& script_name) {

  // Determine the file name (possibly adding ".lua").
  std::string file_name(script_name);

  if (!QuestFiles::data_file_exists(file_name)) {
    std::ostringstream oss;
    oss << script_name << ".lua";
    file_name = oss.str();
  }

  if (QuestFiles::data_file_exists(file_name)) {
    // Load the file.
    const std::string& buffer = QuestFiles::data_file_read(file_name);
    int result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

    if (result != 0) {
      Debug::error(std::string("Failed to load script '")
          + script_name + "': " + lua_tostring(l, -1));
    }
    return true;
  }
  return false;
}

/**
 * \brief Opens a Lua file and executes it.
 *
 * This function just calls load_file() and call_function().
 * The file must exist.
 *
 * \param l A Lua state.
 * \param script_name File name of the script without extension,
 * relative to the data directory.
 */
void LuaContext::do_file(lua_State* l, const std::string& script_name) {

  load_file(l, script_name);
  LuaTools::call_function(l, 0, 0, script_name.c_str());
}

/**
 * \brief Opens a Lua file if it exists and executes it without arguments.
 *
 * This function just calls load_file_if_exists() and call_function().
 * Nothing is done if the file does not exists.
 *
 * \param l A Lua state.
 * \param script_name File name of the script without extension,
 * relative to the data directory.
 * \return true if the file exists and was successfully executed.
 */
bool LuaContext::do_file_if_exists(lua_State* l, const std::string& script_name) {

  if (load_file_if_exists(l, script_name)) {
    LuaTools::call_function(l, 0, 0, script_name.c_str());
    return true;
  }
  return false;
}

/**
 * \brief Prints on a line the content of the Lua stack for debugging purposes.
 * \param l A Lua state.
 */
void LuaContext::print_stack(lua_State* l) {

  int i;
  int top = lua_gettop(l);

  for (i = 1; i <= top; i++) {

    int type = lua_type(l, i);
    switch (type) {

      case LUA_TSTRING:
        std::cout << "\"" << lua_tostring(l, i) << "\"";
        break;

      case LUA_TBOOLEAN:
        std::cout << (lua_toboolean(l, i) ? "true" : "false");
        break;

      case LUA_TNUMBER:
        std::cout << lua_tonumber(l, i);
        break;

      case LUA_TLIGHTUSERDATA:
        std::cout << "lightuserdata:" << lua_touserdata(l, i);
        break;

      case LUA_TUSERDATA:
      {
        const ExportableToLuaPtr& userdata = *(static_cast<ExportableToLuaPtr*>(
            lua_touserdata(l, i)));
        const std::string& lua_type_name = userdata->get_lua_type_name();
        std::cout << lua_type_name.substr(lua_type_name.find_last_of('.') + 1);
        break;
      }

      default:
        std::cout << lua_typename(l, type);
        break;

    }
    std::cout << " ";
  }
  std::cout << std::endl;
}

/**
 * \brief Defines some C++ functions into a Lua table.
 * \param module_name name of the table that will contain the functions
 * (e.g. "sol.main").
 * \param functions list of functions to define in the table
 * (must end with {nullptr, nullptr}).
 */
void LuaContext::register_functions(
    const std::string& module_name,
    const luaL_Reg* functions
) {

  // create a table and fill it with the functions
  luaL_register(l, module_name.c_str(), functions);
  lua_pop(l, 1);
}

/**
 * \brief Defines some C++ functions into a new Lua userdata type.
 * \param module_name name of the table that will contain the functions
 * (e.g. "sol.game"). It may already exist or not.
 * This string will also identify the type.
 * \param functions List of functions to define in the module table or nullptr.
 * Must end with {nullptr, nullptr}.
 * \param methods List of methods to define in the type or nullptr.
 * Must end with {nullptr, nullptr}.
 * \param metamethods List of metamethods to define in the metatable of the
 * type or nullptr.
 * Must end with {nullptr, nullptr}.
 */
void LuaContext::register_type(
    const std::string& module_name,
    const luaL_Reg* functions,
    const luaL_Reg* methods,
    const luaL_Reg* metamethods
) {

  // Check that this type does not already exist.
  luaL_getmetatable(l, module_name.c_str());
  Debug::check_assertion(lua_isnil(l, -1),
      std::string("Type ") + module_name + " already exists");
  lua_pop(l, 1);

  // Make sure we create the table.
  const luaL_Reg empty[] = {
      { nullptr, nullptr }
  };
  luaL_register(l, module_name.c_str(), empty);
                                  // module

  // Add the functions to the module.
  if (functions != nullptr) {
    luaL_register(l, nullptr, functions);
                                  // module
  }
  lua_pop(l, 1);
                                  // --

  // Create the metatable for the type, add it to the Lua registry.
  luaL_newmetatable(l, module_name.c_str());
                                  // meta

  // Store a metafield __solarus_type with the module name.
  lua_pushstring(l, module_name.c_str());
                                  // meta type_name
  lua_setfield(l, -2, "__solarus_type");
                                  // meta

  // Add the methods to the metatable.
  if (methods != nullptr) {
    luaL_register(l, nullptr, methods);
  }
                                  // meta

  // Add the metamethods to the metatable.
  if (metamethods != nullptr) {
    luaL_register(l, nullptr, metamethods);
                                  // meta
  }

  // make metatable.__index = metatable,
  // unless if __index is already defined
  lua_getfield(l, -1, "__index");
                                  // meta __index/nil
  lua_pushvalue(l, -2);
                                  // meta __index/nil meta
  if (lua_isnil(l, -2)) {
                                  // meta nil meta
    lua_setfield(l, -3, "__index");
                                  // meta nil
  }
  lua_settop(l, 0);
                                  // --
}

/**
 * \brief Tells the Lua context what C++ functions it can call.
 */
void LuaContext::register_modules() {

  Debug::check_assertion(lua_gettop(l) == 0,
      "Lua stack is not empty before modules initialization");

  register_main_module();
  register_game_module();
  register_map_module();
  register_entity_module();
  register_audio_module();
  register_timer_module();
  register_surface_module();
  register_text_surface_module();
  register_sprite_module();
  register_movement_module();
  register_item_module();
  register_input_module();
  register_video_module();
  register_file_module();
  register_menu_module();
  register_language_module();

  Debug::check_assertion(lua_gettop(l) == 0,
      "Lua stack is not empty after modules initialization");
}

/**
 * \brief Pushes a string.
 * \param l A Lua state.
 * \param text A string value.
 */
void LuaContext::push_string(lua_State* l, const std::string& text) {
  lua_pushlstring(l, text.c_str(), text.size());
}

/**
 * \brief Pushes a color onto the stack.
 * \param l A Lua context.
 * \param color A color.
 */
void LuaContext::push_color(lua_State* l, const Color& color) {

  uint8_t r, g, b, a;
  color.get_components(r, g, b, a);
  lua_newtable(l);
  lua_pushinteger(l, r);
  lua_rawseti(l, -2, 1);
  lua_pushinteger(l, g);
  lua_rawseti(l, -2, 2);
  lua_pushinteger(l, b);
  lua_rawseti(l, -2, 3);
  lua_pushinteger(l, a);
  lua_rawseti(l, -2, 4);
}

/**
 * \brief Pushes the Lua equivalent of a C++ object onto the stack.
 * \param l A Lua context.
 * \param userdata A userdata. It must live as a std::shared_ptr somewhere:
 * typically, it should have been stored in a std::shared_ptr at creation time.
 */
void LuaContext::push_userdata(lua_State* l, ExportableToLua& userdata) {

  // See if this userdata already exists.
  lua_getfield(l, LUA_REGISTRYINDEX, "sol.all_userdata");
                                  // ... all_udata
  lua_pushlightuserdata(l, &userdata);
                                  // ... all_udata lightudata
  lua_gettable(l, -2);
                                  // ... all_udata udata/nil
  if (!lua_isnil(l, -1)) {
                                  // ... all_udata udata
    // The userdata already exists in the Lua world.
    lua_remove(l, -2);
                                  // ... udata
  }
  else {
    // Create a new userdata.

    if (!userdata.is_known_to_lua()) {
      // This is the first time we create a Lua userdata for this object.
      userdata.set_known_to_lua(true);
    }

                                  // ... all_udata nil
    lua_pop(l, 1);
                                  // ... all_udata
    lua_pushlightuserdata(l, &userdata);
                                  // ... all_udata lightudata

    // Find the existing shared_ptr from the raw pointer.
    ExportableToLuaPtr shared_userdata;
    try {
      shared_userdata = userdata.shared_from_this();
    }
    catch (const std::bad_weak_ptr& ex) {
      // No existing shared_ptr. This is probably because you forgot to
      // store your object in a shared_ptr at creation time..
      Debug::die(
          std::string("No living shared_ptr for ") + userdata.get_lua_type_name()
      );
    }

    ExportableToLuaPtr* block_address = static_cast<ExportableToLuaPtr*>(
          lua_newuserdata(l, sizeof(ExportableToLuaPtr))
    );
    // Manually construct a shared_ptr in the block allocated by Lua.
    new (block_address) ExportableToLuaPtr(shared_userdata);
                                  // ... all_udata lightudata udata
    luaL_getmetatable(l, userdata.get_lua_type_name().c_str());
                                  // ... all_udata lightudata udata mt

#ifndef NDEBUG
    Debug::check_assertion(!lua_isnil(l, -1),
        std::string("Userdata of type '" + userdata.get_lua_type_name()
        + "' has no metatable, this is a memory leak"));

    lua_getfield(l, -1, "__gc");
                                  // ... all_udata lightudata udata mt gc
    Debug::check_assertion(lua_isfunction(l, -1),
        std::string("Userdata of type '") + userdata.get_lua_type_name()
        + "' must have the __gc function LuaContext::userdata_meta_gc");
                                  // ... all_udata lightudata udata mt gc
    lua_pop(l, 1);
                                  // ... all_udata lightudata udata mt
#endif

    lua_setmetatable(l, -2);
                                  // ... all_udata lightudata udata
    // Keep track of our new userdata.
    lua_pushvalue(l, -1);
                                  // ... all_udata lightudata udata udata
    lua_insert(l, -4);
                                  // ... udata all_udata lightudata udata
    lua_settable(l, -3);
                                  // ... udata all_udata
    lua_pop(l, 1);
                                  // ... udata
  }
}

/**
 * \brief Returns whether a value is a userdata of a given type.
 * \param l a Lua context
 * \param index an index in the stack
 * \param module_name name of a userdata metatable in the registry
 * \return true if the value is a userdata with this metatable
 */
bool LuaContext::is_userdata(lua_State* l, int index,
    const std::string& module_name) {

  index = LuaTools::get_positive_index(l, index);

                                  // ... udata ...
  void* udata = lua_touserdata(l, index);
  if (udata == nullptr) {
    // This is not a userdata.
    return false;
  }
  if (!lua_getmetatable(l, index)) {
    // The userdata has no metatable.
    return false;
  }
                                  // ... udata ... mt_found
  lua_getfield(l, LUA_REGISTRYINDEX, module_name.c_str());
                                  // ... udata ... mt_found mt_expected
  bool result = lua_rawequal(l, -1, -2);
  lua_pop(l, 2);
                                  // ... udata ...
  return result;
}

/**
 * \brief Checks that the value at the given index is userdata of the
 * specified type and returns it.
 * \param l A Lua state.
 * \param index An index in the Lua stack.
 * \param module_name Name identifying the userdata type.
 * \return The userdata at this index.
 */
const ExportableToLuaPtr& LuaContext::check_userdata(
    lua_State* l,
    int index,
    const std::string& module_name
) {
  index = LuaTools::get_positive_index(l, index);

  const ExportableToLuaPtr& userdata = *(static_cast<ExportableToLuaPtr*>(
    luaL_checkudata(l, index, module_name.c_str())
  ));
  return userdata;
}

/**
 * \brief Finalizer of a userdata type.
 * \param l a Lua state
 * \return number of values to return to Lua
 */
int LuaContext::userdata_meta_gc(lua_State* l) {

  ExportableToLuaPtr* userdata =
      static_cast<ExportableToLuaPtr*>(lua_touserdata(l, 1));

  // Note that the full userdata disappears from Lua but it may come back later!
  // So we need to keep its table if the refcount is not zero.
  // The full userdata is destroyed, but if the refcount is not zero, the light
  // userdata and its table persist.

  // We don't need to remove the entry from sol.all_userdata
  // because it is already done: that table is weak on its values and the
  // value was the full userdata.

  if (userdata->unique()) {
    // The userdata is not used by other people.
    // The object is going to be destroyed from C++ too.

    if ((*userdata)->is_with_lua_table()) {
      // Remove the table associated to this userdata.
      // Otherwise, if the same pointer gets reallocated, a new userdata will get
      // its table from this deleted one!
                                    // udata
      lua_getfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                    // udata udata_tables
      lua_pushlightuserdata(l, userdata->get());
                                    // udata udata_tables lightudata
      lua_pushnil(l);
                                    // udata udata_tables lightudata nil
      lua_settable(l, -3);
                                    // udata udata_tables
      lua_pop(l, 1);
                                    // udata
      get_lua_context(l).userdata_fields.erase(userdata->get());
    }

  }

  // Manually destroy the shared_ptr allocated by Lua.
  userdata->~shared_ptr<ExportableToLua>();

  return 0;
}

/**
 * \brief Implementation of __newindex that allows userdata to be like tables.
 *
 * Lua code can make "object[key] = value" if object is a userdata with this
 * __newindex metamethod.
 *
 * This metamethod must be used with its corresponding __index
 * metamethod (see userdata_meta_index_as_table).
 *
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::userdata_meta_newindex_as_table(lua_State* l) {

  LuaTools::check_type(l, 1, LUA_TUSERDATA);
  LuaTools::check_any(l, 2);
  LuaTools::check_any(l, 3);

  const ExportableToLuaPtr& userdata =
      *(static_cast<ExportableToLuaPtr*>(lua_touserdata(l, 1)));

  // The user wants to make udata[key] = value but udata is a userdata.
  // So what we make instead is udata_tables[udata][key] = value.
  // This redirection is totally transparent from the Lua side.

  lua_getfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                  // ... udata_tables
  lua_pushlightuserdata(l, userdata.get());
                                  // ... udata_tables lightudata
  lua_gettable(l, -2);
                                  // ... udata_tables udata_table/nil
  if (lua_isnil(l, -1)) {
    // Create the userdata table if it does not exist yet.
    userdata->set_with_lua_table(true);
                                  // ... udata_tables nil
    lua_pop(l, 1);
                                  // ... udata_tables
    lua_newtable(l);
                                  // ... udata_tables udata_table
    lua_pushlightuserdata(l, userdata.get());
                                  // ... udata_tables udata_table lightudata
    lua_pushvalue(l, -2);
                                  // ... udata_tables udata_table lightudata udata_table
    lua_settable(l, -4);
                                  // ... udata_tables udata_table
  }
  lua_pushvalue(l, 2);
                                  // ... udata_tables udata_table key
  lua_pushvalue(l, 3);
                                  // ... udata_tables udata_table key value
  lua_settable(l, -3);
                                  // ... udata_tables udata_table

  if (lua_isstring(l, 2)) {
    if (!lua_isnil(l, 3)) {
      // Add the key to the list of existing strings keys on this userdata.
      get_lua_context(l).userdata_fields[userdata.get()].insert(lua_tostring(l, 2));
    }
    else {
      // Assigning nil: remove the key from the list.
      get_lua_context(l).userdata_fields[userdata.get()].erase(lua_tostring(l, 2));
    }
  }

  return 0;
}

/**
 * \brief Implementation of __index that allows userdata to be like tables.
 *
 * Lua code can get "object[key]" for an arbitrary key previously set.
 *
 * This metamethod must be used with its corresponding __newindex
 * metamethod (see userdata_meta_newindex_as_table).
 *
 * \brief Implementation of __index for the type game.
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::userdata_meta_index_as_table(lua_State* l) {

  /* The user wants to make udata[key] but udata is a userdata.
   * So what we retrieve instead is udata_tables[udata][key].
   * This redirection is totally transparent from the Lua side.
   * If udata_tables[udata][key] does not exist, we fall back
   * to the userdata __index metamethod.
   */

  LuaTools::check_type(l, 1, LUA_TUSERDATA);
  LuaTools::check_any(l, 2);

  const ExportableToLuaPtr& userdata =
      *(static_cast<ExportableToLuaPtr*>(lua_touserdata(l, 1)));
  LuaContext& lua_context = get_lua_context(l);

  // If the userdata actually has a table, lookup this table, unless we already
  // know that we won't find it (because we know all the existing string keys).
  if (userdata->is_with_lua_table() &&
      (!lua_isstring(l, 2) || lua_context.userdata_has_field(*userdata, lua_tostring(l, 2)))) {

    lua_getfield(l, LUA_REGISTRYINDEX, "sol.userdata_tables");
                                  // ... udata_tables
    lua_pushlightuserdata(l, userdata.get());
                                  // ... udata_tables lightudata
    // Lookup the key in the table, without metamethods.
    lua_rawget(l, -2);
                                  // ... udata_tables udata_table/nil
    if (!lua_isnil(l, -1)) {
      lua_pushvalue(l, 2);
                                  // ... udata_tables udata_table key
      lua_gettable(l, -2);
                                  // ... udata_tables udata_table value/nil
      if (!lua_isnil(l, -1)) {
        // Found it!
        return 1;
      }
    }
  }

  // Not in the table. See in the metatable
  // (just like when metatable.__index = metatable).

  lua_pushvalue(l, 1);
                                  // ... udata
  lua_getmetatable(l, -1);
                                  // ... udata meta
  Debug::check_assertion(!lua_isnil(l, -1), "Missing userdata metatable");
  lua_pushvalue(l, 2);
                                  // ... udata meta key
  lua_gettable(l, -2);
                                  // ... udata meta key value/nil
  return 1;
}

/**
 * \brief Calls the on_started() method of the object on top of the stack.
 */
void LuaContext::on_started() {

  if (find_method("on_started")) {
    call_function(1, 0, "on_started");
  }
}

/**
 * \brief Calls the on_finished() method of the object on top of the stack.
 */
void LuaContext::on_finished() {

  if (find_method("on_finished")) {
    call_function(1, 0, "on_finished");
  }
}

/**
 * \brief Calls the on_update() method of the object on top of the stack.
 */
void LuaContext::on_update() {

  if (find_method("on_update")) {
    call_function(1, 0, "on_update");
  }
}

/**
 * \brief Calls the on_draw() method of the object on top of the stack.
 * \param dst_surface The destination surface.
 */
void LuaContext::on_draw(const SurfacePtr& dst_surface) {

  if (find_method("on_draw")) {
    push_surface(l, *dst_surface);
    call_function(2, 0, "on_draw");
  }
}

/**
 * \brief Calls the on_pre_draw() method of the object on top of the stack.
 * \param dst_surface The destination surface.
 */
void LuaContext::on_pre_draw(const SurfacePtr& dst_surface) {

  if (find_method("on_pre_draw")) {
    push_surface(l, *dst_surface);
    call_function(2, 0, "on_pre_draw");
  }
}

/**
 * \brief Calls the on_post_draw() method of the object on top of the stack.
 * \param dst_surface The destination surface.
 */
void LuaContext::on_post_draw(const SurfacePtr& dst_surface) {

  if (find_method("on_post_draw")) {
    push_surface(l, *dst_surface);
    call_function(2, 0, "on_post_draw");
  }
}

/**
 * \brief Calls the on_suspended() method of the object on top of the stack.
 * \param suspended true to suspend the object, false to unsuspend it.
 */
void LuaContext::on_suspended(bool suspended) {

  if (find_method("on_suspended")) {
    lua_pushboolean(l, suspended);
    call_function(2, 0, "on_suspended");
  }
}

/**
 * \brief Calls the on_paused() method of the object on top of the stack.
 */
void LuaContext::on_paused() {

  if (find_method("on_paused")) {
    call_function(1, 0, "on_paused");
  }
}

/**
 * \brief Calls the on_unpaused() method of the object on top of the stack.
 */
void LuaContext::on_unpaused() {

  if (find_method("on_unpaused")) {
    call_function(1, 0, "on_unpaused");
  }
}

/**
 * \brief Calls the on_dialog_started() method of the object on top of the
 * stack.
 * \param dialog The dialog that just started.
 * \param info_ref Lua ref to the info parameter to pass to the method,
 * or an empty ref.
 * \return true if the on_dialog_started() method is defined.
 */
bool LuaContext::on_dialog_started(
    const Dialog& dialog,
    const ScopedLuaRef& info_ref
) {
  if (find_method("on_dialog_started")) {
    push_dialog(l, dialog);
    push_ref(l, info_ref);
    call_function(3, 0, "on_dialog_started");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_dialog_finished() method of the object on top of the stack.
 * \param dialog The dialog that has just finished.
 */
void LuaContext::on_dialog_finished(const Dialog& dialog) {

  if (find_method("on_dialog_finished")) {
    push_dialog(l, dialog);
    call_function(2, 0, "on_dialog_finished");
  }
}

/**
 * \brief Calls the on_game_over_started() method of the object on top of the stack.
 */
bool LuaContext::on_game_over_started() {

  if (find_method("on_game_over_started")) {
    call_function(1, 0, "on_game_over_started");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_game_over_finished() method of the object on top of the stack.
 */
void LuaContext::on_game_over_finished() {

  if (find_method("on_game_over_finished")) {
    call_function(1, 0, "on_game_over_finished");
  }
}

/**
 * \brief Calls an input callback method of the object on top of the stack.
 * \param event The input event to forward.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_input(const InputEvent& event) {

  // Call the Lua function(s) corresponding to this input event.
  bool handled = false;
  if (event.is_keyboard_event()) {
    // Keyboard.
    if (event.is_keyboard_key_pressed()) {
      handled = on_key_pressed(event) || handled;
    }
    else if (event.is_keyboard_key_released()) {
      handled = on_key_released(event) || handled;
    }
  }
  else if (event.is_character_pressed()) {
    handled = on_character_pressed(event) || handled;
  }
  else if (event.is_joypad_event()) {
    // Joypad.
    if (event.is_joypad_button_pressed()) {
      handled = on_joypad_button_pressed(event) || handled;
    }
    else if (event.is_joypad_button_released()) {
      handled = on_joypad_button_released(event) || handled;
    }
    else if (event.is_joypad_axis_moved()) {
      handled = on_joypad_axis_moved(event) || handled;
    }
    else if (event.is_joypad_hat_moved()) {
      handled = on_joypad_hat_moved(event) || handled;
    }
  }
  else if (event.is_mouse_event()) {
    // Mouse.
    if (event.is_mouse_button_pressed()) {
      handled = on_mouse_button_pressed(event) || handled;
    }
    else if (event.is_mouse_button_released()) {
      handled = on_mouse_button_released(event) || handled;
    }
  }

  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a keyboard key was just pressed
 * (including if it is a directional key or a character).
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_key_pressed(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_key_pressed")) {

    const std::string& key_name = InputEvent::get_keyboard_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Solarus API.

      push_string(l, key_name);
      lua_newtable(l);

      if (event.is_with_shift()) {
        lua_pushboolean(l, 1);
        lua_setfield(l, -2, "shift");
      }

      if (event.is_with_control()) {
        lua_pushboolean(l, 1);
        lua_setfield(l, -2, "control");
      }

      if (event.is_with_alt()) {
        lua_pushboolean(l, 1);
        lua_setfield(l, -2, "alt");
      }
      bool success = call_function(3, 1, "on_key_pressed");
      if (!success) {
        // Something was wrong in the script: don't propagate the input to other objects.
        handled = true;
      }
      else {
        handled = lua_toboolean(l, -1);
        lua_pop(l, 1);
      }
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a character was just pressed with the keyboard.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_character_pressed(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_character_pressed")) {

    const std::string& character = event.get_character();
    push_string(l, character);
    bool success = call_function(2, 1, "on_character_pressed");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a keyboard key was just released
 * (including if it is a directional key).
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_key_released(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_key_released")) {

    const std::string& key_name = InputEvent::get_keyboard_key_name(event.get_keyboard_key());
    if (!key_name.empty()) { // This key exists in the Solarus API.
      push_string(l, key_name);
      bool success = call_function(2, 1, "on_key_released");
      if (!success) {
        // Something was wrong in the script: don't propagate the input to other objects.
        handled = true;
      }
      else {
        handled = lua_toboolean(l, -1);
        lua_pop(l, 1);
      }
    }
    else {
      // The method exists but the key is unknown.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad button was just pressed.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_button_pressed(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joypad_button_pressed")) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    bool success = call_function(2, 1, "on_joypad_button_pressed");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad button was just released.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_button_released(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joypad_button_released")) {
    int button = event.get_joypad_button();

    lua_pushinteger(l, button);
    bool success = call_function(2, 1, "on_joypad_button_released");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad axis was just moved.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_axis_moved(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joypad_axis_moved")) {
    int axis = event.get_joypad_axis();
    int state = event.get_joypad_axis_state();

    lua_pushinteger(l, axis);
    lua_pushinteger(l, state);
    bool success = call_function(3, 1, "on_joypad_axis_moved");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a joypad hat was just moved.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_joypad_hat_moved(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_joypad_hat_moved")) {
    int hat = event.get_joypad_hat();
    int direction8 = event.get_joypad_hat_direction();

    lua_pushinteger(l, hat);
    lua_pushinteger(l, direction8);
    bool success = call_function(3, 1, "on_joypad_hat_moved");
    if (!success) {
      // Something was wrong in the script: don't propagate the input to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a mouse button was just pressed.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_mouse_button_pressed(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_mouse_pressed")) {

    const std::string& button_name = InputEvent::get_mouse_button_name(event.get_mouse_button());
    const Rectangle& position = event.get_mouse_position();

    // Don't call the lua event if this button doesn't exists in the Solarus API
    // or if the mouse position is not inside the viewport.
    if (!button_name.empty() && !position.is_flat()) {

      push_string(l, button_name);
      lua_pushinteger(l, position.get_x());
      lua_pushinteger(l, position.get_y());

      bool success = call_function(4, 1, "on_mouse_pressed");
      if (!success) {
        // Something was wrong in the script: don't propagate the input to other objects.
        handled = true;
      }
      else {
        handled = lua_toboolean(l, -1);
        lua_pop(l, 1);
      }
    }
    else {
      // The method exists but parameters are not congruent.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * \brief Notifies the object on top of the stack
 * that a mouse button was just released.
 * \param event The corresponding input event.
 * \return \c true if the event was handled and should stop being propagated.
 */
bool LuaContext::on_mouse_button_released(const InputEvent& event) {

  bool handled = false;
  if (find_method("on_mouse_released")) {

    const std::string& button_name = InputEvent::get_mouse_button_name(event.get_mouse_button());
    const Rectangle& position = event.get_mouse_position();

    // Don't call the lua event if this button doesn't exists in the Solarus API
    // or if the mouse position is not inside the viewport.
    if (!button_name.empty() && !position.is_flat()) {

      push_string(l, button_name);
      lua_pushinteger(l, position.get_x());
      lua_pushinteger(l, position.get_y());

      bool success = call_function(4, 1, "on_mouse_released");
      if (!success) {
        // Something was wrong in the script: don't propagate the input to other objects.
        handled = true;
      }
      else {
        handled = lua_toboolean(l, -1);
        lua_pop(l, 1);
      }
    }
    else {
      // The method exists but parameters are not congruent.
      lua_pop(l, 2);  // Pop the object and the method.
    }
  }
  return handled;
}

/**
 * \brief Calls the on_command_pressed() method of the object on top of the stack.
 * \param command The game command just pressed.
 */
bool LuaContext::on_command_pressed(GameCommand command) {

  bool handled = false;
  if (find_method("on_command_pressed")) {
    push_string(l, GameCommands::get_command_name(command));
    bool success = call_function(2, 1, "on_command_pressed");
    if (!success) {
      // Something was wrong in the script: don't propagate the command to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Calls the on_command_released() method of the object on top of the stack.
 * \param command The game command just pressed.
 */
bool LuaContext::on_command_released(GameCommand command) {

  bool handled = false;
  if (find_method("on_command_released")) {
    push_string(l, GameCommands::get_command_name(command));
    bool success = call_function(2, 1, "on_command_released");
    if (!success) {
      // Something was wrong in the script: don't propagate the command to other objects.
      handled = true;
    }
    else {
      handled = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return handled;
}

/**
 * \brief Calls the on_animation_finished() method of the object on top of the stack.
 * \param animation Name of the animation finished.
 */
void LuaContext::on_animation_finished(const std::string& animation) {

  if (find_method("on_animation_finished")) {
    push_string(l, animation);
    call_function(2, 0, "on_animation_finished");
  }
}

/**
 * \brief Calls the on_animation_changed() method of the object on top of the stack.
 * \param animation Name of the new animation.
 */
void LuaContext::on_animation_changed(const std::string& animation) {

  if (find_method("on_animation_changed")) {
    push_string(l, animation);
    call_function(2, 0, "on_animation_changed");
  }
}

/**
 * \brief Calls the on_direction_changed() method of the object on top of the stack.
 * \param animation Name of the sprite animation.
 * \param frame The new frame.
 */
void LuaContext::on_direction_changed(
    const std::string& animation, int direction) {

  if (find_method("on_direction_changed")) {
    push_string(l, animation);
    lua_pushinteger(l, direction);
    call_function(3, 0, "on_direction_changed");
  }
}

/**
 * \brief Calls the on_frame_changed() method of the object on top of the stack.
 * \param animation Name of the sprite animation.
 * \param frame The new frame.
 */
void LuaContext::on_frame_changed(const std::string& animation, int frame) {

  if (find_method("on_frame_changed")) {
    push_string(l, animation);
    lua_pushinteger(l, frame);
    call_function(3, 0, "on_frame_changed");
  }
}

/**
 * \brief Calls the on_position_changed() method of the object on top of the stack.
 * \param xy The new coordinates.
 */
void LuaContext::on_position_changed(const Point& xy) {

  if (find_method("on_position_changed")) {
    lua_pushinteger(l, xy.x);
    lua_pushinteger(l, xy.y);
    call_function(3, 0, "on_position_changed");
  }
}

/**
 * \brief Calls the on_obstacle_reached() method of the object on top of the stack.
 */
void LuaContext::on_obstacle_reached() {

  if (find_method("on_obstacle_reached")) {
    call_function(1, 0, "on_obstacle_reached");
  }
}

/**
 * \brief Calls the on_changed() method of the object on top of the stack.
 */
void LuaContext::on_changed() {

  if (find_method("on_changed")) {
    call_function(1, 0, "on_changed");
  }
}

/**
 * \brief Calls the on_started() method of the object on top of the stack.
 * \param destination The destination point used (nullptr if it's a special one).
 */
void LuaContext::on_started(Destination* destination) {

  if (find_method("on_started")) {
    if (destination == nullptr) {
      lua_pushnil(l);
    }
    else {
      push_entity(l, *destination);
    }
    call_function(2, 0, "on_started");
  }
}

/**
 * \brief Calls the on_opening_transition_finished() method of the object on top of the stack.
 * \param destination The destination point used (nullptr if it's a special one).
 */
void LuaContext::on_opening_transition_finished(Destination* destination) {

  if (find_method("on_opening_transition_finished")) {
    if (destination == nullptr) {
      lua_pushnil(l);
    }
    else {
      push_entity(l, *destination);
    }
    call_function(2, 0, "on_opening_transition_finished");
  }
}

/**
 * \brief Calls the on_camera_back() method of the object on top of the stack.
 */
void LuaContext::on_camera_back() {

  if (find_method("on_camera_back")) {
    call_function(1, 0, "on_camera_back");
  }
}

/**
 * \brief Calls the on_obtaining_treasure() method of the object on top of the stack.
 * \param treasure The treasure being obtained.
 */
void LuaContext::on_obtaining_treasure(const Treasure& treasure) {

  if (find_method("on_obtaining_treasure")) {
    push_item(l, treasure.get_item());
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(4, 0, "on_obtaining_treasure");
  }
}

/**
 * \brief Calls the on_obtained_treasure() method of the object on top of the stack.
 * \param treasure The treasure just obtained.
 */
void LuaContext::on_obtained_treasure(const Treasure& treasure) {

  if (find_method("on_obtained_treasure")) {
    push_item(l, treasure.get_item());
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(4, 0, "on_obtained_treasure");
  }
}

/**
 * \brief Calls the on_state_changed() method of the object on top of the stack.
 * \param state_name A name describing the new state.
 */
void LuaContext::on_state_changed(const std::string& state_name) {

  if (find_method("on_state_changed")) {
    push_string(l, state_name);
    call_function(2, 0, "on_state_changed");
  }
}

/**
 * \brief Calls the on_taking_damage() method of the object on top of the stack.
 * \param damage The damage to take.
 */
bool LuaContext::on_taking_damage(int damage) {

  if (find_method("on_taking_damage")) {
    lua_pushinteger(l, damage);
    call_function(2, 0, "on_taking_damage");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_activating() method of the object on top of the stack.
 */
void LuaContext::on_activating() {

  if (find_method("on_activating")) {
    call_function(1, 0, "on_activating");
  }
}

/**
 * \brief Calls the on_activating() method of the object on top of the stack.
 * \param direction Direction to pass as parameter.
 */
void LuaContext::on_activating(int direction) {

  if (find_method("on_activating")) {
    lua_pushinteger(l, direction);
    call_function(2, 0, "on_activating");
  }
}

/**
 * \brief Calls the on_activated() method of the object on top of the stack.
 */
void LuaContext::on_activated() {

  if (find_method("on_activated")) {
    call_function(1, 0, "on_activated");
  }
}

/**
 * \brief Calls the on_activated() method of the object on top of the stack.
 * \param direction Direction to pass as parameter.
 */
void LuaContext::on_activated(int direction) {

  if (find_method("on_activated")) {
    lua_pushinteger(l, direction);
    call_function(2, 0, "on_activated");
  }
}

/**
 * \brief Calls the on_inactivated_repeat() method of the object on top of the stack.
 */
void LuaContext::on_activated_repeat() {

  if (find_method("on_activated_repeat")) {
    call_function(1, 0, "on_activated_repeat");
  }
}

/**
 * \brief Calls the on_inactivated() method of the object on top of the stack.
 */
void LuaContext::on_inactivated() {

  if (find_method("on_inactivated")) {
    call_function(1, 0, "on_inactivated");
  }
}

/**
 * \brief Calls the on_left() method of the object on top of the stack.
 */
void LuaContext::on_left() {

  if (find_method("on_left")) {
    call_function(1, 0, "on_left");
  }
}

/**
 * \brief Calls the on_npc_interaction() method of the object on top of the stack.
 * \param npc An NPC.
 */
void LuaContext::on_npc_interaction(Npc& npc) {

  if (find_method("on_npc_interaction")) {
    push_npc(l, npc);
    call_function(2, 0, "on_npc_interaction");
  }
}

/**
 * \brief Calls the on_npc_interaction_item() method of the object on top of the stack.
 * \param npc An NPC.
 * \param item_used The equipment item used.
 * \return true if an interaction occurred.
 */
bool LuaContext::on_npc_interaction_item(Npc& npc, EquipmentItem& item_used) {

  bool interacted = false;
  if (find_method("on_npc_interaction_item")) {
    push_npc(l, npc);
    push_item(l, item_used);
    bool success = call_function(3, 1, "on_npc_interaction_item");
    if (!success) {
      // Something was wrong in the script: don't propagate the event to other objects.
      interacted = true;
    }
    else {
      interacted = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return interacted;
}

/**
 * \brief Calls the on_interaction() method of the object on top of the stack.
 * \return true if an interaction occurred.
 */
bool LuaContext::on_interaction() {

  if (find_method("on_interaction")) {
    call_function(1, 0, "on_interaction");
    return true;
  }

  return false;
}

/**
 * \brief Calls the on_interaction_item() method of the object on top of the stack.
 * \param item_used The equipment item used.
 * \return true if an interaction occurred.
 */
bool LuaContext::on_interaction_item(EquipmentItem& item) {

  bool interacted = false;
  if (find_method("on_interaction_item")) {
    push_item(l, item);
    bool success = call_function(2, 1, "on_interaction_item");
    if (!success) {
      // Something was wrong in the script: don't propagate the event to other objects.
      interacted = true;
    }
    else {
      interacted = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return interacted;
}

/**
 * \brief Calls the on_npc_collision_fire() method of the object on top of the stack.
 * \param npc An NPC.
 */
void LuaContext::on_npc_collision_fire(Npc& npc) {

  if (find_method("on_npc_collision_fire")) {
    push_npc(l, npc);
    call_function(2, 0, "on_npc_collision_fire");
  }
}

/**
 * \brief Calls the on_collision_fire() method of the object on top of the stack.
 */
void LuaContext::on_collision_fire() {

  if (find_method("on_collision_fire")) {
    call_function(1, 0, "on_collision_fire");
  }
}

/**
 * \brief Calls the on_collision_explosion() method of the object on top of the stack.
 */
void LuaContext::on_collision_explosion() {

  if (find_method("on_collision_explosion")) {
    call_function(1, 0, "on_collision_explosion");
  }
}

/**
 * \brief Calls the on_empty() method of the object on top of the stack.
 * \return \c true if the on_empty() method is defined.
 */
bool LuaContext::on_empty() {

  if (find_method("on_empty")) {
    call_function(1, 0, "on_empty");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_buying() method of the object on top of the stack.
 * \return true if the player is allowed to buy the item.
 */
bool LuaContext::on_buying() {

  bool can_buy = true;
  if (find_method("on_buying")) {
    bool success = call_function(1, 1, "on_buying");
    if (!success) {
      // Something was wrong in the script: don't let the player buy the item.
      can_buy = false;
    }
    else {
      can_buy = lua_toboolean(l, -1);
      lua_pop(l, 1);
    }
  }
  return can_buy;
}

/**
 * \brief Calls the on_bought() method of the object on top of the stack.
 */
void LuaContext::on_bought() {

  if (find_method("on_bought")) {
    call_function(1, 0, "on_bought");
  }
}

/**
 * \brief Calls the on_opened() method of the object on top of the stack.
 */
void LuaContext::on_opened() {

  if (find_method("on_opened")) {
    call_function(1, 0, "on_opened");
  }
}

/**
 * \brief Calls the on_closed() method of the object on top of the stack.
 */
void LuaContext::on_closed() {

  if (find_method("on_closed")) {
    call_function(1, 0, "on_closed");
  }
}

/**
 * \brief Calls the on_moving() method of the object on top of the stack.
 */
void LuaContext::on_moving() {

  if (find_method("on_moving")) {
    call_function(1, 0, "on_moving");
  }
}

/**
 * \brief Calls the on_moved() method of the object on top of the stack.
 */
void LuaContext::on_moved() {

  if (find_method("on_moved")) {
    call_function(1, 0, "on_moved");
  }
}

/**
 * \brief Calls the on_map_changed() method of the object on top of the stack.
 * \param map The new active map.
 */
void LuaContext::on_map_changed(Map& map) {

  if (find_method("on_map_changed")) {
    push_map(l, map);
    call_function(2, 0, "on_map_changed");
  }
}

/**
 * \brief Calls the on_pickable_created() method of the object on top of the stack.
 * \param pickable A pickable treasure.
 */
void LuaContext::on_pickable_created(Pickable& pickable) {

  if (find_method("on_pickable_created")) {
    push_entity(l, pickable);
    call_function(2, 0, "on_pickable_created");
  }
}

/**
 * \brief Calls the on_variant_changed() method of the object on top of the stack.
 * \param variant Variant of an equipment item.
 */
void LuaContext::on_variant_changed(int variant) {

  if (find_method("on_variant_changed")) {
    lua_pushinteger(l, variant);
    call_function(2, 0, "on_variant_changed");
  }
}

/**
 * \brief Calls the on_amount_changed() method of the object on top of the stack.
 * \param amount Amount of an equipment item.
 */
void LuaContext::on_amount_changed(int amount) {

  if (find_method("on_amount_changed")) {
    lua_pushinteger(l, amount);
    call_function(2, 0, "on_amount_changed");
  }
}

/**
 * \brief Calls the on_obtaining() method of the object on top of the stack.
 * \param treasure The treasure being obtained.
 */
void LuaContext::on_obtaining(const Treasure& treasure) {

  if (find_method("on_obtaining")) {
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(3, 0, "on_obtaining");
  }
}

/**
 * \brief Calls the on_obtained() method of the object on top of the stack.
 * \param treasure The treasure just obtained.
 */
void LuaContext::on_obtained(const Treasure& treasure) {

  if (find_method("on_obtained")) {
    lua_pushinteger(l, treasure.get_variant());
    if (!treasure.is_saved()) {
      lua_pushnil(l);
    }
    else {
      lua_pushstring(l, treasure.get_savegame_variable().c_str());
    }
    call_function(3, 0, "on_obtained");
  }
}

/**
 * \brief Calls the on_using() method of the object on top of the stack.
 */
void LuaContext::on_using() {

  if (find_method("on_using")) {
    call_function(1, 0, "on_using");
  }
}

/**
 * \brief Calls the on_ability_used() method of the object on top of the stack.
 * \param ability A built-in ability.
 */
void LuaContext::on_ability_used(Ability ability) {

  if (find_method("on_ability_used")) {
    push_string(l, Equipment::ability_names.find(ability)->second);
    call_function(2, 0, "on_ability_used");
  }
}

/**
 * \brief Calls the on_created() method of the object on top of the stack.
 */
void LuaContext::on_created() {

  if (find_method("on_created")) {
    call_function(1, 0, "on_created");
  }
}

/**
 * \brief Calls the on_removed() method of the object on top of the stack.
 */
void LuaContext::on_removed() {

  if (find_method("on_removed")) {
    call_function(1, 0, "on_removed");
  }
}

/**
 * \brief Calls the on_enabled() method of the object on top of the stack.
 */
void LuaContext::on_enabled() {

  if (find_method("on_enabled")) {
    call_function(1, 0, "on_enabled");
  }
}

/**
 * \brief Calls the on_disabled() method of the object on top of the stack.
 */
void LuaContext::on_disabled() {

  if (find_method("on_disabled")) {
    call_function(1, 0, "on_disabled");
  }
}

/**
 * \brief Calls the on_restarted() method of the object on top of the stack.
 */
void LuaContext::on_restarted() {

  if (find_method("on_restarted")) {
    call_function(1, 0, "on_restarted");
  }
}

/**
 * \brief Calls the on_pre_draw() method of the object on top of the stack.
 */
void LuaContext::on_pre_draw() {

  if (find_method("on_pre_draw")) {
    call_function(1, 0, "on_pre_draw");
  }
}

/**
 * \brief Calls the on_post_draw() method of the object on top of the stack.
 */
void LuaContext::on_post_draw() {

  if (find_method("on_post_draw")) {
    call_function(1, 0, "on_post_draw");
  }
}

/**
 * \brief Calls the on_position_changed() method of the object on top of the stack.
 * \param xy The new position.
 * \param layer The new layer.
 */
void LuaContext::on_position_changed(const Point& xy, Layer layer) {

  if (find_method("on_position_changed")) {
    lua_pushinteger(l, xy.x);
    lua_pushinteger(l, xy.y);
    lua_pushinteger(l, layer);
    call_function(4, 0, "on_position_changed");
  }
}

/**
 * \brief Calls the on_obstacle_reached() method of the object on top of the stack.
 * \param movement The movement that reached an obstacle.
 */
void LuaContext::on_obstacle_reached(Movement& movement) {

  if (find_method("on_obstacle_reached")) {
    push_movement(l, movement);
    call_function(2, 0, "on_obstacle_reached");
  }
}

/**
 * \brief Calls the on_movement_changed() method of the object on top of the stack.
 * \param movement A movement.
 */
void LuaContext::on_movement_changed(Movement& movement) {

  if (find_method("on_movement_changed")) {
    push_movement(l, movement);
    call_function(2, 0, "on_movement_changed");
  }
}

/**
 * \brief Calls the on_movement_finished() method of the object on top of the stack.
 */
void LuaContext::on_movement_finished() {

  if (find_method("on_movement_finished")) {
    call_function(1, 0, "on_movement_finished");
  }
}

/**
 * \brief Calls the on_collision_enemy() method of the object on top of the stack.
 * \param other_enemy Another enemy colliding with the object on top of the stack.
 * \param other_sprite Colliding sprite of the other enemy.
 * \param this_sprite Colliding sprite of the first enemy.
 */
void LuaContext::on_collision_enemy(Enemy& other_enemy, Sprite& other_sprite, Sprite& this_sprite) {

  if (find_method("on_collision_enemy")) {
    push_enemy(l, other_enemy);
    push_sprite(l, other_sprite);
    push_sprite(l, this_sprite);
    call_function(4, 0, "on_collision_enemy");
  }
}

/**
 * \brief Calls the on_looked() method of the object on top of the stack.
 */
void LuaContext::on_looked() {

  if (find_method("on_looked")) {
    call_function(1, 0, "on_looked");
  }
}

/**
 * \brief Calls the on_cut() method of the object on top of the stack.
 */
void LuaContext::on_cut() {

  if (find_method("on_cut")) {
    call_function(1, 0, "on_cut");
  }
}

/**
 * \brief Calls the on_lifting() method of the object on top of the stack.
 */
void LuaContext::on_lifting() {

  if (find_method("on_lifting")) {
    call_function(1, 0, "on_lifting");
  }
}

/**
 * \brief Calls the on_exploded() method of the object on top of the stack.
 */
void LuaContext::on_exploded() {

  if (find_method("on_exploded")) {
    call_function(1, 0, "on_exploded");
  }
}

/**
 * \brief Calls the on_regenerating() method of the object on top of the stack.
 */
void LuaContext::on_regenerating() {

  if (find_method("on_regenerating")) {
    call_function(1, 0, "on_regenerating");
  }
}

/**
 * \brief Calls the on_custom_attack_received() method of the object on top of the stack.
 * \param attack The attack received.
 * \param sprite The sprite that receives the attack if any.
 */
void LuaContext::on_custom_attack_received(EnemyAttack attack, Sprite* sprite) {

  if (find_method("on_custom_attack_received")) {
    push_string(l, Enemy::attack_names.find(attack)->second);
    if (sprite != nullptr) {
      // Pixel-precise collision.
      push_sprite(l, *sprite);
      call_function(3, 0, "on_custom_attack_received");
    }
    else {
      call_function(2, 0, "on_custom_attack_received");
    }
  }
}

/**
 * \brief Calls the on_hurt_by_sword() method of the object on top of the stack.
 * \param hero The hero whose sword is hitting the enemy.
 * \param enemy_sprite Sprite of the enemy that gets hits.
 * \return \c true if the method is defined.
 */
bool LuaContext::on_hurt_by_sword(Hero& hero, Sprite& enemy_sprite) {

  if (find_method("on_hurt_by_sword")) {
    push_hero(l, hero);
    push_sprite(l, enemy_sprite);
    call_function(3, 0, "on_hurt_by_sword");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_hurt() method of the object on top of the stack.
 * \param attack The attack received.
 */
void LuaContext::on_hurt(EnemyAttack attack) {

  if (find_method("on_hurt")) {
    push_string(l, Enemy::attack_names.find(attack)->second);
    call_function(2, 0, "on_hurt");
  }
}

/**
 * \brief Calls the on_dying() method of the object on top of the stack.
 */
void LuaContext::on_dying() {

  if (find_method("on_dying")) {
    call_function(1, 0, "on_dying");
  }
}

/**
 * \brief Calls the on_dead() method of the object on top of the stack.
 */
void LuaContext::on_dead() {

  if (find_method("on_dead")) {
    call_function(1, 0, "on_dead");
  }
}

/**
 * \brief Calls the on_immobilized() method of the object on top of the stack.
 */
void LuaContext::on_immobilized() {

  if (find_method("on_immobilized")) {
    call_function(1, 0, "on_immobilized");
  }
}

/**
 * \brief Calls the on_attacking_hero() method of the object on top of the stack.
 * \param hero The hero attacked.
 * \param attacker_sprite Sprite that caused the collision or nullptr.
 * \return \c true if the method is defined.
 */
bool LuaContext::on_attacking_hero(Hero& hero, Sprite* attacker_sprite) {

  if (find_method("on_attacking_hero")) {
    push_hero(l, hero);
    if (attacker_sprite == nullptr) {
      lua_pushnil(l);
    }
    else {
      push_sprite(l, *attacker_sprite);
    }
    call_function(3, 0, "on_attacking_hero");
    return true;
  }
  return false;
}

/**
 * \brief Calls the on_ground_below_changed() method of the object on top of the stack.
 * \param ground_below The new ground below the object.
 */
void LuaContext::on_ground_below_changed(Ground ground_below) {

  if (find_method("on_ground_below_changed")) {
    if (ground_below == Ground::EMPTY) {
      lua_pushnil(l);
    }
    else {
      push_string(l, GroundInfo::get_ground_name(ground_below));
    }
    call_function(2, 0, "on_ground_below_changed");
  }
}

/**
 * \brief Function called when an unprotected Lua error occurs.
 * \param l The Lua context.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_panic(lua_State* l) {

  const std::string& error = luaL_checkstring(l, 1);

  Debug::die(error);

  return 0;
}

/**
 * \brief A loader that makes require() able to load Lua files
 * from the quest data directory or archive.
 * \param l The Lua context.
 * \return Number of values to return to Lua.
 */
int LuaContext::l_loader(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {
    const std::string& script_name = luaL_checkstring(l, 1);
    bool exists = load_file_if_exists(l, script_name);

    if (!exists) {
      std::ostringstream oss;
      oss << std::endl << "\tno quest file '" << script_name
          << ".lua' in 'data/', 'data.solarus' or 'data.solarus.zip'";
      push_string(l, oss.str());
    }
    return 1;
  });
}

}

