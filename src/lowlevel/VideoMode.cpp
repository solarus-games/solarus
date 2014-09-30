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
#include "solarus/lowlevel/VideoMode.h"
#include "solarus/lowlevel/PixelFilter.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/shaders/ShaderContext.h"

namespace Solarus {

/**
 * \brief Creates a video mode with the specified properties.
 *
 * \c software_filter and \c hardware_filter cannot be both set.
 *
 * \param name Lua name of the video mode.
 * \param initial_window_size Default size of the window when selecting this video mode.
 * \param software_filter Software filter to apply to the quest image or nullptr.
 * \param hardware_filter Scaling shader to apply to the quest image or nullptr.
 */
VideoMode::VideoMode(
    const std::string& name,
    const Size& initial_window_size,
    std::unique_ptr<PixelFilter> software_filter,
    std::unique_ptr<Shader> hardware_filter
):
   name(name),
   initial_window_size(initial_window_size),
   software_filter(std::move(software_filter)),
   hardware_filter(std::move(hardware_filter)) {

   Debug::check_assertion(software_filter == nullptr || hardware_filter == nullptr,
       "Video mode can have at most one filter");
}

/**
 * \brief Returns the Lua name of this video mode.
 * \return The video mode name.
 */
const std::string& VideoMode::get_name() const {
  return name;
}

/**
 * \brief Returns the default size of the window when selecting this mode.
 * \return Default size of the window when selecting this video mode.
 */
const Size& VideoMode::get_initial_window_size() const {
  return initial_window_size;
}

/**
 * \brief Returns the software filter applied to the quest image in this mode.
 * \return Software filter or nullptr.
 */
const PixelFilter* VideoMode::get_software_filter() const {
  return software_filter.get();
}

/**
 * \brief Returns the hardware filter applied to the quest image in this mode.
 * \return Hardware filter or nullptr.
 */
const Shader* VideoMode::get_hardware_filter() const {
  return hardware_filter.get();
}

}
