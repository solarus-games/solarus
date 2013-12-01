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

#include "SDL.h"
#include "SDL_opengl.h"
#include "Debug.h"


/**
 * \brief Represents a GLSL shader.
 *
 * This class represents a shader which can be applied before render to the window.
 * This class basically encapsulates a GLSL vertex and fragment shader.
 */
class Shader {

  public:
    static void initialize();
    static SDL_bool compile_shader(GLhandleARB shader, const char* source);
  
    Shader(std::string filename);
    ~Shader();
  
    const std::string get_name();
    double get_logical_scale();
  
    void render_present_shaded(SDL_Renderer* renderer);

  private:
    static SDL_bool shaders_supported;
  
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

    GLhandleARB program;
    GLhandleARB vertex_shader;
    GLhandleARB fragment_shader;
    const char* vertex_source;
    const char* fragment_source;
  
    std::string name;                            /**< Name of the shader and of the video mode's suffixe associated. */
    double logical_scale;                        /**< Logical scale of the renderer when the shader is active,
                                                   compared to the normal quest size. */
};

#endif

