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
#include "QuestProperties.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lua/LuaContext.h"
#include <lua.hpp>

/**
 * @brief Creates the quest properties loader.
 * @param main_loop The quest main loop.
 */
QuestProperties::QuestProperties(MainLoop& main_loop):
  main_loop(main_loop) {

}

/**
 * @brief Destructor.
 */
QuestProperties::~QuestProperties() {

}

/**
 * @brief Reads the quest properties data file quest.lua and applies
 * these properties to the quest.
 */
void QuestProperties::load() {

  // Read the quest properties file.
  const std::string& file_name = "quest.lua";
  lua_State* l = lua_open();
  size_t size;
  char* buffer;
  FileTools::data_file_open_buffer(file_name, &buffer, &size);
  luaL_loadbuffer(l, buffer, size, file_name.c_str());
  FileTools::data_file_close_buffer(buffer);

  lua_register(l, "quest", l_quest);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::die(StringConcat() << "Error: failed to load quest.lua: "
        << lua_tostring(l, -1));
    lua_pop(l, 1);
  }

  lua_close(l);
}

int QuestProperties::l_quest(lua_State* l) {

  // Retrieve the quest properties from the table parameter.
  luaL_checktype(l, 1, LUA_TTABLE);
  const std::string& quest_write_dir = LuaContext::check_string_field(l, 1, "write_dir");
  const std::string& title_bar = LuaContext::check_string_field(l, 1, "title_bar");

  FileTools::set_quest_write_dir(quest_write_dir);
  VideoManager::get_instance()->set_window_title(title_bar);

  return 0;
}

