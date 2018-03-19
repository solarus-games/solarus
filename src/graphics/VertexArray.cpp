#include "solarus/graphics/VertexArray.h"

#include "solarus/graphics/Surface.h"
#include "solarus/graphics/RenderTexture.h"
#include "solarus/core/Debug.h"
#include <cstddef> //offsetof

namespace Solarus {

/**
 * @brief Access vertex at index, relative to view basis
 * @param index
 * @return Vertex reference
 */
Vertex& VerticeView::operator [](size_t index) {
  array.buffer_dirty = true;
  return array[offset+index];
}

/**
 * @brief Access vertex at index, relative to view basis
 * @param index
 * @return Vertex reference
 */
const Vertex& VerticeView::operator [](size_t index) const {
  return array[offset+index];
}

/**
 * @brief Access vertex at index, relative to view basis
 * @param index
 * @return Vertex reference
 */
Vertex& VerticeView::at(size_t index) {
  return (*this)[index];
}

/**
 * @brief Access vertex at index, relative to view basis
 * @param index
 * @return Vertex reference
 */
const Vertex& VerticeView::at(size_t index) const {
  return (*this)[index];
}

/**
 * @brief offset a quad in a target window
 * @param targetQuad target window where the quad is clipped
 * @param offset the offset by wich the quad is displaced
 * @param uvs uvs of the un clipped quad
 */
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

/**
 * @brief create a sub view from this one
 * @param a_offset basis offset
 * @return VerticeView
 */
VerticeView VerticeView::sub_view(off_t a_offset) const {
  return VerticeView(array,offset+a_offset,size-a_offset);
}

/**
 * @brief update uvs of a quad pointed by this view
 * @param uvs new uv coordinates
 */
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

/**
 * @brief update position of a quad pointed by this view
 * @param pos new 'position', rectangle here allow to change shape
 */
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

/**
 * @brief update position of a quad pointed by this view
 * @param position new position
 */
void VerticeView::update_quad_position(const Point& position) {
  array.buffer_dirty = true;
  glm::vec2 old = at(0).position;
  glm::vec2 diff = (glm::vec2)position - old;
  for(int i = 0; i < 6; i++) {
    at(i).position += diff;
  }
}

/**
 * @brief get view size
 * @return size
 */
size_t VerticeView::get_size() const {return size;}

/**
 * @brief Create empty vertex array with type
 * @param type Primitive type
 */
VertexArray::VertexArray(PrimitiveType type):
  type(type)
{

}

/**
 * @brief Create a vertex array with given type and size
 * @param type primitive type
 * @param vertex_count size of the array
 */
VertexArray::VertexArray(PrimitiveType type,size_t vertex_count):
  vertices(vertex_count),type(type)
{
}

/**
 * @brief create a vertex arrayptr
 * @param type primitive type
 * @return shared ptr to new array
 */
VertexArrayPtr VertexArray::create(PrimitiveType type) {
  return std::make_shared<VertexArray>(type);
}

/**
 * @brief create a vertex arrayptr
 * @param type primitive type
 * @param vertex_count size of the array
 * @return shared ptr to new array
 */
VertexArrayPtr VertexArray::create(PrimitiveType type,size_t vertex_count) {
  return std::make_shared<VertexArray>(type,vertex_count);
}

/**
 * @brief set primitive type for this array
 * @param type
 */
void VertexArray::set_primitive_type(PrimitiveType type) {
  this->type = type;
}

/**
 * @brief set primitive type for this array
 * @param type
 */
PrimitiveType VertexArray::get_primitive_type() const {
  return type;
}

/**
 * @brief VertexArray::data
 * @return
 */
Vertex* VertexArray::data() {
  return vertices.data();
}

/**
 * @brief add a quad (6 vertices) to the array
 * @param rect quad position and size
 * @param uvs quad texture coordinates
 * @param color quad color
 * @return a vertice view to the quad
 */
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

/**
 * @brief make a view at the end of the array
 * @param size size of the view
 * @return the created view
 */
VerticeView VertexArray::make_view(size_t size) {
  return VerticeView(*this,vertex_count()-size,size);
}

/**
 * @brief get pointer to data contiguous in memory
 * @return pointer to vector storage
 */
const Vertex* VertexArray::data() const {
  return vertices.data();
}

/**
 * @brief add a single vertex at the end of array
 * @param v vertex to add
 */
void VertexArray::add_vertex(const Vertex& v) {
  vertices.push_back(v);
}

/**
 * @brief get vertex count
 * @return array size
 */
size_t VertexArray::vertex_count() const {
  return vertices.size();
}

/**
 * @brief get vertex at given index
 * @param index index of the vertex in the array
 * @return vertex reference
 */
Vertex& VertexArray::operator [](size_t index) {
  return vertices.at(index);
}

/**
 * @brief get vertex at given index
 * @param index index of the vertex in the array
 * @return vertex reference
 */
const Vertex& VertexArray::operator [](size_t index) const {
  return vertices.at(index);
}

}
