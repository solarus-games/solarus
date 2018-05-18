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
#ifndef SOLARUS_SHADER_H
#define SOLARUS_SHADER_H

#include "solarus/core/Common.h"
#include "solarus/core/Debug.h"
#include "solarus/graphics/ShaderData.h"
#include "solarus/graphics/SurfacePtr.h"
#include "solarus/graphics/VertexArrayPtr.h"
#include "solarus/graphics/BlendMode.h"
#include "solarus/third_party/glm/mat4x4.hpp"
#include "solarus/third_party/glm/mat3x3.hpp"
#include "solarus/lua/ExportableToLua.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/lua/LuaTools.h"

#include "solarus/graphics/Drawable.h"

#include <string>

#ifdef SOLARUS_HAVE_OPENGL
#  include <SDL_opengl.h>
#else
#  include <SDL_opengles2.h>
#endif

namespace Solarus {

/**
 * \brief Represents a shader for a driver and sampler-independant uses.
 */
class Shader : public DrawProxy, public ExportableToLua {
  public:
    constexpr static const char* POSITION_NAME = "sol_vertex";
    constexpr static const char* TEXCOORD_NAME = "sol_tex_coord";
    constexpr static const char* COLOR_NAME = "sol_color";
    constexpr static const char* MVP_MATRIX_NAME = "sol_mvp_matrix";
    constexpr static const char* UV_MATRIX_NAME = "sol_uv_matrix";
    constexpr static const char* TEXTURE_NAME = "sol_texture";
    constexpr static const char* INPUT_SIZE_NAME = "sol_input_size";
    constexpr static const char* OUTPUT_SIZE_NAME = "sol_output_size";
    constexpr static const char* TIME_NAME = "sol_time";

    explicit Shader(const std::string& shader_id);
    virtual ~Shader();

    bool is_valid() const;
    std::string get_error() const;

    const std::string& get_id() const;
    const ShaderData& get_data() const;

    std::string get_vertex_source() const;
    std::string get_fragment_source() const;

    virtual std::string default_vertex_source() const = 0;
    virtual std::string default_fragment_source() const = 0;

    virtual void set_uniform_1b(
        const std::string& uniform_name, bool value);  // TODO make pure virtual
    virtual void set_uniform_1i(
        const std::string& uniform_name, int value);
    virtual void set_uniform_1f(
        const std::string& uniform_name, float value);
    virtual void set_uniform_2f(
        const std::string& uniform_name, float value_1, float value_2);
    virtual void set_uniform_3f(
        const std::string& uniform_name, float value_1, float value_2, float value_3);
    virtual void set_uniform_4f(
        const std::string& uniform_name, float value_1, float value_2, float value_3, float value_4);
    virtual bool set_uniform_texture(const std::string& uniform_name, const SurfacePtr& value);

    void render(const Surface &surface, const Rectangle &region, const Size &dst_size, const Point &dst_position = Point(), bool flip_y = false);
    virtual void draw(Surface& dst_surface, const Surface &src_surface, const DrawInfos &infos) const override;
    /**
     * @brief render the given vertex array with this shader, passing the texture and matrices as uniforms
     * @param array a vertex array
     * @param texture a valid surface
     * @param mvp_matrix model view projection matrix
     * @param uv_matrix uv_matrix
     */
    virtual void render(const VertexArray &array, const Surface &texture, const glm::mat4& mvp_matrix = glm::mat4(), const glm::mat3& uv_matrix = glm::mat3()) = 0;

    const std::string& get_lua_type_name() const override;
  protected:
    void set_valid(bool valid);
    void set_error(const std::string& error);
    void set_data(const ShaderData& data);
    virtual void load();  // TODO make pure virtual
    static VertexArray screen_quad; /**< The quad used to draw surfaces with shaders*/
  private:

    const std::string shader_id;  /**< The id of the shader (filename without extension). */
    ShaderData data;              /**< The loaded shader data file. */
    bool valid;                   /**< \c true if the compilation succedeed. */
    std::string error;            /**< Error message of the last operation if any. */
};

}

#endif
