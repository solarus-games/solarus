/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUS_VIDEO_H
#define SOLARUS_VIDEO_H

#include "solarus/Common.h"
#include "solarus/lowlevel/Point.h"
#include "solarus/lowlevel/SurfacePtr.h"
#include <vector>
#include <string>

struct SDL_PixelFormat;
struct SDL_Renderer;
struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
struct SDL_PixelFormat;

namespace Solarus {

class Arguments;
class Rectangle;
class Size;
class VideoMode;

/**
 * \brief Draws the window and handles the video mode.
 */
class Video {

  public:

    static void initialize(const Arguments& args);
    static void quit();
    static bool is_initialized();

    static SDL_Window* get_window();
    static SDL_Renderer* get_renderer();

    static SDL_Texture* get_render_target();
    static SDL_PixelFormat* get_pixel_format();
    static bool is_acceleration_enabled();
    static const std::string& get_rendering_driver_name();
    static void show_window();

    static const VideoMode& get_video_mode();
    static std::vector<const VideoMode*> get_video_modes();
    static bool is_mode_supported(const VideoMode& mode);
    static bool set_video_mode(const VideoMode& mode);
    static bool set_video_mode(const VideoMode& mode, bool fullscreen);
    static void set_default_video_mode();
    static void switch_video_mode();

    static const VideoMode* get_video_mode_by_name(const std::string& mode_name);

    static bool is_fullscreen();
    static void set_fullscreen(bool fullscreen);

    static bool is_cursor_visible();
    static void set_cursor_visible(bool cursor_visible);

    static std::string get_window_title();
    static void set_window_title(const std::string& window_title);

    static bool parse_size(const std::string& size_string, Size& size);

    static const Size& get_quest_size();
    static void get_quest_size_range(
        Size& normal_quest_size,
        Size& min_quest_size,
        Size& max_quest_size);
    static void set_quest_size_range(
        const Size& normal_quest_size,
        const Size& min_quest_size,
        const Size& max_quest_size);

    static Size get_window_size();
    static void set_window_size(const Size& size);
    static void reset_window_size();

    static Rectangle get_viewport();
    static Point window_to_quest_coordinates(const Point& window_xy);
    static bool renderer_to_quest_coordinates(const Point& renderer_xy, Point& quest_xy);

    static void render(const SurfacePtr& quest_surface);

};

}

#endif

