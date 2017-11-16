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
#include "solarus/lowlevel/shaders/ShaderContext.h"
#include "solarus/lowlevel/shaders/GLContext.h"
#include "solarus/lowlevel/shaders/GL_ARBShader.h"
#include "solarus/lowlevel/shaders/GL_2DShader.h"


namespace Solarus {

bool ShaderContext::shader_supported = false;

/**
 * \brief Initializes the shader system.
 * \return \c true if any shader system is supported.
 */
bool ShaderContext::initialize() {

  // TODO try to initialize DX shaders before GL ones.
  shader_supported = GLContext::initialize();

  return shader_supported;
}

/**
 * \brief Free shader-related context.
 */
void ShaderContext::quit() {

  if (shader_supported) {
    GLContext::quit();
  }
}

/**
 * \brief Construct a shader from a name.
 * \param shader_id The id of the shader to load.
 * \return The created shader, or nullptr if the shader fails to compile.
 */
std::unique_ptr<Shader> ShaderContext::create_shader(const std::string& shader_id) {

  std::unique_ptr<Shader> shader = nullptr;
  bool error = false;

  if (Shader::get_sampler_type() == "sampler2DRect") {
    shader = std::unique_ptr<Shader>(new GL_ARBShader(shader_id));
  }
  else {
    shader = std::unique_ptr<Shader>(new GL_2DShader(shader_id));
  }

  if (glGetError() != GL_NO_ERROR) {
    error = true;
  }

  if (error) {
    Debug::error("Can't compile shader '" + shader_id + "'");
    shader = nullptr;
  }

  return shader;
}

}
