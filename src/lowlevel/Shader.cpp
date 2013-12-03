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
#include "lowlevel/Shader.h"
#include "lowlevel/VideoManager.h"


PFNGLATTACHOBJECTARBPROC Shader::glAttachObjectARB;
PFNGLCOMPILESHADERARBPROC Shader::glCompileShaderARB;
PFNGLCREATEPROGRAMOBJECTARBPROC Shader::glCreateProgramObjectARB;
PFNGLCREATESHADEROBJECTARBPROC Shader::glCreateShaderObjectARB;
PFNGLDELETEOBJECTARBPROC Shader::glDeleteObjectARB;
PFNGLGETINFOLOGARBPROC Shader::glGetInfoLogARB;
PFNGLGETOBJECTPARAMETERIVARBPROC Shader::glGetObjectParameterivARB;
PFNGLGETUNIFORMLOCATIONARBPROC Shader::glGetUniformLocationARB;
PFNGLLINKPROGRAMARBPROC Shader::glLinkProgramARB;
PFNGLSHADERSOURCEARBPROC Shader::glShaderSourceARB;
PFNGLUNIFORM1IARBPROC Shader::glUniform1iARB;
PFNGLUSEPROGRAMOBJECTARBPROC Shader::glUseProgramObjectARB;

SDL_bool Shader::shaders_supported = SDL_FALSE;
GLint Shader::default_shader_program;

void Shader::initialize() {
  
  VideoManager* videomanager = VideoManager::get_instance();
  
  if (!SDL_GL_CreateContext(videomanager->get_window())) {
    Debug::die("Unable to create OpenGL context : " + std::string(SDL_GetError()));
  }

  GLdouble aspect;
  Rectangle quest_size = videomanager->get_quest_size();
  
  glViewport(0, 0, quest_size.get_width(), quest_size.get_height());
  glDepthFunc(GL_LESS);                        // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);                     // Enables Depth Testing
  glShadeModel(GL_SMOOTH);                     // Enables Smooth Color Shading
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();                            // Reset The Projection Matrix
  
  aspect = (GLdouble)quest_size.get_width() / quest_size.get_height();
  glOrtho(-3.0, 3.0, -3.0 / aspect, 3.0 / aspect, 0.0, 1.0);
  
  glMatrixMode(GL_MODELVIEW);
  
  // Check for shader support
  if (SDL_GL_ExtensionSupported("GL_ARB_shader_objects") &&
      SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") &&
      SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") &&
      SDL_GL_ExtensionSupported("GL_ARB_fragment_shader")) {
    glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC) SDL_GL_GetProcAddress("glAttachObjectARB");
    glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC) SDL_GL_GetProcAddress("glCompileShaderARB");
    glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glCreateProgramObjectARB");
    glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC) SDL_GL_GetProcAddress("glCreateShaderObjectARB");
    glDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC) SDL_GL_GetProcAddress("glDeleteObjectARB");
    glGetInfoLogARB = (PFNGLGETINFOLOGARBPROC) SDL_GL_GetProcAddress("glGetInfoLogARB");
    glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC) SDL_GL_GetProcAddress("glGetObjectParameterivARB");
    glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC) SDL_GL_GetProcAddress("glGetUniformLocationARB");
    glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC) SDL_GL_GetProcAddress("glLinkProgramARB");
    glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC) SDL_GL_GetProcAddress("glShaderSourceARB");
    glUniform1iARB = (PFNGLUNIFORM1IARBPROC) SDL_GL_GetProcAddress("glUniform1iARB");
    glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
    if (glAttachObjectARB &&
        glCompileShaderARB &&
        glCreateProgramObjectARB &&
        glCreateShaderObjectARB &&
        glDeleteObjectARB &&
        glGetInfoLogARB &&
        glGetObjectParameterivARB &&
        glGetUniformLocationARB &&
        glLinkProgramARB &&
        glShaderSourceARB &&
        glUniform1iARB &&
        glUseProgramObjectARB) {
      shaders_supported = SDL_TRUE;
    }
  }
  
  if(!shaders_supported) {
    SDL_SetHint(SDL_HINT_RENDER_OPENGL_SHADERS, "0");

    // TODO authorize shaded modes o not use shaders, or implement a software alternative.
    Debug::warning("OpenGL shaders not supported.");
  }
  
  // Get the deault shader program
  glGetIntegerv(GL_CURRENT_PROGRAM, &default_shader_program);

  // Initialize default and shaded video modes.
  videomanager->initialize_video_modes();
}

SDL_bool Shader::compile_shader(GLhandleARB shader, const char* source) {
  
  GLint status;
  
  glShaderSourceARB(shader, 1, &source, NULL);
  glCompileShaderARB(shader);
  glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
  if (status == 0) {
    GLint length;
    char *info;
    
    glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    info = SDL_stack_alloc(char, length+1);
    glGetInfoLogARB(shader, length, NULL, info);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to compile shader:\n%s\n%s", source, info);
    SDL_stack_free(info);
    
    return SDL_FALSE;
  } else {
    return SDL_TRUE;
  }
}

Shader::Shader(std::string filename) :
  logical_scale(1.0),
  program(0),
  vertex_shader(0),
  fragment_shader(0),
  name(filename) {
    
  if (shaders_supported) {
    
    const int num_tmus_bound = 4;
    int i;
    GLint location;
    
    glGetError();
    
    // Get the vertex and fragment shader sources.
    //TODO remove color shader and get from file.
    vertex_source = "varying vec4 v_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
    "    v_color = gl_Color;\n"
      "}";
    /* fragment shader */
    fragment_source = "varying vec4 v_color;\n"
    "\n"
    "void main()\n"
    "{\n"
    "    gl_FragColor = v_color;\n"
    "}";
    
    // Create one program object to rule them all
    program = glCreateProgramObjectARB();
    
    // Create the vertex shader
    vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    if (!compile_shader(vertex_shader, vertex_source)) {
      Debug::die("Cannot compile the vertex shader for : " + filename);
    }
    
    // Create the fragment shader
    fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    if (!compile_shader(fragment_shader, fragment_source)) {
      Debug::die("Cannot compile the fragment shader for : " + filename);
    }
    
    // ... and in the darkness bind them
    glAttachObjectARB(program, vertex_shader);
    glAttachObjectARB(program, fragment_shader);
    glLinkProgramARB(program);
    
    // Set up some uniform variables
    glUseProgramObjectARB(program);
    for (i = 0; i < num_tmus_bound; ++i) {
      char tex_name[5];
      SDL_snprintf(tex_name, SDL_arraysize(tex_name), "tex%d", i);
      location = glGetUniformLocationARB(program, tex_name);
      if (location >= 0) {
        glUniform1iARB(location, i);
      }
    }
    glUseProgramObjectARB(0);
    
    Debug::check_assertion(glGetError() == GL_NO_ERROR, "Cannot compile the shader : " + filename);
  }
}

Shader::~Shader()
{
  if (shaders_supported) {
    glDeleteObjectARB(vertex_shader);
    glDeleteObjectARB(fragment_shader);
    glDeleteObjectARB(program);
  }
}

const std::string Shader::get_name() {
  return name;
}

double Shader::get_logical_scale() {
  return logical_scale;
}

void Shader::render_present_shaded(SDL_Renderer* renderer)
{
  if (shaders_supported) {
    // Apply our shader
    glUseProgramObjectARB(program);
    SDL_RenderPresent(renderer);
    
    // Restore the default shader
    glUseProgram(default_shader_program);
  }
  else {
    SDL_RenderPresent(renderer);
  }
}