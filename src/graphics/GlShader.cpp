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
#include "solarus/graphics/ShaderContext.h"

#include "solarus/third_party/glm/gtc/type_ptr.hpp"
#include "solarus/third_party/glm/gtx/transform.hpp"
#include "solarus/third_party/glm/gtx/matrix_transform_2d.hpp"


namespace Solarus {

constexpr auto DEFAULT_VERTEX =
    R"(
    #version 100
    precision mediump float;

    uniform mat4 sol_mvp_matrix;
    uniform mat3 sol_uv_matrix;
    attribute vec2 sol_vertex;
    attribute vec2 sol_tex_coord;
    attribute vec4 sol_color;

    varying vec2 sol_vtex_coord;
    varying vec4 sol_vcolor;
    void main() {
      gl_Position = sol_mvp_matrix * vec4(sol_vertex,0,1);
      sol_vcolor = sol_color;
      sol_vtex_coord = (sol_uv_matrix * vec3(sol_tex_coord,1)).xy;
    }
    )";

constexpr auto DEFAULT_FRAGMENT =
    R"(
    #version 100
    precision mediump float;
    uniform sampler2D sol_texture;
    varying vec2 sol_vtex_coord;
    varying vec4 sol_vcolor;
    void main() {
      vec4 tex_color = texture2D(sol_texture,sol_vtex_coord);
      gl_FragColor = tex_color*sol_vcolor;
    }
    )";

struct GlContext {
#define SDL_PROC(ret,func,params) ret (APIENTRY* func) params;
  #include "gles2funcs.h"
#undef SDL_PROC
};

namespace {
 GlContext ctx;
}

VertexArray GlShader::screen_quad(TRIANGLES);

/**
 * \brief Initializes the GL 2D shader system.
 * \return \c true if GL 2D shaders are supported.
 */
bool GlShader::initialize() {

  // Setting some parameters.
  glClearDepthf(1.0); // Enables clearing of the depth buffer.

#if SDL_VIDEO_DRIVER_UIKIT || SDL_VIDEO_DRIVER_ANDROID || SDL_VIDEO_DRIVER_PANDORA
#define SDL_PROC(ret,func,params) data.func=func;
#else
#define SDL_PROC(ret,func,params) \
    do { \
        ctx.func = (ret(*)params)(SDL_GL_GetProcAddress(#func)); \
        if ( ! ctx.func ) { \
            Debug::warning(std::string("Couldn't load GLES2 function" #func)+  SDL_GetError()); \
            return false; \
        } \
    } while ( 0 );
#endif
#include "gles2funcs.h"
#undef SDL_PROC

  //Init screen quad
  screen_quad.add_quad(Rectangle(-1,-1,2,2),Rectangle(0,0,1,1),Color::white);

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

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);

  ctx.glGetError();

  // Load the shader.
  load();

  ctx.glUseProgram(program);
  // Set up constant uniform variables.
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

  ctx.glUseProgram(previous_program);
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
  GLenum gl_error(glGetError());

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
void GlShader::render(const SurfacePtr& surface, const Rectangle &region, const Size &dst_size) {
  //TODO compute mvp and uv_matrix here
  render(screen_quad,surface);
}

/**
 * \copydoc Shader::render
 */
void GlShader::render(const VertexArray& array, const SurfacePtr& texture, const glm::mat4 &mvp_matrix, const glm::mat3 &uv_matrix) {
  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);
  if(array.vertex_buffer == 0) {
    //Generate vertex-buffer
    ctx.glGenBuffers(1,&array.vertex_buffer);
  }
  GLint previous_buffer;
  ctx.glGetIntegerv(GL_ARRAY_BUFFER_BINDING,&previous_buffer);
  ctx.glBindBuffer(GL_ARRAY_BUFFER,array.vertex_buffer);
  if(array.buffer_dirty) {
    //Upload vertex buffer //TODO use glSubData if array size <= previous size
    ctx.glBufferData(GL_ARRAY_BUFFER,array.vertex_count()*sizeof(Vertex),array.data(),GL_DYNAMIC_DRAW);
    array.buffer_dirty = false;
  }

  const Size& output_size = Video::get_output_size();
  set_uniform_1i(Shader::TIME_NAME, System::now());
  set_uniform_2f(Shader::OUTPUT_SIZE_NAME, output_size.width, output_size.height);

  glm::mat4 mvp = mvp_matrix;
  ctx.glUniformMatrix4fv(get_uniform_location(Shader::MVP_MATRIX_NAME),1,GL_FALSE,glm::value_ptr(mvp));

  glm::mat3 uvm = uv_matrix;
  uvm = glm::scale(uvm,glm::vec2(1,-1));
  uvm = glm::translate(uvm,glm::vec2(0,-1));
  ctx.glUniformMatrix3fv(get_uniform_location(Shader::UV_MATRIX_NAME),1,GL_FALSE,glm::value_ptr(uvm));

  ctx.glEnableVertexAttribArray(position_location);
  ctx.glVertexAttribPointer(position_location,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,position));

  ctx.glEnableVertexAttribArray(tex_coord_location);
  ctx.glVertexAttribPointer(tex_coord_location,2,GL_FLOAT,GL_FALSE,sizeof(Vertex),(void*)offsetof(Vertex,texcoords));

  ctx.glEnableVertexAttribArray(color_location);
  ctx.glVertexAttribPointer(color_location,4,GL_UNSIGNED_BYTE,GL_TRUE,sizeof(Vertex),(void*)offsetof(Vertex,color));

  ctx.glActiveTexture(GL_TEXTURE0 + 0);  // Texture unit 0.
  SDL_GL_BindTexture(texture->get_internal_surface().get_texture(), nullptr, nullptr);

  for (const auto& kvp : uniform_textures) {
    const GLuint texture_unit = kvp.second.unit;
    ctx.glActiveTexture(GL_TEXTURE0 + texture_unit);
    SDL_GL_BindTexture(kvp.second.surface->get_internal_surface().get_texture(),nullptr,nullptr);
  }

  ctx.glDrawArrays((GLenum)array.get_primitive_type(),0,array.vertex_count());

  //ctx.glDisableVertexAttribArray(position_location);
  //ctx.glDisableVertexAttribArray(tex_coord_location);
  ctx.glDisableVertexAttribArray(color_location);

  ctx.glBindBuffer(GL_ARRAY_BUFFER,previous_buffer);
  ctx.glUseProgram(previous_program);
}

/**
 * \brief Returns the location of a uniform value in the shader program.
 * \param uniform_name Name of the uniform to find.
 * \return The uniform location or \c -1.
 */
GLint GlShader::get_uniform_location(const std::string& uniform_name) const {
  const auto& it = uniform_locations.find(uniform_name);
  if (it != uniform_locations.end()) {
    return it->second;
  }

  const GLint location = ctx.glGetUniformLocation(program, uniform_name.c_str());
  uniform_locations.insert(std::make_pair(uniform_name, location));
  return location;
}

/**
 * \copydoc Shader::set_uniform1f
 */
void GlShader::set_uniform_1b(const std::string& uniform_name, bool value) {

  const GLint location = get_uniform_location(uniform_name);
  if (location == -1) {
    return;
  }

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);
  ctx.glUniform1i(location, (value ? 1 : 0));
  ctx.glUseProgram(previous_program);
}

/**
 * \copydoc Shader::set_uniform_1i
 */
void GlShader::set_uniform_1i(const std::string& uniform_name, int value) {

  const GLint location = get_uniform_location(uniform_name);
  if (location == -1) {
    return;
  }

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);
  ctx.glUniform1i(location, value);
  ctx.glUseProgram(previous_program);
}

/**
 * \copydoc Shader::set_uniform_1f
 */
void GlShader::set_uniform_1f(const std::string& uniform_name, float value) {

  const GLint location = get_uniform_location(uniform_name);
  if (location == -1) {
    return;
  }

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);
  ctx.glUniform1f(location, value);
  ctx.glUseProgram(previous_program);
}

/**
 * \copydoc Shader::set_uniform_2f
 */
void GlShader::set_uniform_2f(const std::string& uniform_name, float value_1, float value_2) {

  const GLint location = get_uniform_location(uniform_name);
  if (location == -1) {
    return;
  }

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);
  ctx.glUniform2f(location, value_1, value_2);
  ctx.glUseProgram(previous_program);
}

/**
 * \copydoc Shader::set_uniform_3f
 */
void GlShader::set_uniform_3f(
    const std::string& uniform_name, float value_1, float value_2, float value_3) {

  const GLint location = get_uniform_location(uniform_name);
  if (location == -1) {
    return;
  }

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);
  ctx.glUniform3f(location, value_1, value_2, value_3);
  ctx.glUseProgram(previous_program);
}

/**
 * \copydoc Shader::set_uniform_4f
 */
void GlShader::set_uniform_4f(
    const std::string& uniform_name, float value_1, float value_2, float value_3, float value_4) {

  const GLint location = get_uniform_location(uniform_name);
  if (location == -1) {
    return;
  }

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);
  ctx.glUniform4f(location, value_1, value_2, value_3, value_4);
  ctx.glUseProgram(previous_program);
}

/**
 * \copydoc Shader::set_uniform_texture
 */
bool GlShader::set_uniform_texture(const std::string& uniform_name, const SurfacePtr& value) {
  const GLint location = get_uniform_location(uniform_name);

  if (location == -1) {
    // Not an error.
    return true;
  }

  auto it = uniform_textures.find(uniform_name);
  if(it != uniform_textures.end()) {
    it->second.surface = value;
    return true; //Nothing else to do
  }
  //else find a new texture unit

  GLint previous_program;
  ctx.glGetIntegerv(GL_CURRENT_PROGRAM, &previous_program);
  ctx.glUseProgram(program);

  int texture_unit = ++current_texture_unit;
  uniform_textures[uniform_name] = TextureUniform{value,(GLuint)texture_unit};

  ctx.glUniform1i(location, texture_unit);

  ctx.glUseProgram(previous_program);
  return true;
}

}
