 /*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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
#include "lowlevel/Shader.h"
#include "lowlevel/PixelFilter.h"

namespace solarus {

/**
 * \brief Creates a video mode with the specified properties.
 *
 * \c software_filter and \c hardware_filter cannot be both set.
 *
 * \param name Lua name of the video mode.
 * \param window_size Final size of the window when selecting this video mode.
 * \param software_filter Software filter to apply to the quest image or NULL.
 * \param hardware_filter Scaling shader to apply to the quest image or NULL.
 */
VideoMode::VideoMode(
    const std::string& name,
    const Rectangle& window_size,
    PixelFilter* software_filter,
    Shader* hardware_filter
):
   name(name),
   window_size(window_size),
   software_filter(software_filter),
   hardware_filter(hardware_filter) {

   Debug::check_assertion(software_filter == NULL || hardware_filter == NULL,
       "Video mode can have at most one filter");
}

/**
 * \brief Destructor.
 */
VideoMode::~VideoMode() {

  if (software_filter != NULL) {
    delete software_filter;
  }
  if (hardware_filter) {
    delete hardware_filter;
  }
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
 * \brief Returns the software filter applied to the quest image in this mode.
 * \return Software filter or NULL.
 */
PixelFilter* VideoMode::get_software_filter() const {
  return software_filter;
}

/**
 * \brief Returns the hardware filter applied to the quest image in this mode.
 * \return Hardware filter or NULL.
 */
Shader* VideoMode::get_hardware_filter() const {
  return hardware_filter;
}

}

