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

Shader* Shader::current_loaded_shader = NULL;
SDL_bool Shader::shaders_supported = SDL_FALSE;
GLint Shader::default_shader_program;

/**
 * \brief Initialize the OpenGL shader system.
 */
void Shader::initialize() {
  
  VideoManager* videomanager = VideoManager::get_instance();
  
  SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
  
  if (!SDL_GL_CreateContext(videomanager->get_window())) {
    Debug::die("Unable to create OpenGL context : " + std::string(SDL_GetError()));
  }

  GLdouble aspect;
  Rectangle quest_size = videomanager->get_quest_size();
  
  glDepthFunc(GL_LESS);                        // The Type Of Depth Test To Do
  glEnable(GL_DEPTH_TEST);                     // Enables Depth Testing
  glShadeModel(GL_SMOOTH);                     // Enables Smooth Color Shading
  
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
    
    // Only authorize default modes.
    videomanager->initialize_video_modes(true);
    Debug::warning("OpenGL shaders not supported.");
    
    return;
  }
  
  // Get the default shader program
  glGetIntegerv(GL_CURRENT_PROGRAM, &default_shader_program);

  // Initialize default and shaded video modes.
  videomanager->initialize_video_modes();
}

/**
 * \brief Compile a shader from source.
 * \return true if success.
 */
SDL_bool Shader::compile_shader(GLhandleARB& shader, const char* source) {
  
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

/**
 * \brief Restore the default shader.
 */
void Shader::restore_default_shader_program() {

  glUseProgramObjectARB((void*)default_shader_program);
}

/**
 * \brief Constructor.
 * \param shadername The name of the shader to load.
 */
Shader::Shader(std::string shader_name) :
  logical_scale(1.0),
  program(0),
  vertex_shader(0),
  fragment_shader(0) {
    
  if (shaders_supported) {
    
    const int num_tmus_bound = 4;
    GLint location;
    
    glGetError();
    
    // Load the shader.
    load(shader_name);
    
    // Set up some uniform variables
    glUseProgramObjectARB(program);
    for (int i = 0; i < num_tmus_bound; ++i) {
      char tex_name[5];
      SDL_snprintf(tex_name, SDL_arraysize(tex_name), "tex%d", i);
      location = glGetUniformLocationARB(program, tex_name);
      if (location >= 0) {
        glUniform1iARB(location, i);
      }
    }
    glUseProgramObjectARB((void*)default_shader_program);
    
    Debug::check_assertion(glGetError() == GL_NO_ERROR, "Cannot compile the shader : " + shader_name);
  }
}

/**
 * \brief Destructor.
 */
Shader::~Shader()
{
  if (shaders_supported) {
    glDeleteObjectARB(vertex_shader);
    glDeleteObjectARB(fragment_shader);
    glDeleteObjectARB(program);
  }
}

/**
 * \brief Get the scale to apply on the quest size to get the final window size of this shader.
 * \return The logical scale.
 */
double Shader::get_logical_scale() {
  return logical_scale;
}

/**
 * \brief Load all shader files, parse the Lua one and compile GLSL others.
 */
void Shader::load(std::string& shader_name) {
  
  // Parse the lua file
  load_lua_file(shader_name);
  
  // Get the vertex and fragment shader sources.
  //TODO remove color shader and get sources from the corresponding driver/shader file.
#if defined(SOLARUS_HAVE_GLES)
  
#else
  const char* vertex_source = "varying vec4 v_color;\n"
  "\n"
  "void main()\n"
  "{\n"
  "    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
  "    v_color = gl_Color;\n"
  "}";
  const char* fragment_source = "varying vec4 v_color;\n"
  "\n"
  "void main()\n"
  "{\n"
  "    gl_FragColor = v_color;\n"
  "}";
#endif
  
  // Create one program object to rule them all ...
  program = glCreateProgramObjectARB();
  
  // Create the vertex shader
  vertex_shader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
  if (!compile_shader(vertex_shader, vertex_source)) {
    Debug::die("Cannot compile the vertex shader for : " + shader_name);
  }
  
  // Create the fragment shader
  fragment_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
  if (!compile_shader(fragment_shader, fragment_source)) {
    Debug::die("Cannot compile the fragment shader for : " + shader_name);
  }
  
  // ... and in the darkness bind them
  glAttachObjectARB(program, vertex_shader);
  glAttachObjectARB(program, fragment_shader);
  glLinkProgramARB(program);
}

/**
 * \brief Load and parse the Lua file of the requested shader.
 */
void Shader::load_lua_file(std::string& shader_name) {
  
  lua_State* l = luaL_newstate();
  size_t size;
  char* buffer;  
  const std::string& file_path = 
      "shaders/" + get_rendering_driver() + "/" + shader_name + "/" + shader_name + ".lua";
  FileTools::data_file_open_buffer(file_path, &buffer, &size);
  int load_result = luaL_loadbuffer(l, buffer, size, file_path.c_str());
  current_loaded_shader = this;
  
  if (load_result != 0) {
    // Syntax error in quest.dat.
    Debug::die(std::string("Failed to load : ") + file_path);
  }
  else {
    
    lua_register(l, "shader", l_shader);
    if (lua_pcall(l, 0, 0, 0) != 0) {
      // Loading the lua file failed.
      // There may be a syntax error, or this is a quest for Solarus 0.9.
      // There was no version number at that time.
      
      if (std::string(buffer).find("[info]")) {
        // Quest format of Solarus 0.9.
        Debug::die(std::string("This quest is made for Solarus 0.9 but you are running Solarus ")
            + SOLARUS_VERSION);
      }
      else {
        // Runtime error.
        Debug::die(std::string("Failed to parse: ") + file_path);
      }
      lua_pop(l, 1);
    }
  }
  
  current_loaded_shader = NULL;
  FileTools::data_file_close_buffer(buffer);
  lua_close(l);
}

int Shader::l_shader(lua_State* l) {
  
  // Retrieve the shader properties from the table parameter.
  luaL_checktype(l, 1, LUA_TTABLE);
  
  const double& window_scale =
      LuaContext::opt_number_field(l, 1, "logical_scale", 1.0);
  
  current_loaded_shader->logical_scale = window_scale;
  
  return 0;
}

// Move to VideoManager ?
/**
 * \brief Get the current rendering driver (OpenGL ES2 or OpenGL).
 * \return a string containing the rendering driver name.
 */
const std::string Shader::get_rendering_driver() {

#if defined(SOLARUS_HAVE_GLES)
  return "opengles2";
#else
  return "opengl";
#endif
}

/**
 * \brief Apply the current shader if supported, and render to the window.
 * \param renderer The renderer.
 */
void Shader::apply()
{
  if (shaders_supported) {
    glUseProgramObjectARB(program);
  }
}