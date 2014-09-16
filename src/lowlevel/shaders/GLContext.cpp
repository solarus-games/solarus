/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/shaders/GLContext.h"

#if SOLARUS_HAVE_OPENGL == 1

#include "lowlevel/shaders/GL_ARBShader.h"
#include "lowlevel/shaders/GL_2DShader.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Video.h"
#include "lua/LuaContext.h"
#include "lua/LuaTools.h"


namespace solarus {

SDL_GLContext GLContext::gl_context = nullptr;


/**
 * \brief Initializes OpenGL and the shader system.
 * \return \c true if a GL shader system is supported.
 */
bool GLContext::initialize() {

  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (!(gl_context = SDL_GL_CreateContext(Video::get_window()))) {
    debug::warning("Unable to create OpenGL context : " + std::string(SDL_GetError()));
    return false;
  }

  // Setting some parameters
  glClearDepth(1.0); // Enables clearing of the depth buffer.
  glEnable(GL_DEPTH_TEST); // The type of depth test to do.
  glDepthFunc(GL_LESS); // Enables depth testing.
  glShadeModel(GL_SMOOTH); // Enables smooth color shading.

  // Use late swap tearing, or try to use the classic swap interval (aka VSync) if not supported.
  if (SDL_GL_SetSwapInterval(-1) == -1) {
    SDL_GL_SetSwapInterval(1);
  }

  // Get the shading language version.
  Shader::set_shading_language_version(
      reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));

  // Try to initialize a gl shader system, in order from the earlier to the older.
  return GL_ARBShader::initialize() || GL_2DShader::initialize();
}

/**
 * \brief Free GL context.
 */
void GLContext::quit() {

  if (gl_context) {
    SDL_GL_DeleteContext(gl_context);
  }
}

}

#endif // SOLARUS_HAVE_OPENGL
