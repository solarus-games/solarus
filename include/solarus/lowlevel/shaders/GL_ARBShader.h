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
#ifndef SOLARUS_GL_ARB_SHADER_H
#define SOLARUS_GL_ARB_SHADER_H

#include "solarus/Common.h"

#include "solarus/lowlevel/shaders/Shader.h"
#include <SDL.h>
#include <string>

#ifdef SOLARUS_HAVE_OPENGL
#  include <SDL_opengl.h>
#endif


namespace Solarus {

/**
 * \brief Represents a GLSL shader and displays it using the GL ARB extension way.
 *
 * This class basically encapsulates a GLSL vertex and fragment shader.
 */
class GL_ARBShader : public Shader {

  public:

#ifdef SOLARUS_HAVE_OPENGL
    static bool initialize();

    explicit GL_ARBShader(const std::string& shader_id);
    ~GL_ARBShader();

  protected:

    void load() override;

  private:

    GLhandleARB create_shader(uint type, const char* source);
    static void set_rendering_settings();

    void render(const SurfacePtr& quest_surface) const override;

    GLhandleARB program;                         /**< The program which bind the vertex and fragment shader. */
    GLhandleARB vertex_shader;                   /**< The vertex shader. */
    GLhandleARB fragment_shader;                 /**< The fragment shader. */
#else

  static bool initialize() { return false; }
  explicit GL_ARBShader(const std::string& shader_id): Shader(shader_id)  {}
#endif
};

}

#endif
