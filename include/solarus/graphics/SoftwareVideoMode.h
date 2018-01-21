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
#ifndef SOLARUS_SOFTWARE_VIDEO_MODE_H
#define SOLARUS_SOFTWARE_VIDEO_MODE_H

#include "solarus/core/Common.h"
#include "solarus/core/Size.h"
#include <memory>
#include <string>

namespace Solarus {

class SoftwarePixelFilter;
class Shader;

/**
 * \brief Represents a method to display the quest content on the screen.
 *
 * The video mode may include a scaling algorithm.
 *
 * \deprecated Software video modes are deprecated since Solarus 1.6.
 * The new recommended way is to use shaders.
 */
class SoftwareVideoMode {

  public:

    SoftwareVideoMode(
        const std::string& name,
        const Size& initial_window_size,
        std::unique_ptr<SoftwarePixelFilter> software_filter
    );

    const std::string& get_name() const;
    const Size& get_initial_window_size() const;
    const SoftwarePixelFilter* get_software_filter() const;

  private:

    std::string name;              /**< Lua name of this video mode. */
    Size initial_window_size;      /**< Default size of the window when
                                    * selecting this video mode. */

    std::unique_ptr<SoftwarePixelFilter>
        software_filter;           /**< Software scaling pixel filter to use or nullptr. */

};

}

#endif

