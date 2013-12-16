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
#include "lowlevel/FileTools.h"
#include "lowlevel/Video.h"

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

SDL_GLContext Shader::gl_context;
GLhandleARB Shader::default_shader_program;
Shader* Shader::loading_shader = NULL;

#endif

/**
 * \brief Initialize OpenGL and the shader system.
 * \return true if shader are supported.
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
  glClearDepth(1.0); // Enables Clearing Of The Depth Buffer.
  glEnable(GL_DEPTH_TEST); // The Type Of Depth Test To Do.
  glDepthFunc(GL_LESS); // Enables Depth Testing.
  glShadeModel(GL_SMOOTH); // Enables Smooth Color Shading.
  
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
      default_shader_program = glGetHandleARB(GL_CURRENT_PROGRAM);
      return true;
    }
  }
#endif

  Debug::warning("OpenGL shaders not supported : " + std::string(SDL_GetError()));
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
 * \brief Compile a shader from source.
 * \param shader Reference to the shader to fill and compile.
 * \param source Sources to compile.
 * \return true if success.
 */
bool Shader::compile_shader(GLhandleARB& shader, const char* source) {
  
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
    
    return false;
  } else {
    return true;
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
 * \brief Constructor.
 * \param shadername The name of the shader to load.
 */
Shader::Shader(std::string shader_name):
  program(0),
  vertex_shader(0),
  fragment_shader(0),
  logical_scale(1.0) {
    
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
    
  Debug::check_assertion(glGetError() == GL_NO_ERROR, "Cannot compile the shader : " + shader_name);
}

/**
 * \brief Destructor.
 */
Shader::~Shader()
{
  glDeleteObjectARB(vertex_shader);
  glDeleteObjectARB(fragment_shader);
  glDeleteObjectARB(program);
}

/**
 * \brief Get the scale to apply on the quest size to get the final window size of this shader.
 * \return The logical scale.
 */
double Shader::get_logical_scale() {
  return logical_scale;
}

/**
 * \brief Load all files from the corresponding shader, depending on the driver and shader names.
 * Parse the Lua file and compile GLSL others.
 * \param shader_name The name of the shader to load.
 */
void Shader::load(const std::string& shader_name) {
  
  const std::string base_shader_path = 
      "shaders/filters/" + Video::get_rendering_driver_name() +
      "/" + shader_name + "/" + shader_name;
  
  // Parse the lua file
  load_lua_file(base_shader_path + ".lua");
  
  // Create the vertex and fragment shaders.
  load_shader_file(base_shader_path + ".slv", GL_VERTEX_SHADER_ARB, &vertex_shader);
  load_shader_file(base_shader_path + ".slf", GL_FRAGMENT_SHADER_ARB, &fragment_shader);
  
  // Create one program object to rule them all ...
  program = glCreateProgramObjectARB();
  
  // ... and in the darkness bind them
  glAttachObjectARB(program, vertex_shader);
  glAttachObjectARB(program, fragment_shader);
  glLinkProgramARB(program);
}

/**
 * \brief Load and parse the Lua file of the requested shader.
 * \param path The path to the lua file, relative to the data folder.
 */
void Shader::load_lua_file(const std::string& path) {
  
  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;  
  
  FileTools::data_file_open_buffer(path, &buffer, &size);
  int load_result = luaL_loadbuffer(l, buffer, size, path.c_str());
  loading_shader = this;
  
  if (load_result != 0) {
    // Syntax error in the lua file.
    Debug::die(std::string("Failed to load : ") + path);
  }
  else {
    
    lua_register(l, "shader", l_shader);
    if (lua_pcall(l, 0, 0, 0) != 0) {

      // Runtime error.
      Debug::die(std::string("Failed to parse: ") + path);
      lua_pop(l, 1);
    }
  }
  
  loading_shader = NULL;
  FileTools::data_file_close_buffer(buffer);
  lua_close(l);
}

/**
 * \brief Load and compile a shader from a vertex or shader file.
 * \param path The path to the shader file source, relative to the data folder.
 * \param shader_type The type of shader (vertex or fragment).
 * \param shader The GLhandleARB pointer to fill with the result.
 */
void Shader::load_shader_file(const std::string& path, GLenum shader_type, GLhandleARB* shader) {
  
  size_t size;
  char* buffer;
  std::string source; 
  
  FileTools::data_file_open_buffer(path, &buffer, &size);
  source = std::string(buffer, size); // Make sure the buffer is a valid string.
  FileTools::data_file_close_buffer(buffer);
  
  *shader = glCreateShaderObjectARB(shader_type);
  
  if (!compile_shader(*shader, source.c_str())) {
    Debug::die("Cannot compile the shader : " + path);
  }
}

/**
 * \brief Callback when parsing the lua file. Fill the loading shader with the result.
 * \param l The lua state.
 */
int Shader::l_shader(lua_State* l) {
  
  if (loading_shader != NULL) {

    // Retrieve the shader properties from the table parameter.
    luaL_checktype(l, 1, LUA_TTABLE);
  
    const double& window_scale =
        LuaContext::opt_number_field(l, 1, "logical_scale", 1.0);
    
    loading_shader->logical_scale = window_scale;
  }

  return 0;
}

/**
 * \brief Apply the shader program.
 */
void Shader::apply()
{
  glUseProgramObjectARB(program);
}

#endif

}

