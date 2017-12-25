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
#include "solarus/lowlevel/shaders/GLContext.h"
#include "solarus/lowlevel/shaders/Shader.h"
#include "solarus/lowlevel/shaders/ShaderContext.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include "solarus/CurrentQuest.h"
#include <iostream>

namespace Solarus {

/**
 * Name of the Lua table representing the shader module.
 */
const std::string LuaContext::shader_module_name = "sol.shader";

/**
 * \brief Initializes the shader features provided to Lua.
 */
void LuaContext::register_shader_module() {

  if (!CurrentQuest::is_format_at_least({ 1, 6 })) {
    return;
  }

  // Functions of sol.shader.
  const std::vector<luaL_Reg> functions = {
      { "create", shader_api_create },
      { "get_opengl_version", shader_api_get_opengl_version },
      { "get_shading_language_version", shader_api_get_shading_language_version },
  };

  // Methods of the shader type.
  const std::vector<luaL_Reg> methods = {
      { "get_id", shader_api_get_id },
      { "get_vertex_file", shader_api_get_vertex_file },
      { "get_fragment_file", shader_api_get_fragment_file },
      { "set_uniform", shader_api_set_uniform },
  };

  const std::vector<luaL_Reg> metamethods = {
      { "__gc", userdata_meta_gc }
  };

  register_type(shader_module_name, functions, methods, metamethods);
}

/**
 * \brief Returns whether a value is a userdata of type shader.
 * \param l A Lua context.
 * \param index An index in the stack.
 * \return \c true if the value at this index is a shader.
 */
bool LuaContext::is_shader(lua_State* l, int index) {
  return is_userdata(l, index, shader_module_name);
}

/**
 * \brief Checks that the userdata at the specified index of the stack is a
 * shader and returns it.
 * \param l a Lua context
 * \param index an index in the stack
 * \return The shader.
 */
ShaderPtr LuaContext::check_shader(lua_State* l, int index) {
  return std::static_pointer_cast<Shader>(
      check_userdata(l, index, shader_module_name)
  );
}

/**
 * \brief Pushes a shader userdata onto the stack.
 * \param l A Lua context.
 * \param shader A shader.
 */
void LuaContext::push_shader(lua_State* l, Shader& shader) {
  push_userdata(l, shader);
}

/**
 * \brief Implementation of sol.shader.create().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::shader_api_create(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    const std::string& shader_id = LuaTools::check_string(l, 1);

    ShaderPtr shader = ShaderContext::create_shader(shader_id);

    if (!shader->is_valid()) {
      LuaTools::error(l, "Failed to create shader '" + shader_id + "': " + shader->get_error());
    }

    push_shader(l, *shader);
    return 1;
  });
}

/**
 * \brief Implementation of sol.shader.get_opengl_version().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::shader_api_get_opengl_version(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    const std::string& opengl_version = GLContext::get_opengl_version();

    push_string(l, opengl_version);
    return 1;
  });
}

/**
 * \brief Implementation of sol.shader.get_shading_language_version().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::shader_api_get_shading_language_version(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    const std::string& shading_language_version = GLContext::get_shading_language_version();

    push_string(l, shading_language_version);
    return 1;
  });
}

/**
 * \brief Implementation of shader:get_id().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::shader_api_get_id(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    const Shader& shader = *check_shader(l, 1);

    push_string(l, shader.get_id());
    return 1;
  });
}

/**
 * \brief Implementation of shader:get_vertex_file().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::shader_api_get_vertex_file(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    const Shader& shader = *check_shader(l, 1);

    push_string(l, shader.get_data().get_vertex_source());
    return 1;
  });
}

/**
 * \brief Implementation of shader:get_fragment_file().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::shader_api_get_fragment_file(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    const Shader& shader = *check_shader(l, 1);

    push_string(l, shader.get_data().get_fragment_source());
    return 1;
  });
}

/**
 * \brief Implementation of shader:set_uniform().
 * \param l The Lua context that is calling this function.
 * \return Number of values to return to Lua.
 */
int LuaContext::shader_api_set_uniform(lua_State* l) {

  return LuaTools::exception_boundary_handle(l, [&] {

    Shader& shader = *check_shader(l, 1);
    const std::string& uniform_name = LuaTools::check_string(l, 2);

    if (lua_isboolean(l, 3)) {
      // Boolean.
      const bool value = lua_toboolean(l, 3);
      shader.set_uniform_1b(uniform_name, value);
    }
    else if (lua_isnumber(l, 3)) {
      // Number.
      const float value = static_cast<float>(lua_tonumber(l, 3));
      shader.set_uniform_1f(uniform_name, value);
    }
    else if (lua_istable(l, 3)) {
      // Table of 2, 3 or 4 numbers.
      const char* table_error_message = "Table parameter should have 2, 3 or 4 number elements";

      lua_rawgeti(l, 3, 1);
      if (!lua_isnumber(l, -1)) {
        LuaTools::arg_error(l, 3, table_error_message);
      }
      const float value_1 = static_cast<float>(LuaTools::check_number(l, -1));

      lua_rawgeti(l, 3, 2);
      if (!lua_isnumber(l, -1)) {
        LuaTools::arg_error(l, 3, table_error_message);
      }
      const float value_2 = static_cast<float>(LuaTools::check_number(l, -1));

      lua_rawgeti(l, 3, 3);
      if (lua_isnil(l, -1)) {
        // 2 numbers.
        shader.set_uniform_2f(uniform_name, value_1, value_2);
        return 0;
      }

      if (!lua_isnumber(l, -1)) {
        LuaTools::arg_error(l, 3, table_error_message);
      }
      const float value_3 = static_cast<float>(LuaTools::check_number(l, -1));

      lua_rawgeti(l, 3, 4);
      if (lua_isnil(l, -1)) {
        // 3 numbers.
        shader.set_uniform_3f(uniform_name, value_1, value_2, value_3);
        return 0;
      }

      if (!lua_isnumber(l, -1)) {
        // 4 numbers.
        LuaTools::arg_error(l, 3, table_error_message);
      }
      const float value_4 = static_cast<float>(LuaTools::check_number(l, -1));
      shader.set_uniform_4f(uniform_name, value_1, value_2, value_3, value_4);
    }
    else if (is_surface(l, 3)) {
      // Surface.
      const SurfacePtr& value = check_surface(l, 3);
      bool success = shader.set_uniform_texture(uniform_name, value);
      if (!success) {
        LuaTools::arg_error(l, 3, "Cannot use this surface in a shader");
      }
    }
    else {
      LuaTools::type_error(l, 3, "boolean, number, table or surface");
    }
    return 0;
  });
}

}

