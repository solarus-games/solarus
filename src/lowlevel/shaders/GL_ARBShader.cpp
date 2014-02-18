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
#include "lowlevel/shaders/GL_ARBShader.h"
#include "lowlevel/Surface.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Video.h"

#if SOLARUS_HAVE_OPENGL == 1


namespace solarus {

PFNGLATTACHOBJECTARBPROC GL_ARBShader::glAttachObjectARB;
PFNGLCOMPILESHADERARBPROC GL_ARBShader::glCompileShaderARB;
PFNGLCREATEPROGRAMOBJECTARBPROC GL_ARBShader::glCreateProgramObjectARB;
PFNGLCREATESHADEROBJECTARBPROC GL_ARBShader::glCreateShaderObjectARB;
PFNGLDELETEOBJECTARBPROC GL_ARBShader::glDeleteObjectARB;
PFNGLGETINFOLOGARBPROC GL_ARBShader::glGetInfoLogARB;
PFNGLGETOBJECTPARAMETERIVARBPROC GL_ARBShader::glGetObjectParameterivARB;
PFNGLGETUNIFORMLOCATIONARBPROC GL_ARBShader::glGetUniformLocationARB;
PFNGLLINKPROGRAMARBPROC GL_ARBShader::glLinkProgramARB;
PFNGLSHADERSOURCEARBPROC GL_ARBShader::glShaderSourceARB;
PFNGLUNIFORM1IARBPROC GL_ARBShader::glUniform1iARB;
PFNGLUNIFORM2IARBPROC GL_ARBShader::glUniform2iARB;
PFNGLUSEPROGRAMOBJECTARBPROC GL_ARBShader::glUseProgramObjectARB;
PFNGLGETHANDLEARBPROC GL_ARBShader::glGetHandleARB;

GLhandleARB GL_ARBShader::default_shader_program = 0;
GL_ARBShader* GL_ARBShader::loading_shader = NULL;


/**
 * \brief Initializes the GL ARB shader system.
 * \return \c true if GL ARB shaders are supported.
 */
bool GL_ARBShader::initialize() {

  // Check for shader support
  if ((SDL_GL_ExtensionSupported("GL_ARB_texture_rectangle") || // WORKAROUND : this is the way SDL check for the GL_ARB support
      SDL_GL_ExtensionSupported("GL_EXT_texture_rectangle")) && // but it may change in the future or be deactivate with render target.
      SDL_GL_ExtensionSupported("GL_ARB_shader_objects") &&
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
    glUniform2iARB = (PFNGLUNIFORM2IARBPROC) SDL_GL_GetProcAddress("glUniform2iARB");
    glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC) SDL_GL_GetProcAddress("glUseProgramObjectARB");
    glGetHandleARB = (PFNGLGETHANDLEARBPROC) SDL_GL_GetProcAddress("glGetHandleARB");
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
        glUniform2iARB &&
        glUseProgramObjectARB &&
        glGetHandleARB) {
        
      // Get ARB default configuration.
      default_shader_program = glGetHandleARB(GL_CURRENT_PROGRAM);
      sampler_type = "sampler2DRect";
        
      return true;
    }
  }
    
  return false;
}
  
/**
 * \brief Constructor.
 * \param shader_name The name of the shader to load.
 */
GL_ARBShader::GL_ARBShader(const std::string& shader_name): Shader(shader_name),
    program(0),
    vertex_shader(0),
    fragment_shader(0) {
    
  glGetError();
    
  // Load the shader.
  load(shader_name);
  
  // Set up the sampler, quest size and window size as uniform variables.
  const Rectangle& quest_size = Video::get_quest_size();
  glUseProgramObjectARB(program);
  GLint location = glGetUniformLocationARB(program, std::string("solarus_sampler").c_str());
  if (location >= 0) {
    glUniform1iARB(location, 0);
  }
      
  location = glGetUniformLocationARB(program, std::string("solarus_input_size").c_str());
  if (location >= 0) {
    glUniform2iARB(location, quest_size.get_width(), quest_size.get_height());
  }

  location = glGetUniformLocationARB(program, std::string("solarus_output_size").c_str());
  if (location >= 0) {
    glUniform2iARB(location,
        static_cast<double>(quest_size.get_width()) * default_window_scale,
        static_cast<double>(quest_size.get_height()) * default_window_scale);
  }
  glUseProgramObjectARB(default_shader_program);
}
  
/**
 * \brief Destructor.
 */
GL_ARBShader::~GL_ARBShader() {

  glDeleteObjectARB(vertex_shader);
  glDeleteObjectARB(fragment_shader);
  glDeleteObjectARB(program);
}
  
/**
 * \brief Compile a shader from source.
 * \param shader Reference to the shader to fill and compile.
 * \param source Sources to compile.
 */
void GL_ARBShader::compile_shader(GLhandleARB& shader, const char* source) {

  GLint status;
    
  glShaderSourceARB(shader, 1, &source, NULL);
  glCompileShaderARB(shader);
  glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
  if (status == 0) {
    GLint length;
    char* info;
      
    glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    info = SDL_stack_alloc(char, length+1);
    glGetInfoLogARB(shader, length, NULL, info);
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to compile shader:\n%s\n%s", source, info);
    SDL_stack_free(info);
  }
}

/**
 * \brief Set up OpenGL rendering parameter.
 * This basically reset the projection matrix.
 */
void GL_ARBShader::set_rendering_settings() {
  
  const Rectangle& quest_size = Video::get_quest_size();
  static const GLdouble aspect = GLdouble(quest_size.get_width() / quest_size.get_height());
    
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, 0.0, 1.0);
    
  glMatrixMode(GL_MODELVIEW);
}
  
/**
 * \brief Callback when parsing the lua file. Fill the loading shader with the result.
 * \param l The lua state.
 */
int GL_ARBShader::l_shader(lua_State* l) {

  if (loading_shader != NULL) {

    GLhandleARB& program = loading_shader->program,
        vertex_shader = loading_shader->vertex_shader,
        fragment_shader = loading_shader->fragment_shader;

    // Retrieve the shader properties from the table parameter.
    luaL_checktype(l, 1, LUA_TTABLE);

    const double& default_window_scale =
        LuaTools::opt_number_field(l, 1, "default_window_scale", 1.0);
    const std::string shader_name =
        LuaTools::opt_string_field(l, 1, "name", loading_shader->shader_name);
    const std::string vertex_source =
        LuaTools::check_string_field(l, 1, "vertex_source");
    const std::string fragment_source =
        LuaTools::check_string_field(l, 1, "fragment_source");

    loading_shader->default_window_scale = default_window_scale;
    loading_shader->shader_name = shader_name;

    // Create the vertex and fragment shaders.
    vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
    compile_shader(vertex_shader, vertex_source.c_str());

    fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
    compile_shader(fragment_shader, fragment_source.c_str());

    // Create one program object to rule them all ...
    program = glCreateProgramObjectARB();

    // ... and in the darkness bind them
    glAttachObjectARB(program, vertex_shader);
    glAttachObjectARB(program, fragment_shader);
    glLinkProgramARB(program);

    loading_shader = NULL;
  }

  return 0;
}
  
/**
 * \brief Dummy method used to call the static lua callback for a specific shader implementation.
 * \param l The lua state.
 */
void GL_ARBShader::register_callback(lua_State* l) {
  
  loading_shader = this;
  lua_register(l, "shader", l_shader);
}
  
/**
 * \brief Update the uniform variable corresponding to the output size.
 */
void GL_ARBShader::resize_output(int width, int height) {

  glUseProgramObjectARB(program);
  GLint location = glGetUniformLocationARB(program, std::string("solarus_output_size").c_str());
  if (location >= 0) {
    glUniform2iARB(location, width, height);
  }
  glUseProgramObjectARB(default_shader_program);
}
  
/**
 * \brief Draws the quest surface on the screen in a shader-allowed context.
 * It will perform the render using the OpenGL API directly.
 * \param quest_surface the surface to render on the screen
 */
void GL_ARBShader::render(Surface& quest_surface) {
    
  float rendering_width, rendering_height;
  SDL_Renderer* renderer = Video::get_renderer();
  SDL_Window* window = Video::get_window();
  SDL_Texture* render_target = Video::get_render_target();
    
  // Clear the window
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
    
  // Clear the render target
  SDL_SetRenderTarget(renderer, render_target);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderSetClipRect(renderer, NULL);
  SDL_RenderClear(renderer);
    
  // Draw on the render target.
  quest_surface.render(renderer);
    
  // Render on the window using OpenGL, to apply a shader if we have to.
  SDL_SetRenderTarget(renderer, NULL);
  set_rendering_settings();
    
  glEnable(GL_TEXTURE_RECTANGLE_ARB);
  SDL_GL_BindTexture(render_target, &rendering_width, &rendering_height);
  glUseProgramObjectARB(program);
    
  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-1.0f, 1.0f, 0.0f); // Top left
  glTexCoord2f(rendering_width, 0.0f);
  glVertex3f(1.0f, 1.0f, 0.0f); // Top right
  glTexCoord2f(rendering_width, rendering_height);
  glVertex3f(1.0f, -1.0f, 0.0f); // Bottom right
  glTexCoord2f(0.0f, rendering_height);
  glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom left
  glEnd();
    
  // Restore default states.
  glUseProgramObjectARB(default_shader_program);
  SDL_GL_UnbindTexture(render_target);
  glDisable(GL_TEXTURE_RECTANGLE_ARB);

  // And swap the window.
  SDL_GL_SwapWindow(window);
}

}

#endif // SOLARUS_HAVE_OPENGL
