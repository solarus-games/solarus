#pragma once

#include "solarus/core/Rectangle.h"
#include "solarus/graphics/Color.h"
#include "solarus/graphics/SurfacePtr.h"
#include "solarus/graphics/ShaderPtr.h"
#include "solarus/graphics/VertexArrayPtr.h"

#include <memory>
#include <vector>

#include <glm/vec2.hpp>

#include <SDL_opengles2.h>

namespace Solarus {

struct Vertex {
  Vertex(const glm::vec2& position,
         const Color& color,
         const glm::vec2& texcoords
         ) :
    position(position),
    texcoords(texcoords),
    color(color){}
  Vertex() = default;
  glm::vec2 position;
  glm::vec2 texcoords;
  Color color;
};

enum PrimitiveType {
    POINTS = GL_POINTS,
    LINES = GL_LINES,
    LINE_STRIP = GL_LINE_STRIP,
    TRIANGLES = GL_TRIANGLES,
    TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
    TRIANGLE_FAN = GL_TRIANGLE_FAN
    //Don't put quad as it is deprecated
};

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
    void draw(const SurfacePtr &dst_surface, const Point& dst_position, const SurfacePtr &texture) const;
    const Vertex& operator [](size_t index) const;
private:
    std::vector <Vertex> vertices;
    mutable GLuint vertex_buffer;
    PrimitiveType type;
    mutable bool buffer_dirty = true;
};



}
