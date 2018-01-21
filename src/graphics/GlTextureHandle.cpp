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
#include "solarus/graphics/GlTextureHandle.h"

namespace Solarus {

/**
 * \brief Creates an empty OpenGL texture handle.
 */
GlTextureHandle::GlTextureHandle() :
  texture(0) {

}

/**
 * \brief Destroys the texture.
 */
GlTextureHandle::~GlTextureHandle() {

  clear();
}

/**
 * \brief Creates a wrapper for an existing texture handle.
 * \param texture An OpenGL texture handle or \c 0.
 */
GlTextureHandle::GlTextureHandle(GLuint texture) :
  texture(texture) {

}

/**
 * \brief Move constructor.
 * \param other A texture handle.
 */
GlTextureHandle::GlTextureHandle(GlTextureHandle&& other) :
  texture(other.texture) {
  other.texture = 0;
}

/**
 * \brief Move assignment operator.
 * \param other The object to move.
 * \return This object.
 */
GlTextureHandle& GlTextureHandle::operator=(GlTextureHandle&& other) {

  clear();
  this->texture = other.texture;
  other.texture = 0;
  return *this;
}

/**
 * \brief Returns whether this wrapper is empty.
 * \return \c true if there is no OpenGL texture in this object.
 */
bool GlTextureHandle::is_empty() const {

  return texture == 0;
}

/**
 * \brief Returns the wrapped OpenGL texture handle.
 * \return The OpenGL texture handle or \c 0.
 */
GLuint GlTextureHandle::get() const {
  return texture;
}

/**
 * \brief Destroys the texture if any and makes this wrapper empty.
 */
void GlTextureHandle::clear() {

  glDeleteTextures(1, &texture);
  texture = 0;
}

}
