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
#include "solarus/lowlevel/shaders/Shader.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param shader_name The name of the shader to load.
 */
Shader::Shader(const std::string& shader_name):
    shader_name(shader_name),
    default_window_scale(1.0) {
}

/**
 * \brief Destructor.
 */
Shader::~Shader() {
}

/**
 * \brief Get the name of the shader, which is also the name of the related video mode.
 * \return The name of the shader.
 */
const std::string& Shader::get_name() {

  return shader_name;
}

/**
 * \brief Get the scale to apply on the quest size to get the final default size of the related video mode.
 * \return The window scale.
 */
double Shader::get_default_window_scale() {

  return default_window_scale;
}

/**
 * \brief Draws the quest surface on the screen in a shader-allowed context.
 * It will perform the render using the OpenGL API directly.
 * \param quest_surface the surface to render on the screen
 */
void Shader::render(const SurfacePtr& /* quest_surface */) const {

}

/**
 * \brief Load all files from the corresponding shader, depending on the driver and shader names.
 * Parse the Lua file and compile GLSL others.
 * \param shader_id The name of the shader to load.
 */
void Shader::load(const std::string& shader_id) {

  const std::string shader_file_name =
      "shaders/" + shader_id + ".dat";

  // Parse the lua file
  load_lua_file(shader_file_name);
}

/**
 * \brief Dummy method used to call the static lua callback for a specific shader implementation.
 * \param l The lua state.
 */
void Shader::register_callback(lua_State* /* l */) {
}

/**
 * \brief Load and parse the Lua file of the requested shader.
 * \param path The path to the lua file, relative to the data folder.
 */
void Shader::load_lua_file(const std::string& path) {

  lua_State* l = luaL_newstate();

  const std::string& buffer = QuestFiles::data_file_read(path);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), path.c_str());

  if (load_result != 0) {
    // Syntax error in the lua file.
    Debug::error(std::string("Failed to load ") + path + " : " + lua_tostring(l, -1));
    lua_pop(l, 1);
  }
  else {
    // Register the callback and send string parameters to the lua script.
    register_callback(l);

    if (lua_pcall(l, 0, 0, 0) != 0) {
      // Runtime error.
      Debug::error(std::string("Failed to parse ") + path + " : " + lua_tostring(l, -1));
      lua_pop(l, 1);
    }
  }

  lua_close(l);
}

}
