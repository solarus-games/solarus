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
#include "solarus/lowlevel/shaders/ShaderData.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lua/LuaTools.h"
#include <ostream>

namespace Solarus {

/**
 * \brief Creates an empty shader data.
 */
ShaderData::ShaderData() :
    vertex_source(),
    fragment_source() {

}

/**
 * \brief Returns the vertex shader source.
 * \return The source of the vertex shader or an empty string.
 */
const std::string& ShaderData::get_vertex_source() const {
  return vertex_source;
}

/**
 * \brief Sets the vertex shader source.
 * \param vertex_source The source of the vertex shader or an empty string.
 */
void ShaderData::set_vertex_source(const std::string& vertex_source) {
  this->vertex_source = vertex_source;
}

/**
 * \brief Returns the fragment shader source.
 * \return The source of the fragment shader or an empty string.
 */
const std::string& ShaderData::get_fragment_source() const {
  return fragment_source;
}

/**
 * \brief Sets the fragment shader source.
 * \param fragment_source The source of the fragment shader or an empty string.
 */
void ShaderData::set_fragment_source(const std::string& fragment_source) {
  this->fragment_source = fragment_source;
}

/**
 * \copydoc LuaData::import_from_lua
 */
bool ShaderData::import_from_lua(lua_State* l) {

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, "shader");
  lua_register(l, "shader", l_shader);
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

  out << "shader{\n";

  if (!vertex_source.empty()) {
    export_multiline_string("vertex_source", vertex_source, out);
  }
  out << '\n';
  export_multiline_string("fragment_source", fragment_source, out);

  return true;
}

/**
 * \brief Function called by the Lua data file to define a shader.
 *
 * - Argument 1 (table): properties of the shader.
 *
 * \param l the Lua context that is calling this function
 * \return Number of values to return to Lua.
 */
int ShaderData::l_shader(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    lua_getfield(l, LUA_REGISTRYINDEX, "shader");
    ShaderData* shader_data = static_cast<ShaderData*>(
        lua_touserdata(l, -1));
    lua_pop(l, 1);

    // Retrieve the shader properties from the table parameter.
    LuaTools::check_type(l, 1, LUA_TTABLE);

    const std::string vertex_source =
        LuaTools::opt_string_field(l, 1, "vertex_source",
            "void main(){\n\
             gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n\
             gl_TexCoord[0] = gl_MultiTexCoord0;\n\
           }");
    const std::string fragment_source =
        LuaTools::check_string_field(l, 1, "fragment_source");

    shader_data->set_vertex_source(vertex_source);
    shader_data->set_fragment_source(fragment_source);

    return 0;
  });
}

}
