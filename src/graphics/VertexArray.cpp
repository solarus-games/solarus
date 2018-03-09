#include "solarus/graphics/VertexArray.h"

#include "solarus/graphics/Surface.h"
#include "solarus/graphics/RenderTexture.h"
#include "solarus/core/Debug.h"
#include <cstddef> //offsetof

namespace Solarus {

Vertex& VerticeView::operator [](size_t index) {
  array.buffer_dirty = true;
  return array[offset+index];
}

const Vertex& VerticeView::operator [](size_t index) const {
  return array[offset+index];
}

Vertex& VerticeView::at(size_t index) {
  return (*this)[index];
}

const Vertex& VerticeView::at(size_t index) const {
  return (*this)[index];
}

void VerticeView::set_4quad_offset(const Rectangle& targetQuad,const Point& offset, const Rectangle& uvs) {
  VerticeView& tlq = *this; //All 4 quads as views
  VerticeView blq = sub_view(6);
  VerticeView brq = sub_view(12);
  VerticeView trq = sub_view(18);
  {
    Rectangle moved = targetQuad;
    Rectangle clamped;
    moved.add_xy(offset);
    clamped = moved & targetQuad;
    tlq.update_quad_positions(clamped);

    Rectangle uvmask = uvs;
    uvmask.add_xy(-offset);
    Rectangle newv = uvs & uvmask;
    tlq.update_quad_uvs(newv);
  }
  {
    Rectangle moved = targetQuad;
    Rectangle clamped;
    moved.add_xy(offset);
    moved.add_y(-targetQuad.get_height());
    clamped = moved & targetQuad;
    blq.update_quad_positions(clamped);

    Rectangle uvmask = uvs;
    uvmask.add_xy(-offset);
    uvmask.add_y(uvs.get_height());
    Rectangle newv = uvs & uvmask;
    blq.update_quad_uvs(newv);
  }
  {
    Rectangle moved = targetQuad;
    Rectangle clamped;
    moved.add_xy(offset);
    moved.add_xy(-targetQuad.get_size().width,-targetQuad.get_size().height);
    clamped = moved & targetQuad;
    brq.update_quad_positions(clamped);

    Rectangle uvmask = uvs;
    uvmask.add_xy(-offset);
    uvmask.add_xy(uvs.get_size().width,uvs.get_size().height);
    Rectangle newv = uvs & uvmask;
    brq.update_quad_uvs(newv);
  }
  {
    Rectangle moved = targetQuad;
    Rectangle clamped;
    moved.add_xy(offset);
    moved.add_x(-targetQuad.get_width());
    clamped = moved & targetQuad;
    trq.update_quad_positions(clamped);

    Rectangle uvmask = uvs;
    uvmask.add_xy(-offset);
    uvmask.add_x(uvs.get_width());
    Rectangle newv = uvs & uvmask;
    trq.update_quad_uvs(newv);
  }
}

VerticeView VerticeView::sub_view(off_t a_offset) const {
  return VerticeView(array,offset+a_offset,size-a_offset);
}

void VerticeView::update_quad_uvs(const Rectangle& uvs) {
  array.buffer_dirty = true;
  glm::vec2 u1 = uvs.get_top_left();
  glm::vec2 u2 = uvs.get_bottom_left();
  glm::vec2 u3 = uvs.get_bottom_right();
  glm::vec2 u4 = uvs.get_top_right();
  at(0).texcoords = u1;
  at(1).texcoords = u2;
  at(2).texcoords = u3;
  at(3).texcoords = u3;
  at(4).texcoords = u4;
  at(5).texcoords = u1;
}

void VerticeView::update_quad_positions(const Rectangle& pos)
{
  array.buffer_dirty = true;
  glm::vec2 v1 = pos.get_top_left();
  glm::vec2 v2 = pos.get_bottom_left();
  glm::vec2 v3 = pos.get_bottom_right();
  glm::vec2 v4 = pos.get_top_right();
  at(0).position = v1;
  at(1).position = v2;
  at(2).position = v3;
  at(3).position = v3;
  at(4).position = v4;
  at(5).position = v1;
}

void VerticeView::update_quad_position(const Point& position) {
  array.buffer_dirty = true;
  glm::vec2 old = at(0).position;
  glm::vec2 diff = (glm::vec2)position - old;
  for(int i = 0; i < 6; i++) {
    at(i).position += diff;
  }
}

size_t VerticeView::get_size() const {return size;}


VertexArray::VertexArray(PrimitiveType type):
  type(type)
{

}

VertexArray::VertexArray(PrimitiveType type,size_t vertex_count):
  vertices(vertex_count),type(type)
{
}

VertexArrayPtr VertexArray::create(PrimitiveType type) {
  return std::make_shared<VertexArray>(type);
}

VertexArrayPtr VertexArray::create(PrimitiveType type,size_t vertex_count) {
  return std::make_shared<VertexArray>(type,vertex_count);
}

void VertexArray::set_primitive_type(PrimitiveType type) {
  this->type = type;
}

PrimitiveType VertexArray::get_primitive_type() const {
  return type;
}

Vertex* VertexArray::data() {
  return vertices.data();
}

VerticeView VertexArray::add_quad(const Rectangle& rect, const Rectangle& uvs, const Color& color) {
  buffer_dirty = true;
  Vertex v1(rect.get_top_left(),
            color,
            uvs.get_top_left());
  Vertex v2(rect.get_bottom_left(),
            color,
            uvs.get_bottom_left());
  Vertex v3(rect.get_bottom_right(),
            color,
            uvs.get_bottom_right());
  Vertex v4(rect.get_top_right(),
            color,
            uvs.get_top_right());
  add_vertex(v1);
  add_vertex(v2);
  add_vertex(v3);
  add_vertex(v3);
  add_vertex(v4);
  add_vertex(v1);
  return make_view(6);
}

VerticeView VertexArray::make_view(size_t size) {
  return VerticeView(*this,vertex_count()-size,size);
}

const Vertex* VertexArray::data() const {
  return vertices.data();
}

void VertexArray::add_vertex(const Vertex& v) {
  vertices.push_back(v);
}

size_t VertexArray::vertex_count() const {
  return vertices.size();
}

Vertex& VertexArray::operator [](size_t index) {
  return vertices.at(index);
}

void VertexArray::draw(const SurfacePtr& dst_surface,const Point& dst_position, const SurfacePtr& texture) const {
  //TODO
}
const Vertex& VertexArray::operator [](size_t index) const {
  return vertices.at(index);
}


}
