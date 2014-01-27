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
#include "lowlevel/Shader.h"
#include "lowlevel/Surface.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Video.h"
#include "lua/LuaTools.h"

namespace solarus {

#if SOLARUS_HAVE_OPENGL_OR_ES == 1

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
PFNGLGETHANDLEARBPROC Shader::glGetHandleARB;

SDL_GLContext Shader::gl_context = NULL;
GLhandleARB Shader::default_shader_program = 0;
GLenum Shader::gl_texture_type = GL_TEXTURE_2D;
std::string Shader::shading_language_version = "";
Shader* Shader::loading_shader = NULL;

#endif

/**
 * \brief Initializes OpenGL and the shader system.
 * \return \c true if shaders are supported.
 */
bool Shader::initialize() {

#if SOLARUS_HAVE_OPENGL_OR_ES == 1
  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  if (!(gl_context = SDL_GL_CreateContext(Video::get_window()))) {
    Debug::warning("Unable to create OpenGL context : " + std::string(SDL_GetError()));
    return false;
  }

  // Setting some parameters
  glClearDepth(1.0); // Enables clearing of the depth buffer.
  glEnable(GL_DEPTH_TEST); // The type of depth test to do.
  glDepthFunc(GL_LESS); // Enables depth testing.
  glShadeModel(GL_SMOOTH); // Enables smooth color shading.
  
  // Use late swap tearing, or try to use the classic swap interval (aka VSync) if not supported.
  if(SDL_GL_SetSwapInterval(-1) == -1) {
    SDL_GL_SetSwapInterval(1);
  }
  
  // Get the shading language version.
  shading_language_version = *glGetString(GL_SHADING_LANGUAGE_VERSION);
  
  // Check for shader support
  if((SDL_GL_ExtensionSupported("GL_ARB_texture_rectangle") || // WORKAROUND : this is the way SDL check for the GL_ARB support
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
        glUseProgramObjectARB &&
        glGetHandleARB) {

      // Get ARB default configuration.
      default_shader_program = glGetHandleARB(GL_CURRENT_PROGRAM);
      gl_texture_type = GL_TEXTURE_RECTANGLE_ARB;
      
      return true;
    }
    else {
      // TODO Force SDL to use 2D fetch textures.
    }
  }
  
  // TODO Use fetch sampler and shaders if GL_ARB not supported.
  // TODO2 Deactivate shaders if fetch shaders are not supported too.
  
  // TODISCUSS Use a virtual class to access the used shader context type ?
#endif

  return false;
}

/**
 * \brief Free shader-related context.
 */
void Shader::quit() {

#if SOLARUS_HAVE_OPENGL_OR_ES == 1
  SDL_GL_DeleteContext(gl_context);
#endif
}

#if SOLARUS_HAVE_OPENGL_OR_ES == 1

/**
 * \brief Constructor.
 * \param shader_name The name of the shader to load.
 */
Shader::Shader(const std::string& shader_name):
    program(0),
    vertex_shader(0),
    fragment_shader(0),
    shader_name(shader_name),
    window_scale(1.0) {
    
  glGetError();
    
  // Load the shader.
  load(shader_name);
    
  // Notify the shader program that the uniform sampler will be in the texture unit 0.
  glUseProgramObjectARB(program);
  GLint location = glGetUniformLocationARB(program, std::string("solarus_sampler").c_str());
  if (location >= 0) {
    glUniform1iARB(location, 0);
  }
  restore_default_shader_program();
}
  
/**
 * \brief Destructor.
 */
Shader::~Shader() {
    
  glDeleteObjectARB(vertex_shader);
  glDeleteObjectARB(fragment_shader);
  glDeleteObjectARB(program);
}

/**
 * \brief Construct a shader from a name.
 * \param shader_name The name of the shader to load.
 * \return The created shader, or NULL if the shader fails to compile.
 */
Shader* Shader::create(const std::string& shader_name) {
    
  Shader* shader = new Shader(shader_name);
    
  if (glGetError() != GL_NO_ERROR) {
    Debug::warning("Cannot compile shader '" + shader_name + "'");
    return NULL;
  }
  return shader;
}
  
/**
 * \brief Compile a shader from source.
 * \param shader Reference to the shader to fill and compile.
 * \param source Sources to compile.
 */
void Shader::compile_shader(GLhandleARB& shader, const char* source) {

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
 * \brief Restore the default shader.
 */
void Shader::restore_default_shader_program() {

  glUseProgramObjectARB(default_shader_program);
}

/**
 * \brief Set up OpenGL rendering parameter.
 * This basically reset the projection matrix.
 */
void Shader::set_rendering_settings() {

  Rectangle quest_size = Video::get_quest_size();
  static const GLdouble aspect = GLdouble(quest_size.get_width() / quest_size.get_height());

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, 0.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
}

/**
 * \brief Draws the quest surface on the screen in a shader-allowed context.
 * It will perform the render using the OpenGL API directly.
 * \param quest_surface the surface to render on the screen
 * \param shader the shader to apply if any, or NULL.
 */
void Shader::shaded_render(Surface& quest_surface, Shader* shader) {

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

  glEnable(gl_texture_type);
  SDL_GL_BindTexture(render_target, &rendering_width, &rendering_height);
  if (shader != NULL) {
    shader->apply();
  }

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
  if (shader != NULL) {
    restore_default_shader_program();
  }
  SDL_GL_UnbindTexture(render_target);
  glDisable(gl_texture_type);

  // And swap the window.
  SDL_GL_SwapWindow(window);  
}

/**
 * \brief Get the name of the shader, which is also the name of the related video mode.
 * \return The name of the shader.
 */
const std::string& Shader::get_name() {

  return shader_name;
}
  
/**
 * \brief Get the scale to apply on the quest size to get the final default size of the related video mode.
 * \return The window scale.
 */
double Shader::get_window_scale() {

  return window_scale;
}

/**
 * \brief Load all files from the corresponding shader, depending on the driver and shader names.
 * Parse the Lua file and compile GLSL others.
 * \param shader_name The name of the shader to load.
 */
void Shader::load(const std::string& shader_name) {

  const std::string shader_path = 
      "shaders/filters/" + shader_name;

  // Parse the lua file
  load_lua_file(shader_path);
}

/**
 * \brief Load and parse the Lua file of the requested shader.
 * \param path The path to the lua file, relative to the data folder.
 */
void Shader::load_lua_file(const std::string& path) {

  lua_State* l = luaL_newstate();
  luaL_openlibs(l);  // FIXME don't open the libs
  size_t size;
  char* buffer;  

  FileTools::data_file_open_buffer(path, &buffer, &size);
  int load_result = luaL_loadbuffer(l, buffer, size, path.c_str());
  loading_shader = this;

  if (load_result != 0) {
    // Syntax error in the lua file.
    Debug::die(std::string("Failed to load ") + path + " : " + lua_tostring(l, -1));
  }
  else {
    const Rectangle& quest_size = Video::get_quest_size();
    lua_register(l, "shader", l_shader);

    // Send some parameters to the lua script.
    lua_pushstring(l, Video::get_rendering_driver_name().c_str());
    lua_pushstring(l, shading_language_version.c_str());
    lua_pushstring(l, get_sampler_type().c_str());
    lua_pushinteger(l, quest_size.get_width());
    lua_pushinteger(l, quest_size.get_height());

    if (lua_pcall(l, 5, 0, 0) != 0) {

      // Runtime error.
      Debug::die(std::string("Failed to parse ") + path + " : " + lua_tostring(l, -1));
      lua_pop(l, 6);
    }
  }

  loading_shader = NULL;
  FileTools::data_file_close_buffer(buffer);
  lua_close(l);
}

/**
 * \brief Callback when parsing the lua file. Fill the loading shader with the result.
 * \param l The lua state.
 */
int Shader::l_shader(lua_State* l) {

  if (loading_shader != NULL) {

    GLhandleARB& program = loading_shader->program,
      vertex_shader = loading_shader->vertex_shader,
      fragment_shader = loading_shader->fragment_shader;

    // Retrieve the shader properties from the table parameter.
    luaL_checktype(l, 1, LUA_TTABLE);

    const double& window_scale =
        LuaTools::opt_number_field(l, 1, "window_scale", 1.0);
    const std::string shader_name =
        LuaTools::opt_string_field(l, 1, "name", loading_shader->shader_name);
    const std::string vertex_source =
        LuaTools::check_string_field(l, 1, "vertex_source");
    const std::string fragment_source =
        LuaTools::check_string_field(l, 1, "fragment_source");

    loading_shader->window_scale = window_scale;
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
  }

  return 0;
}

/**
 * \brief Get the type of the sampler type to use into the GLSL shader.
 * \return A string containing the type of sampler to use.
 */
const std::string Shader::get_sampler_type() {

  if (gl_texture_type == GL_TEXTURE_RECTANGLE_ARB) {
    return "sampler2DRect";
  }
  return "sampler2D";
}
  
/**
 * \brief Apply the shader program.
 */
void Shader::apply() {
  glUseProgramObjectARB(program);
}
 
#endif

}
