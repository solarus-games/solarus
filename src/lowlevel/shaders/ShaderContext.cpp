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
#include "solarus/lowlevel/shaders/GL_ARBShader.h"
#include "solarus/lowlevel/shaders/GL_2DShader.h"
#include "solarus/lowlevel/Logger.h"
#include "solarus/lowlevel/Video.h"


namespace Solarus {

namespace {

SDL_GLContext gl_context = nullptr;
std::string opengl_version;
std::string shading_language_version;
std::string opengl_vendor;
std::string opengl_renderer;
bool is_universal_shader_supported = false;

}  // Anonymous namespace.


/**
 * \brief Initializes the shader system.
 * \return \c true if any shader system is supported.
 */
bool ShaderContext::initialize() {

  opengl_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  shading_language_version = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
  opengl_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  opengl_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));

  Logger::info(std::string("OpenGL: ") + opengl_version);
  Logger::info(std::string("OpenGL vendor: ") + opengl_vendor);
  Logger::info(std::string("OpenGL renderer: ") + opengl_renderer);
  Logger::info(std::string("OpenGL shading language: ") + shading_language_version);

  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (!(gl_context = SDL_GL_CreateContext(Video::get_window()))) {
    Debug::warning("Unable to create OpenGL context : " + std::string(SDL_GetError()));
    return false;
  }

  // Setting some parameters
  glEnable(GL_DEPTH_TEST); // The type of depth test to do.
  glDepthFunc(GL_LESS); // Enables depth testing.

  // Use late swap tearing, or try to use the classic swap interval (aka VSync) if not supported.
  if (SDL_GL_SetSwapInterval(-1) == -1) {
    SDL_GL_SetSwapInterval(1);
  }

  // Try to initialize a gl shader system, in order from the earlier to the older.
  is_universal_shader_supported = GL_2DShader::initialize();
  if (is_universal_shader_supported) {
    return true;
  }

  return GL_ARBShader::initialize();
}

/**
 * \brief Free shader-related context.
 */
void ShaderContext::quit() {

  if (gl_context) {
    SDL_GL_DeleteContext(gl_context);
  }
}

/**
 * \brief Returns the OpenGL version name.
 * \return The OpenGL version name.
 */
const std::string& ShaderContext::get_opengl_version() {
  return opengl_version;
}

/**
 * \brief Returns the shading language version.
 * \return The shading language version.
 */
const std::string& ShaderContext::get_shading_language_version() {
  return shading_language_version;
}

/**
 * \brief Construct a shader from a name.
 * \param shader_id The id of the shader to load.
 * \return The created shader.
 */
ShaderPtr ShaderContext::create_shader(const std::string& shader_id) {

  ShaderPtr shader = nullptr;

  if (is_universal_shader_supported) {
    shader = std::make_shared<GL_2DShader>(shader_id);
  }
  else {
    shader = std::make_shared<GL_ARBShader>(shader_id);
  }

  return shader;
}

}
