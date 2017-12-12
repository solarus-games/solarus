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
#include "solarus/lowlevel/shaders/GL_ARBShader.h"
#include "solarus/lowlevel/shaders/ShaderData.h"
#include "solarus/lowlevel/Logger.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Video.h"

#ifdef SOLARUS_HAVE_OPENGL

namespace Solarus {

namespace {

PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
PFNGLGETINFOLOGARBPROC glGetInfoLogARB;
PFNGLGETOBJECTPARAMETERIVARBPROC glGetObjectParameterivARB;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
PFNGLUNIFORM1IARBPROC glUniform1iARB;
PFNGLUNIFORM2FARBPROC glUniform2fARB;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
PFNGLGETHANDLEARBPROC glGetHandleARB;

GLhandleARB default_shader_program = 0;

/**
 * @brief Casts a pointer-to-object (void*) to a pointer-to-function.
 *
 * This function avoids compilation warnings when using SDL_GL_GetProcAddress(),
 * because directly casting a pointer-to-object to a pointer-to-function
 * is not allowed.
 *
 * @param object_ptr The pointer to cast.
 * @return The pointer to a function.
 */
template<typename FunctionPointerType>
FunctionPointerType get_proc_address_cast(void* object_ptr) {
  return *reinterpret_cast<FunctionPointerType*>(&object_ptr);
}

}  // Anonymous namespace

/**
 * \brief Initializes the GL ARB shader system.
 * \return \c true if GL ARB shaders are supported.
 */
bool GL_ARBShader::initialize() {

  // TODO Make sure that SDL doesn't disable GL ARB for render targets even if available.

  // Setting some parameters.
  glClearDepth(1.0); // Enables clearing of the depth buffer.
  glShadeModel(GL_SMOOTH); // Enables smooth color shading.

  // Initialize GL ARB functions.
  if (SDL_GL_ExtensionSupported("GL_ARB_shader_objects") &&
      SDL_GL_ExtensionSupported("GL_ARB_shading_language_100") &&
      SDL_GL_ExtensionSupported("GL_ARB_vertex_shader") &&
      SDL_GL_ExtensionSupported("GL_ARB_fragment_shader")) {
    glAttachObjectARB = get_proc_address_cast<PFNGLATTACHOBJECTARBPROC>(SDL_GL_GetProcAddress("glAttachObjectARB"));
    glCompileShaderARB = get_proc_address_cast<PFNGLCOMPILESHADERARBPROC>(SDL_GL_GetProcAddress("glCompileShaderARB"));
    glCreateProgramObjectARB = get_proc_address_cast<PFNGLCREATEPROGRAMOBJECTARBPROC>(SDL_GL_GetProcAddress("glCreateProgramObjectARB"));
    glCreateShaderObjectARB = get_proc_address_cast<PFNGLCREATESHADEROBJECTARBPROC>(SDL_GL_GetProcAddress("glCreateShaderObjectARB"));
    glDeleteObjectARB = get_proc_address_cast<PFNGLDELETEOBJECTARBPROC>(SDL_GL_GetProcAddress("glDeleteObjectARB"));
    glGetInfoLogARB = get_proc_address_cast<PFNGLGETINFOLOGARBPROC>(SDL_GL_GetProcAddress("glGetInfoLogARB"));
    glGetObjectParameterivARB = get_proc_address_cast<PFNGLGETOBJECTPARAMETERIVARBPROC>(SDL_GL_GetProcAddress("glGetObjectParameterivARB"));
    glGetUniformLocationARB = get_proc_address_cast<PFNGLGETUNIFORMLOCATIONARBPROC>(SDL_GL_GetProcAddress("glGetUniformLocationARB"));
    glLinkProgramARB = get_proc_address_cast<PFNGLLINKPROGRAMARBPROC>(SDL_GL_GetProcAddress("glLinkProgramARB"));
    glShaderSourceARB = get_proc_address_cast<PFNGLSHADERSOURCEARBPROC>(SDL_GL_GetProcAddress("glShaderSourceARB"));
    glUniform1iARB = get_proc_address_cast<PFNGLUNIFORM1IARBPROC>(SDL_GL_GetProcAddress("glUniform1iARB"));
    glUniform2fARB = get_proc_address_cast<PFNGLUNIFORM2FARBPROC>(SDL_GL_GetProcAddress("glUniform2fARB"));
    glUseProgramObjectARB = get_proc_address_cast<PFNGLUSEPROGRAMOBJECTARBPROC>(SDL_GL_GetProcAddress("glUseProgramObjectARB"));
    glGetHandleARB = get_proc_address_cast<PFNGLGETHANDLEARBPROC>(SDL_GL_GetProcAddress("glGetHandleARB"));
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
        glUniform2fARB &&
        glUseProgramObjectARB &&
        glGetHandleARB) {

      // Get default shader.
      default_shader_program = glGetHandleARB(GL_CURRENT_PROGRAM);

      return true;
    }
  }

  return false;
}

/**
 * \brief Constructor.
 * \param shader_id Id of the shader to load.
 */
GL_ARBShader::GL_ARBShader(const std::string& shader_id):
    Shader(shader_id),
    program(0),
    vertex_shader(0),
    fragment_shader(0) {

  glGetError();

  // Load the shader.
  load();

  // Set up constant uniform variables.
  glUseProgramObjectARB(program);

  GLint location = glGetUniformLocationARB(program, "solarus_sampler");
  if (location >= 0) {
    glUniform1iARB(location, 0);
  }

  const Size& quest_size = Video::get_quest_size();
  location = glGetUniformLocationARB(program, "solarus_input_size");
  if (location >= 0) {
    glUniform2fARB(location, quest_size.width, quest_size.height);
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
 * \copydoc Shader::load
 */
void GL_ARBShader::load() {

  // Load the shader data file.
  const std::string shader_file_name =
      "shaders/" + get_id() + ".dat";

  ShaderData data;
  bool success = data.import_from_quest_file(shader_file_name);
  if (!success) {
    return;
  }
  set_data(data);

  // Create the vertex and fragment shaders.
  vertex_shader = create_shader(GL_VERTEX_SHADER_ARB, data.get_vertex_source().c_str());
  fragment_shader = create_shader(GL_FRAGMENT_SHADER_ARB, data.get_fragment_source().c_str());

  // Create a program object with both shaders.
  program = glCreateProgramObjectARB();
  glAttachObjectARB(program, vertex_shader);
  glAttachObjectARB(program, fragment_shader);

  glLinkProgramARB(program);

  // Check GL status.
  GLint status;
  glGetObjectParameterivARB(program, GL_OBJECT_LINK_STATUS_ARB, &status);

  if (status == 0) {
    GLint length = 0;
    char* info = nullptr;

    glGetObjectParameterivARB(program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    info = SDL_stack_alloc(char, length + 1);
    glGetInfoLogARB(program, length, nullptr, info);
    Logger::error(std::string("Failed to link shader ") + get_id() + std::string(" :\n") + info);
    SDL_stack_free(info);
  }
}

/**
 * \brief Compile a shader from source.
 * \param type The type of the shader to fill and compile.
 * \param source Sources to compile.
 */
GLhandleARB GL_ARBShader::create_shader(uint type, const char* source) {

  GLint status;

  // Create the shader object.
  GLhandleARB shader = glCreateShaderObjectARB(type);

  glShaderSourceARB(shader, 1, &source, nullptr);
  glCompileShaderARB(shader);

  // Check the compile status.
  glGetObjectParameterivARB(shader, GL_OBJECT_COMPILE_STATUS_ARB, &status);
  if (status == 0) {
    GLint length;
    char* info;

    glGetObjectParameterivARB(shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &length);
    info = SDL_stack_alloc(char, length + 1);
    glGetInfoLogARB(shader, length, nullptr, info);
    Logger::error(std::string("Failed to compile shader ") + get_id() + std::string(" :\n") + info);
    SDL_stack_free(info);
  }

  return shader;
}

/**
 * \brief Set up OpenGL rendering parameter.
 * This basically resets the projection matrix.
 */
void GL_ARBShader::set_rendering_settings() {

  const Size& quest_size = Video::get_quest_size();
  static const GLdouble aspect = GLdouble(quest_size.width / quest_size.height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect, 0.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
}

/**
 * \brief Draws the quest surface on the screen in a shader-allowed context.
 * It will perform the render using the OpenGL API directly.
 * \param quest_surface The surface to render on the screen.
 */
void GL_ARBShader::render(const SurfacePtr& quest_surface) const {

  Shader::render(quest_surface);

  SDL_Renderer* renderer = Video::get_renderer();
  SDL_Window* window = Video::get_window();
  SDL_Texture* render_target = Video::get_render_target();

  // Clear the window.
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // Clear the render target.
  SDL_SetRenderTarget(renderer, render_target);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderSetClipRect(renderer, nullptr);
  SDL_RenderClear(renderer);

  // Draw on the render target.
  quest_surface->render(renderer);

  // Render on the window using OpenGL, to apply a shader if we have to.
  SDL_SetRenderTarget(renderer, nullptr);
  set_rendering_settings();

  glEnable(GL_TEXTURE_2D);
  SDL_GL_BindTexture(render_target, nullptr, nullptr);

  glUseProgramObjectARB(program);

  // Update uniform variables.
  GLint location = glGetUniformLocationARB(program, "solarus_time");
  if (location >= 0) {
    glUniform1iARB(location, System::now());
  }

  const Size& window_size = Video::get_window_size();
  location = glGetUniformLocationARB(program, "solarus_output_size");
  if (location >= 0) {
    glUniform2fARB(location, window_size.width, window_size.height);
  }

  // Draw.
  const GLfloat square_texcoord[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
  const GLfloat* texcoord = square_texcoord;

  glBegin(GL_QUADS);
  glTexCoord2f(texcoord[0], texcoord[1]);
  glVertex3f(-1.0f, 1.0f, 0.0f); // Top left
  glTexCoord2f(texcoord[2], texcoord[1]);
  glVertex3f(1.0f, 1.0f, 0.0f); // Top right
  glTexCoord2f(texcoord[2], texcoord[3]);
  glVertex3f(1.0f, -1.0f, 0.0f); // Bottom right
  glTexCoord2f(texcoord[0], texcoord[3]);
  glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom left
  glEnd();

  // Restore default states.
  glUseProgramObjectARB(default_shader_program);
  SDL_GL_UnbindTexture(render_target);

  glDisable(GL_TEXTURE_2D);

  // And swap the window.
  SDL_GL_SwapWindow(window);
}

}

#endif /* SOLARUS_HAVE_OPENGL */
