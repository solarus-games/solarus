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
#ifndef SOLARUS_GL_ARB_SHADER_H
#define SOLARUS_GL_ARB_SHADER_H

#include "Common.h"

#if SOLARUS_HAVE_OPENGL == 1

#include "lowlevel/shaders/Shader.h"
#include <SDL.h>
#include <SDL_opengl.h>


namespace Solarus {

/**
 * \brief Represents a GLSL shader for use with GL rectangle ARB sampler.
 *
 * This class basically encapsulates a GLSL vertex and fragment shader.
 */
class GL_ARBShader : public Shader {

  public:

    static bool initialize();

    GL_ARBShader(const std::string& shader_name);
    ~GL_ARBShader();

  private:

    static void compile_shader(GLhandleARB& shader, const char* source);
    static void set_rendering_settings();
    static int l_shader(lua_State* l);

    void register_callback(lua_State* l);
    void render(const SurfacePtr& quest_surface);

    GLhandleARB program;                         /**< The program which bind the vertex and fragment shader. */
    GLhandleARB vertex_shader;                   /**< The vertex shader. */
    GLhandleARB fragment_shader;                 /**< The fragment shader. */
};

}

#endif // SOLARUS_HAVE_OPENGL

#endif
