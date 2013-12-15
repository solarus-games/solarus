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
#ifndef SOLARUS_VIDEO_MODE_H
#define SOLARUS_VIDEO_MODE_H

#include "Common.h"
#include "lowlevel/Rectangle.h"

/**
 * \brief Represents a method to display the quest content on the screen.
 *
 * The video mode may include a scaling algorithm.
 */
class VideoMode {

  public:

    VideoMode(
        const std::string& name,
        const Rectangle& window_size,
        Shader* shader);
    ~VideoMode();

    const std::string& get_name() const;
    const Rectangle& get_window_size() const;
    Shader* get_shader() const;

  private:

    VideoMode(const VideoMode& other);

    const std::string name;            /**< Lua name of this video mode. */
    Rectangle window_size;             /**< Final size of the window when selecting this video mode. */
    Shader* shader;                    /**< Shader to use or NULL. */

};

#endif

