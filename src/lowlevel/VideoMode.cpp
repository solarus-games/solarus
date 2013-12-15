 /*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/VideoMode.h"

/**
 * \brief Creates a video mode with the specified properties.
 * \param name Lua name of the video mode.
 * \param window_size Final size of the window when selecting this video mode.
 * \param shader Shader to apply to the quest image or NULL.
 */
VideoMode::VideoMode(
    const std::string& name,
    const Rectangle& window_size,
    Shader* shader
):
   name(name),
   window_size(window_size),
   shader(shader) {

}

/**
 * \brief Destructor.
 */
VideoMode::~VideoMode() {

}

/**
 * \brief Returns the Lua name of this video mode.
 * \return The video mode name.
 */
const std::string& VideoMode::get_name() const {
  return name;
}

/**
 * \brief Returns the size of the window when selecting this mode.
 * \return Size of the window when selecting this video mode.
 */
const Rectangle& VideoMode::get_window_size() const {
  return window_size;
}

/**
 * \brief Returns the shader applied to the quest image in this mode.
 * \return Shader to apply to the quest image or NULL.
 */
Shader* VideoMode::get_shader() const {
  return shader;
}

