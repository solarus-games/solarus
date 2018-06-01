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
#include "solarus/core/Debug.h"
#include "solarus/graphics/ShaderData.h"
#include "solarus/lua/LuaTools.h"
#include <ostream>

namespace Solarus {

/**
 * \brief Creates an empty shader data.
 */
ShaderData::ShaderData() :
    vertex_file(),
    fragment_file() {

}

/**
 * \brief Returns the vertex shader file name.
 * \return The file of the vertex shader or an empty string.
 */
const std::string& ShaderData::get_vertex_file() const {
  return vertex_file;
}

/**
 * \brief Sets the vertex shader file.
 * \param vertex_file The file of the vertex shader or an empty string.
 */
void ShaderData::set_vertex_file(const std::string& vertex_file) {
  this->vertex_file = vertex_file;
}

/**
 * \brief Returns the fragment shader file.
 * \return The file of the fragment shader or an empty string.
 */
const std::string& ShaderData::get_fragment_file() const {
  return fragment_file;
}

/**
 * \brief Sets the fragment shader file.
 * \param fragment_file The file of the fragment shader or an empty string.
 */
void ShaderData::set_fragment_file(const std::string& fragment_file) {
  this->fragment_file = fragment_file;
}

/**
 * \copydoc LuaData::import_from_lua
 */
bool ShaderData::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "shader");
  lua_register(l, "vertex_shader", l_vertex_shader);
  lua_register(l, "fragment_shader", l_fragment_shader);
  if (lua_pcall(l, 0, 0, 0) != 0) {
    Debug::error(std::string("Failed to load shader: ") + lua_tostring(l, -1));
    lua_pop(l, 1);
    return false;
  }

  return true;
}

/**
 * \copydoc LuaData::export_to_lua
 */
bool ShaderData::export_to_lua(std::ostream& out) const {

  if (!vertex_file.empty()) {
    out << "vertex_shader{\n";
    out << "  source_file = \"" << escape_string(vertex_file) << "\",\n";
    out << "}\n";
  }
  if (!fragment_file.empty()) {
    out << "fragment_shader{\n";
    out << "  source_file = \"" << escape_string(fragment_file) << "\",\n";
    out << "}\n";
  }

  return true;
}

/**
 * \brief Function called by the Lua data file to define a vertex shader.
 *
 * - Argument 1 (table): properties of the shader.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int ShaderData::l_vertex_shader(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    lua_getfield(l, LUA_REGISTRYINDEX, "shader");
    ShaderData* shader_data = static_cast<ShaderData*>(
        lua_touserdata(l, -1));
    lua_pop(l, 1);

    // Retrieve the shader properties from the table parameter.
    LuaTools::check_type(l, 1, LUA_TTABLE);

    const std::string vertex_file =
        LuaTools::opt_string_field(l, 1, "source_file", "");

    shader_data->set_vertex_file(vertex_file);

    return 0;
  });
}

/**
 * \brief Function called by the Lua data file to define a fragment shader.
 *
 * - Argument 1 (table): properties of the shader.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int ShaderData::l_fragment_shader(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    lua_getfield(l, LUA_REGISTRYINDEX, "shader");
    ShaderData* shader_data = static_cast<ShaderData*>(
        lua_touserdata(l, -1));
    lua_pop(l, 1);

    // Retrieve the shader properties from the table parameter.
    LuaTools::check_type(l, 1, LUA_TTABLE);

    const std::string fragment_file =
        LuaTools::check_string_field(l, 1, "source_file");

    shader_data->set_fragment_file(fragment_file);

    return 0;
  });
}

}
