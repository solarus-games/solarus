/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_SHADER_H
#define SOLARUS_SHADER_H

#include "Debug.h"

#if SOLARUS_HAVE_OPENGL_OR_ES == 1
#  include "lua/LuaContext.h"
#  include <SDL.h>
#  include <SDL_opengl.h>
#endif


namespace solarus {

/**
 * \brief Represents a GLSL shader.
 *
 * This class represents a shader which can be applied before render to the window.
 * This class basically encapsulates a GLSL vertex and fragment shader.
 */
class Shader {

  public:

    static bool initialize();
    static void quit();

#if SOLARUS_HAVE_OPENGL_OR_ES == 1
    static GLenum get_texture_type();
    static void restore_default_shader_program();
    static void set_rendering_settings();
    static Shader* create(const std::string& shader_name);
  
    Shader(const std::string& shader_name);
    ~Shader();

    std::string get_name();
    double get_logical_scale();
  
    void apply();

  private:

    static PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
    static PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
    static PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
    static PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
    static PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
    static PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
    static PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
    static PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
    static PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
    static PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
    static PFNGLUNIFORM1IARBPROC glUniform1iARB;
    static PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
    static PFNGLGETHANDLEARBPROC glGetHandleARB;
  
    static void compile_shader(GLhandleARB& shader, const char* source);
    static int l_shader(lua_State* l);
    static const std::string get_sampler_type();
  
    void load(const std::string& shader_name);
    void load_lua_file(const std::string& path);
  
    static SDL_GLContext gl_context;             /**< The OpenGL context. */
    static GLhandleARB default_shader_program;   /**< Default shader program to restore once a shaded render is done. */
    static GLenum gl_texture_type;               /**< The type of GL texture used by SDL. */
    static std::string shading_language_version; /**< The version of the shading language. */
    static Shader* loading_shader;               /**< Shader to fill by l_shader(). TODO : remove if possible. */
  
    GLhandleARB program;                         /**< The program which bind the vertex and fragment shader. */
    GLhandleARB vertex_shader;                   /**< The vertex shader. */
    GLhandleARB fragment_shader;                 /**< The fragment shader. */
    std::string shader_name;                     /**< The name of the shader. */
    double logical_scale;                        /**< Logical scale of the renderer when the shader is active,
                                                   compared to the normal quest size. */
#endif
};

}

#endif

