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

#ifdef SOLARUS_HAVE_OPENGLES2
#  include "SDL_opengles2.h"
#endif


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

#ifdef SOLARUS_HAVE_OPENGLES2
    static bool initialize();

    explicit GlShader(const std::string& shader_id);
    void render(const VertexArrayPtr& vertex_array) override;
    std::string default_vertex_source() const override;
    std::string default_fragment_source() const override;
    ~GlShader();

  protected:

    void load() override;

  private:

    GLuint create_shader(GLenum type, const char* source);
    static void check_gl_error();

    void render(const SurfacePtr& quest_surface) override;

    GLuint program;                         /**< The program which bind the vertex and fragment shader. */
    GLuint vertex_shader;                   /**< The vertex shader. */
    GLuint fragment_shader;                 /**< The fragment shader. */
    GLint  position_location;               /**< The location of the position attrib. */
    GLint  tex_coord_location;              /**< The location of the tex_coord attrib */
    GLint  color_location;                  /**< The location of the color attrib */
#else
  std::string default_vertex_source() const override{}
  std::string default_fragment_source() const override{}
  void render(const VertexArrayPtr& array, const SurfacePtr &texture, const Point& dst_position) override{}
    static bool initialize() { return false; }
    explicit GlShader(const std::string& shader_id): Shader(shader_id) {}
#endif
};

}

#endif
