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
#include "solarus/core/QuestFiles.h"
#include "solarus/core/System.h"
#include "solarus/graphics/Shader.h"
#include "solarus/graphics/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"

namespace Solarus {

/**
 * \brief Constructor.
 * \param shader_id The id of the shader to load (filename without extension).
 */
Shader::Shader(const std::string& shader_id):
    shader_id(shader_id),
    data(),
    valid(true),
    error() {
}

/**
 * \brief Destructor.
 */
Shader::~Shader() {
}

/**
 * \brief Returns whether this shader is valid.
 * \return \c true if the shader was successfully loaded.
 */
bool Shader::is_valid() const {
  return valid;
}

/**
 * \brief Sets whether this shader is valid.
 * \param valid \c true to indicate that the shader was successfully loaded.
 */
void Shader::set_valid(bool valid) {
  this->valid = valid;
}

/**
 * \brief Returns the error message of the last operation if any.
 * \return The error message or an empty string.
 */
std::string Shader::get_error() const {
  return error;
}

/**
 * \brief Sets the error message of the last operation.
 *
 * There should be an error message when \c is_valid() returns \c false.
 *
 * \param error The error message or an empty string.
 */
void Shader::set_error(const std::string& error) {
  this->error = error;
}

/**
 * \brief Returns the id of the shader.
 * \return The id of the shader.
 */
const std::string& Shader::get_id() const {

  return shader_id;
}

/**
 * \brief Returns the shader information loaded from the data file.
 * \return The shader data.
 */
const ShaderData& Shader::get_data() const {
  return data;
}

/**
 * @brief Returns the vertex source of the data or the default vertex source
 * @return the vertex source
 */
std::string Shader::get_vertex_source() const {
  const auto& ds = get_data().get_vertex_source();
  if(ds != "") {
    return ds;
  }
  return default_vertex_source();
}

/**
 * @brief Returns the fragment source of the data or the default fragment source
 * @return The fragment source
 */
std::string Shader::get_fragment_source() const {
  const auto& ds = get_data().get_fragment_source();
  if(ds != "") {
    return ds;
  }
  return default_fragment_source();
}

/**
 * \brief Sets the shader information.
 * \param data The shader data.
 */
void Shader::set_data(const ShaderData& data) {
  this->data = data;
}

/**
 * \fn Shader::set_uniform_1b
 * \brief Uploads a boolean uniform value to this shader program.
 *
 * Does nothing if there is no such uniform in the shader program.
 *
 * \param uniform_name Name of the uniform to set.
 * \param value The boolean value to set.
 */
void Shader::set_uniform_1b(const std::string&, bool) {
  // TODO make pure virtual
}

/**
 * \fn Shader::set_uniform_1i
 * \brief Uploads an integer uniform value to this shader program.
 *
 * Does nothing if there is no such uniform in the shader program.
 *
 * \param uniform_name Name of the uniform to set.
 * \param value The integer value to set.
 */
void Shader::set_uniform_1i(const std::string&, int) {
  // TODO make pure virtual
}

/**
 * \fn Shader::set_uniform_1f
 * \brief Uploads a float uniform value to this shader program.
 *
 * Does nothing if there is no such uniform in the shader program.
 *
 * \param uniform_name Name of the uniform to set.
 * \param value The float value to set.
 */
void Shader::set_uniform_1f(const std::string&, float) {
  // TODO make pure virtual
}

/**
 * \fn Shader::set_uniform_2f
 * \brief Uploads a vec2 uniform value to this shader program.
 *
 * Does nothing if there is no such uniform in the shader program.
 *
 * \param uniform_name Name of the uniform to set.
 * \param value_1 The first float value to set.
 * \param value_2 The second float value to set.
 */
void Shader::set_uniform_2f(const std::string&, float, float) {
  // TODO make pure virtual
}

/**
 * \fn Shader::set_uniform_3f
 * \brief Uploads a vec3 uniform value to this shader program.
 *
 * Does nothing if there is no such uniform in the shader program.
 *
 * \param uniform_name Name of the uniform to set.
 * \param value_1 The first float value to set.
 * \param value_2 The third float value to set.
 * \param value_3 The second float value to set.
 */
void Shader::set_uniform_3f(const std::string&, float, float, float) {
  // TODO make pure virtual
}

/**
 * \fn Shader::set_uniform_4f
 * \brief Uploads a vec4 uniform value to this shader program.
 *
 * Does nothing if there is no such uniform in the shader program.
 *
 * \param uniform_name Name of the uniform to set.
 * \param value_1 The first float value to set.
 * \param value_2 The second float value to set.
 * \param value_3 The third float value to set.
 * \param value_4 The fourth float value to set.
 */
void Shader::set_uniform_4f(const std::string&, float, float, float, float) {
  // TODO make pure virtual
}

/**
 * \fn Shader::set_uniform_texture
 * \brief Uploads a 2D texture uniform value to this shader program.
 *
 * Does nothing if there is no such uniform in the shader program.
 *
 * \param uniform_name Name of the uniform to set.
 * \param value The 2D texture value value to set.
 * \return \c true in case of success.
 */
bool Shader::set_uniform_texture(const std::string&, const SurfacePtr&) {
  // TODO make pure virtual
  return false;
}

/**
 * \fn Shader::render
 * \brief Draws the quest surface on the screen in a shader-allowed context.
 * It will perform the render using the OpenGL API directly.
 * \param quest_surface the surface to render on the screen
 */
void Shader::render(const SurfacePtr&) {
  // TODO make pure virtual
}


/**
 * \fn Shader::load
 * \brief Loads this shader.
 *
 * Parses the shader data file and compiles GLSL shaders.
 */
void Shader::load() {
  // TODO make pure virtual
}

/**
 * \brief Returns the name identifying this type in Lua.
 * \return The name identifying this type in Lua.
 */
const std::string& Shader::get_lua_type_name() const {
  return LuaContext::shader_module_name;
}

}
