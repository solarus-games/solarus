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
#ifndef SOLARUS_GL_2D_SHADER_H
#define SOLARUS_GL_2D_SHADER_H

#include "solarus/core/Common.h"

#include "solarus/graphics/Shader.h"
#include <SDL.h>
#include <string>

#  include "SDL_opengles2.h"


namespace Solarus {

/**
 * \brief Represents a GLSL shader and implement it using the post-deprecation way.
 *
 * This class basically encapsulates a GLSL vertex and fragment shader.
 * The purpose is to support both desktop and ES OpenGLs in a single class.
 * Supports OpenGL 3.0 and superior and OpenGL ES2.
 */
class GlShader : public Shader {

  public:

  static bool initialize();

  explicit GlShader(const std::string& shader_id);
  ~GlShader();

  void set_uniform_1b(
      const std::string& uniform_name, bool value) override;
  void set_uniform_1i(
      const std::string& uniform_name, int value) override;
  void set_uniform_1f(
      const std::string& uniform_name, float value) override;
  void set_uniform_2f(
      const std::string& uniform_name, float value_1, float value_2) override;
  void set_uniform_3f(
      const std::string& uniform_name, float value_1, float value_2, float value_3) override;
  void set_uniform_4f(
      const std::string& uniform_name, float value_1, float value_2, float value_3, float value_4) override;
  bool set_uniform_texture(const std::string& uniform_name, const SurfacePtr& value) override;

  void render(const VertexArray &array, const SurfacePtr &texture, const glm::mat4& mvp_matrix = glm::mat4(), const glm::mat3& uv_matrix = glm::mat3()) override;

  std::string default_vertex_source() const override;
  std::string default_fragment_source() const override;
protected:
  void load() override;
private:

  void check_gl_error();

  struct TextureUniform{
    SurfacePtr surface;
    GLuint unit;
  };

  GLuint create_shader(unsigned int type, const char* source);
  static void set_rendering_settings();
  GLint get_uniform_location(const std::string& uniform_name) const;

  void render(const SurfacePtr& surface, const Rectangle &region, const Size& dst_size, const Point& dst_position) override;

  GLuint program;                         /**< The program which bind the vertex and fragment shader. */
  GLuint vertex_shader;                   /**< The vertex shader. */
  GLuint fragment_shader;                 /**< The fragment shader. */
  GLint position_location;                     /**< The location of the position attrib. */
  GLint tex_coord_location;                    /**< The location of the tex_coord attrib. */
  GLint color_location;                        /**< The location of the color attrib. */
  mutable std::map<std::string, GLint>
      uniform_locations;                       /**< Cache of uniform locations. */
  mutable std::map<std::string, TextureUniform>
      uniform_textures;                        /**< Uniform texture value of surfaces. */
  GLuint current_texture_unit = 0;
  static VertexArray screen_quad;
};

}

#endif
