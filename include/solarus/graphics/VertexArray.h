#pragma once

#include "solarus/core/Rectangle.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/SurfacePtr.h"
#include "solarus/graphics/ShaderPtr.h"
#include "solarus/graphics/VertexArrayPtr.h"

#include <memory>
#include <vector>
#include <glm/vec2.hpp>

#ifdef SOLARUS_HAVE_OPENGL
#  include <SDL_opengl.h>
#else
#  include <SDL_opengles2.h>
#endif

namespace Solarus {

/**
 * @brief Solarus 2D vertex structure
 *
 * Vertex is used as assembly data for every interaction
 * with shaders and opengl
 */
struct Vertex {
  Vertex(const glm::vec2& position,
         const Color& color,
         const glm::vec2& texcoords
         ) :
    position(position),
    texcoords(texcoords),
    color(color){}
  Vertex() = default;
  glm::vec2 position; /**< vertex position*/
  glm::vec2 texcoords;/**< uv textures coordinates*/
  Color color; /**< vertex rgba color */
};

/**
 * @brief Primitive type, specifying how vertex should be drawn
 */
enum PrimitiveType {
    POINTS = GL_POINTS, /**< Draw each vertex as a single pixel*/
    LINES = GL_LINES, /**< Draw each couple of vertex as a line*/
    LINE_STRIP = GL_LINE_STRIP, /**< Draw a line through all vertices*/
    TRIANGLES = GL_TRIANGLES, /**< Draw a triangle with each vertices trio */
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP, /**< Draw a triangle strip */
    TRIANGLE_FAN = GL_TRIANGLE_FAN /**< Draw a triangle fan*/
    //Don't put quad as it is deprecated
};

/**
 * @brief View on a Vertex array, allowing to modify it
 *
 * VerticeView is used to manipulate parts of a vertex array
 */
class VerticeView
{
public:
    VerticeView(VertexArray& array, off_t offset, size_t size):
        array(array),offset(offset),size(size){}

    void update_quad_uvs(const Rectangle& uvs);
    void update_quad_position(const Point& position);
    void update_quad_positions(const Rectangle& pos);
    void set_4quad_offset(const Rectangle& targetQuad,const Point& offset, const Rectangle& uvs);
    inline Vertex& operator [](size_t index);
    Vertex& at(size_t index);
    const Vertex& at(size_t index) const;
    inline const Vertex& operator [](size_t index) const;
    VerticeView sub_view(off_t offset) const;
    size_t get_size() const;
private:
    VertexArray& array;
    off_t offset;
    size_t size;
};

/**
 * @brief Solarus VertexArray
 *
 * A vertex array is a set of vertices that can be drawn
 */
class VertexArray
{
  friend class GlShader;
  friend class GlArbShader;
  friend class VerticeView;
public:
    VertexArray(PrimitiveType type = TRIANGLES);
    VertexArray(PrimitiveType type,size_t vertex_count);
    static VertexArrayPtr create(PrimitiveType type = TRIANGLES);
    static VertexArrayPtr create(PrimitiveType type,size_t vertex_count);
    void set_primitive_type(PrimitiveType type);
    PrimitiveType get_primitive_type() const;
    Vertex* data();
    const Vertex* data() const;
    void add_vertex(const Vertex& v);
    VerticeView add_quad(const Rectangle& rect, const Rectangle& uvs, const Color &color);
    VerticeView make_view(size_t size);
    size_t vertex_count() const;
    Vertex& operator [](size_t index);
    const Vertex& operator [](size_t index) const;
private:
    std::vector <Vertex> vertices; /**< actual vertices storage*/
    mutable GLuint vertex_buffer; /**< buffer object where vertices are uploaded in GPU*/
    PrimitiveType type; /**< Primitive type the VertexArray should be drawn with*/
    mutable bool buffer_dirty = true; /**< dirty bit stating that array needs to be reuploaded*/
};



}
