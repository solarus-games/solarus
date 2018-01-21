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
#ifndef SOLARUS_VIDEO_H
#define SOLARUS_VIDEO_H

#include "solarus/core/Common.h"
#include "solarus/core/Point.h"
#include "solarus/graphics/ShaderPtr.h"
#include "solarus/graphics/SurfacePtr.h"
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
class SoftwareVideoMode;

/**
 * \brief Draws the window and handles the video mode.
 */
namespace Video {

    void initialize(const Arguments& args);
    void quit();
    bool is_initialized();

    SDL_Window* get_window();
    SDL_Renderer* get_renderer();

    SDL_Texture* get_render_target();
    SDL_PixelFormat* get_pixel_format();
    const std::string& get_rendering_driver_name();
    void show_window();

    bool is_fullscreen();
    void set_fullscreen(bool fullscreen);

    bool is_cursor_visible();
    void set_cursor_visible(bool cursor_visible);

    std::string get_window_title();
    void set_window_title(const std::string& window_title);

    const ShaderPtr& get_shader();
    void set_shader(const ShaderPtr& shader);

    const SoftwareVideoMode& get_video_mode();
    std::vector<const SoftwareVideoMode*> get_video_modes();
    bool is_mode_supported(const SoftwareVideoMode& mode);
    bool set_video_mode(const SoftwareVideoMode& mode);
    void set_default_video_mode();
    void switch_video_mode();
    const SoftwareVideoMode* get_video_mode_by_name(const std::string& mode_name);

    bool parse_size(const std::string& size_string, Size& size);

    const Size& get_quest_size();
    void get_quest_size_range(
        Size& normal_quest_size,
        Size& min_quest_size,
        Size& max_quest_size);
    void set_quest_size_range(
        const Size& normal_quest_size,
        const Size& min_quest_size,
        const Size& max_quest_size);

    Size get_window_size();
    void set_window_size(const Size& size);
    void reset_window_size();

    Size get_output_size();
    Size get_output_size_no_bars();

    Rectangle get_viewport();
    Point window_to_quest_coordinates(const Point& window_xy);
    bool renderer_to_quest_coordinates(const Point& renderer_xy, Point& quest_xy);

    void render(const SurfacePtr& quest_surface);

}  // namespace Video

}  // namespace Solarus

#endif

