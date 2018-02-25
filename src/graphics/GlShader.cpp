/*
 * Copyright (C) 2006-2018 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/core/Logger.h"
#include "solarus/core/QuestFiles.h"
#include "solarus/core/Size.h"
#include "solarus/core/System.h"
#include "solarus/graphics/GlShader.h"
#include "solarus/graphics/VertexArray.h"
#include "solarus/graphics/ShaderData.h"
#include "solarus/graphics/Surface.h"
#include "solarus/graphics/Video.h"

#ifdef SOLARUS_HAVE_OPENGLES2




// TODO Use an OpenGL loading library instead of this ugly code.
typedef struct GL_Context {
  void (APIENTRY *glAttachShader) (GLuint, GLuint);
  void (APIENTRY *glClear) (GLbitfield);
  void (APIENTRY *glClearColor) (GLclampf, GLclampf, GLclampf, GLclampf);
  void (APIENTRY *glCompileShader) (GLuint);
  GLuint (APIENTRY *glCreateProgram) (void);
  GLuint (APIENTRY *glCreateShader) (GLenum);
  void (APIENTRY *glDeleteProgram) (GLuint);
  void (APIENTRY *glDeleteShader) (GLuint);
  void (APIENTRY *glDisable) (GLenum);
  void (APIENTRY *glDrawArrays) (GLenum, GLint, GLsizei);
  void (APIENTRY *glEnable) (GLenum);
  void (APIENTRY *glGenBuffers)(GLsizei,GLuint*);
  void (APIENTRY *glEnableVertexAttribArray) (GLuint);
  GLenum (APIENTRY *glGetError) (void);
  void (APIENTRY *glGetIntegerv) (GLenum, GLint *);
  void (APIENTRY *glGetProgramInfoLog) (GLuint, GLsizei, GLsizei *, char *);
  void (APIENTRY *glGetProgramiv) (GLuint, GLenum, GLint *);
  void (APIENTRY *glGetShaderInfoLog) (GLuint, GLsizei, GLsizei *, char *);
  void (APIENTRY *glGetShaderiv) (GLuint, GLenum, GLint *);
  GLint (APIENTRY *glGetUniformLocation) (GLuint, const char *);
  GLint (APIENTRY *glGetAttribLocation)(GLuint,const GLchar*);
  void (APIENTRY *glBindBuffer)(GLuint,GLuint);
  void (APIENTRY *glBufferData)(GLenum,GLsizeiptr,const void*,GLenum);
  void (APIENTRY *glBindTexture)(GLuint,GLuint);
  void (APIENTRY *glLinkProgram) (GLuint);
#if __NACL__ || __ANDROID__
  void (APIENTRY *glShaderSource) (GLuint, GLsizei, const GLchar **, const GLint *);
#else
  void (APIENTRY *glShaderSource) (GLuint, GLsizei, const GLchar* const*, const GLint *);
#endif
  void (APIENTRY *glUniform1i) (GLint, GLint);
  void (APIENTRY *glUniform2f) (GLint, GLfloat, GLfloat);
  void (APIENTRY *glUseProgram) (GLuint);
  void (APIENTRY *glVertexAttribPointer) (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *);
} GLES2_Context;

namespace Solarus {

constexpr auto DEFAULT_VERTEX =
    R"(
    #version 100
    uniform mat4 sol_mvp_matrix;
    attribute vec2 sol_vertex;
    attribute vec2 sol_tex_coord;
    attribute vec4 sol_color;

    varying vec2 sol_vtex_coord;
    varying vec4 sol_vcolor;
    void main() {
      gl_Position = sol_mvp_matrix * vec4(sol_vertex,0,1);
      sol_vcolor = col_color;
      sol_vtex_coord = sol_tex_coord;
    }
    )";

constexpr auto DEFAULT_FRAGMENT =
    R"(
    #version 100
    uniform sampler2D sol_texture;
    varying vec2 sol_vtex_coord;
    varying vec4 sol_vcolor;
    void main() {
      vec4 tex_color = texture2D(sol_texture,sol_vtex_coord);
      gl_FragColor = tex_color*sol_vcolor;
    }
    )";

namespace {
GL_Context ctx; //TODO Remove if possible or make it a separate class
GLint default_shader_program = 0;
}

/**
 * \brief Initializes the GL 2D shader system.
 * \return \c true if GL 2D shaders are supported.
 */
bool GlShader::initialize() {

  // Setting some parameters.
  glClearDepthf(1.0); // Enables clearing of the depth buffer.

  // Initialize GLES2 functions.
#if SDL_VIDEO_DRIVER_UIKIT || SDL_VIDEO_DRIVER_ANDROID || SDL_VIDEO_DRIVER_PANDORA
#  define SDL_PROC(ret,func,params) ctx.func=func;
#else
#  define SDL_PROC(ret,func,params) \
    do { \
      ctx.func = (ret(*)params)(SDL_GL_GetProcAddress(#func)); \
      if ( ! ctx.func ) { \
        return SDL_SetError("Couldn't load GLES2 function %s: %s", #func, SDL_GetError()); \
        } \
      } while ( 0 );
#endif

  SDL_PROC(void, glAttachShader, (GLuint, GLuint))
  SDL_PROC(void, glClear, (GLbitfield))
  SDL_PROC(void, glClearColor, (GLclampf, GLclampf, GLclampf, GLclampf))
  SDL_PROC(void, glCompileShader, (GLuint))
  SDL_PROC(GLuint, glCreateProgram, (void))
  SDL_PROC(GLuint, glCreateShader, (GLenum))
  SDL_PROC(void, glDeleteProgram, (GLuint))
  SDL_PROC(void, glDeleteShader, (GLuint))
  SDL_PROC(void, glDisable, (GLenum))
  SDL_PROC(void, glDrawArrays, (GLenum, GLint, GLsizei))
  SDL_PROC(void, glEnable, (GLenum))
  SDL_PROC(void,glGenBuffers,(GLsizei,GLuint*))
  SDL_PROC(void, glEnableVertexAttribArray, (GLuint))
  SDL_PROC(GLenum, glGetError, (void))
  SDL_PROC(void, glGetIntegerv, (GLenum, GLint *))
  SDL_PROC(void, glGetProgramInfoLog, (GLuint, GLsizei, GLsizei *, char *))
  SDL_PROC(void, glGetProgramiv, (GLuint, GLenum, GLint *))
  SDL_PROC(void, glGetShaderInfoLog, (GLuint, GLsizei, GLsizei *, char *))
  SDL_PROC(void, glGetShaderiv, (GLuint, GLenum, GLint *))
  SDL_PROC(GLint, glGetUniformLocation, (GLuint, const char *))
  SDL_PROC(void,glBindBuffer,(GLuint,GLuint));
  SDL_PROC(void,glBufferData,(GLenum,GLsizeiptr,const void*,GLenum))
  SDL_PROC(void,glBindTexture,(GLuint,GLuint))
  SDL_PROC(void, glLinkProgram, (GLuint))
  SDL_PROC(GLint,glGetAttribLocation,(GLuint,const char*))
#if __NACL__ || __ANDROID__
  SDL_PROC(void, glShaderSource, (GLuint, GLsizei, const GLchar **, const GLint *))
#else
  SDL_PROC(void, glShaderSource, (GLuint, GLsizei, const GLchar* const*, const GLint *))
#endif
  SDL_PROC(void, glUniform1i, (GLint, GLint))
  SDL_PROC(void, glUniform2f, (GLint, GLfloat, GLfloat))
  SDL_PROC(void, glUseProgram, (GLuint))
  SDL_PROC(void, glVertexAttribPointer, (GLuint, GLint, GLenum, GLboolean, GLsizei, const void *))
#undef SDL_PROC

  // Get default shader.
  glGetIntegerv(GL_CURRENT_PROGRAM, &default_shader_program);

  return true;
}

/**
 * \brief Constructor.
 * \param shader_name The name of the shader to load.
 */
GlShader::GlShader(const std::string& shader_id):
    Shader(shader_id),
    program(0),
    vertex_shader(0),
    fragment_shader(0) {

  ctx.glGetError();

  // Load the shader.
  load();

  // Set up constant uniform variables.
  ctx.glUseProgram(program);

  GLint location = ctx.glGetUniformLocation(program, TEXTURE_NAME);
  if (location >= 0) {
    ctx.glUniform1i(location, 0);
  }

  const Size& quest_size = Video::get_quest_size();
  location = ctx.glGetUniformLocation(program, INPUT_SIZE_NAME);
  if (location >= 0) {
    ctx.glUniform2f(location, quest_size.width, quest_size.height);
  }

  position_location = ctx.glGetAttribLocation(program,POSITION_NAME);
  tex_coord_location = ctx.glGetAttribLocation(program,TEXCOORD_NAME);
  color_location = ctx.glGetAttribLocation(program,COLOR_NAME);

  ctx.glUseProgram(default_shader_program);
}

/**
 * \brief Destructor.
 */
GlShader::~GlShader() {

  ctx.glDeleteShader(vertex_shader);
  ctx.glDeleteShader(fragment_shader);
  ctx.glDeleteProgram(program);
}

/**
 * \copydoc Shader::load
 */
void GlShader::load() {

  GLint linked;

  // Load the shader data file.
  const std::string shader_file_name =
  "shaders/" + get_id() + ".dat";

  ShaderData data;
  bool success = data.import_from_quest_file(shader_file_name);
  if (!success) {
    return;
  }

  // Create the vertex and fragment shaders.
  vertex_shader = create_shader(GL_VERTEX_SHADER, data.get_vertex_source().c_str());
  fragment_shader = create_shader(GL_FRAGMENT_SHADER, data.get_fragment_source().c_str());

  // Create a program object with both shaders.
  program = ctx.glCreateProgram();
  if (program == 0) {
    Logger::error(std::string("Could not create OpenGL program"));
    return;
  }

  ctx.glAttachShader(program, vertex_shader);
  ctx.glAttachShader(program, fragment_shader);

  ctx.glLinkProgram(program);

  // Check GL status.
  ctx.glGetProgramiv(program, GL_LINK_STATUS, &linked);

  if (!linked) {
    GLint info_len = 0;
    ctx.glGetProgramiv(program, GL_INFO_LOG_LENGTH, &info_len);

    if (info_len > 1) {
      char* info = (char*)malloc(sizeof(char) * info_len);
      ctx.glGetProgramInfoLog(program, info_len, NULL, info);
      Logger::error(std::string("Failed to link shader ") + get_id() + std::string(" :\n") + info);
      free(info);
    }

    ctx.glDeleteProgram(program);
  }
}

/**
 * \brief Compile a shader from source.
 * \param type The type of the shader to fill and compile.
 * \param source Sources to compile.
 * \return The shader created and compiled.
 */
GLuint GlShader::create_shader(GLenum type, const char* source) {

  GLint compiled;

  // Create the shader object.
  GLuint shader = ctx.glCreateShader(type);
  check_gl_error();

  if(shader == 0) {
    Logger::error(std::string("Could not create OpenGL shader "));
    return shader;
  }

  ctx.glShaderSource(shader, 1, &source, NULL);
  ctx.glCompileShader(shader);

  // Check the compile status.
  ctx.glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

  if(!compiled) {
    GLint info_len = 0;

    ctx.glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);

    if(info_len > 1) {
      char* info = (char*)malloc(sizeof(char) * info_len);
      ctx.glGetShaderInfoLog(shader, info_len, NULL, info);
      Logger::error(std::string("Error compiling shader ") + get_id() + std::string(" :\n") + info);
      free(info);
    }

    ctx.glDeleteShader(shader);
    shader = 0;
  }

  return shader;
}

/**
 * \brief Check for a possible error returned by glGetError().
 */
void GlShader::check_gl_error() {
  GLenum gl_error(ctx.glGetError());

  while (gl_error != GL_NO_ERROR) {
    std::string error;

    switch(gl_error) {
      case GL_INVALID_OPERATION:
        error="INVALID_OPERATION";
        break;
      case GL_INVALID_ENUM:
        error="INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        error="INVALID_VALUE";
        break;
      case GL_OUT_OF_MEMORY:
        error="OUT_OF_MEMORY";
        break;
      case GL_INVALID_FRAMEBUFFER_OPERATION:
        error="INVALID_FRAMEBUFFER_OPERATION";
        break;
    }

    Logger::error(std::string("GL_") + error.c_str() + std::string(" - "));
    gl_error = ctx.glGetError();
  }
}

std::string GlShader::default_vertex_source() const {
  return DEFAULT_VERTEX;
}

std::string GlShader::default_fragment_source() const {
  return DEFAULT_FRAGMENT;
}

/**
 * \copydoc Shader::render
 */
void GlShader::render(const SurfacePtr& quest_surface) {

  Shader::render(quest_surface);

  SDL_Renderer* renderer = Video::get_renderer();
  SDL_Window* window = Video::get_window();
  SDL_Texture* render_target = Video::get_render_target();

  // Clear the window.
  ctx.glClearColor(0.0, 0.0, 0.0, 1.0);
  ctx.glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  //ctx.glLoadIdentity();

  // Clear the render target.
  SDL_SetRenderTarget(renderer, render_target);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderSetClipRect(renderer, nullptr);
  SDL_RenderClear(renderer);

  // Draw on the render target.
  quest_surface->render(renderer);

  // Render on the window using OpenGL, to apply a shader if we have to.
  SDL_SetRenderTarget(renderer, nullptr);

  ctx.glEnable(GL_TEXTURE_2D);
  SDL_GL_BindTexture(render_target, nullptr, nullptr);

  ctx.glUseProgram(program);

  // Update uniform variables.
  GLint location = ctx.glGetUniformLocation(program, "solarus_time");
  if (location >= 0) {
    ctx.glUniform1i(location, System::now());
  }

  const Size& window_size = Video::get_window_size();
  location = ctx.glGetUniformLocation(program, "solarus_output_size");
  if (location >= 0) {
    ctx.glUniform2f(location, window_size.width, window_size.height);
  }

  // Draw.
  enum  {
    ATTRIB_VERTEX = 0,
    ATTRIB_TEXTURE_POSITION
  };
  static const GLfloat square_vertices[] = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    -1.0f, 1.0f,
    1.0f, 1.0f,
  };
  static const GLfloat texture_vertices[] = {
    1.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    0.0f, 0.0f,
  };

  ctx.glVertexAttribPointer(ATTRIB_VERTEX, 2, GL_FLOAT, 0, 0, square_vertices);
  ctx.glEnableVertexAttribArray(ATTRIB_VERTEX);
  ctx.glVertexAttribPointer(ATTRIB_TEXTURE_POSITION, 2, GL_FLOAT, 0, 0, texture_vertices);
  ctx.glEnableVertexAttribArray(ATTRIB_TEXTURE_POSITION);

  ctx.glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  // Restore default states.
  ctx.glUseProgram(default_shader_program);
  SDL_GL_UnbindTexture(render_target);

  ctx.glDisable(GL_TEXTURE_2D);

  // And swap the window.
  SDL_GL_SwapWindow(window);
}

void GlShader::render(const VertexArrayPtr& vertex_array) {
  if(vertex_array->vertex_buffer == 0) {
    //Generate vertex-buffer
    ctx.glGenBuffers(GL_ARRAY_BUFFER,&vertex_array->vertex_buffer);
  }
  ctx.glBindBuffer(GL_ARRAY_BUFFER,vertex_array->vertex_buffer);
  if(vertex_array->buffer_dirty) {
    //Upload vertex buffer
    ctx.glBufferData(GL_ARRAY_BUFFER,vertex_array->vertex_count()*sizeof(Vertex),vertex_array->data(),GL_DYNAMIC_DRAW);

  }
  ctx.glUseProgram(program);

  ctx.glEnableVertexAttribArray(position_location);
  ctx.glVertexAttribPointer(position_location,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,position));

  ctx.glEnableVertexAttribArray(tex_coord_location);
  ctx.glVertexAttribPointer(tex_coord_location,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,texcoords));

  ctx.glEnableVertexAttribArray(color_location);
  ctx.glVertexAttribPointer(color_location,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(Vertex),(void*)offsetof(Vertex,color));

  glDrawArrays(vertex_array.get_primitive_type(),0,vertex_array.get_count());

  ctx.glUseProgram(default_shader_program);
  ctx.glBindBuffer(GL_ARRAY_BUFFER,0);
}

}

#endif /* SOLARUS_HAVE_OPENGLES2 */
