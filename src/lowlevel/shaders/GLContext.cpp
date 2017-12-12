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
#include "solarus/lowlevel/shaders/GL_ARBShader.h"
#include "solarus/lowlevel/shaders/GL_2DShader.h"
#include "solarus/lowlevel/Logger.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"
#include <string>


namespace Solarus {

namespace GLContext {

namespace {

SDL_GLContext gl_context = nullptr;
std::string opengl_version;
std::string shading_language_version;
std::string opengl_vendor;
std::string opengl_renderer;

}  // Anonymous namespace.

/**
 * \brief Initializes OpenGL and the shader system.
 * \return \c true if a GL shader system is supported.
 */
bool initialize() {

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
  return GL_ARBShader::initialize() || GL_2DShader::initialize();
}

/**
 * \brief Frees the GL context.
 */
void quit() {

  if (gl_context) {
    SDL_GL_DeleteContext(gl_context);
  }
}

/**
 * \brief Returns the OpenGL version name.
 * \return The OpenGL version name.
 */
const std::string& get_opengl_version() {
  return opengl_version;
}

/**
 * \brief Returns the shading language version.
 * \return The shading language version.
 */
const std::string& get_shading_language_version() {
  return shading_language_version;
}

}  // namespace GLContext

}  // namespace Solarus
