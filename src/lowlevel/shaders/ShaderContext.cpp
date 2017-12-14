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
 * \return The created shader.
 */
ShaderPtr ShaderContext::create_shader(const std::string& shader_id) {

  ShaderPtr shader = nullptr;

  if (true) {  // TODO
    shader = std::make_shared<GL_ARBShader>(shader_id);
  }
  else {
    shader = std::make_shared<GL_2DShader>(shader_id);
  }

  return shader;
}

}
