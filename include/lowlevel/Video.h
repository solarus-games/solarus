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
#ifndef SOLARUS_VIDEO_H
#define SOLARUS_VIDEO_H

#include "Common.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/Shader.h"
#include <vector>
#include <map>
#include <string>

struct SDL_PixelFormat;
struct SDL_Renderer;

/**
 * \brief Draws the window and handles the video mode.
 */
class Video {

  public:

    static void initialize(int argc, char** argv);
    static void quit();

    static SDL_Window* get_window();
    static SDL_Renderer* get_renderer();

    static SDL_PixelFormat* get_pixel_format();
    static bool is_acceleration_enabled();
    static const std::string& get_rendering_driver_name();
    static void create_window();
    static void show_window();
  
    static Rectangle& get_viewport();
    static void update_viewport();
  
    static void initialize_video_modes(bool allow_shaded_modes);
    static const VideoMode& get_video_mode();
    static std::vector<const VideoMode*> get_video_modes();
    static bool is_mode_supported(const VideoMode& mode);
    static bool set_video_mode(const VideoMode& mode);
    static void set_default_video_mode();
    static void switch_video_mode();

    static const std::string& get_video_mode_name(const VideoMode& mode);
    static const VideoMode* get_video_mode_by_name(const std::string& mode_name);

    static bool is_fullscreen();
    static void set_fullscreen(bool fullscreen);
    static void switch_fullscreen();

    static const std::string get_window_title();
    static void set_window_title(const std::string& window_title);

    static bool parse_size(const std::string& size_string, Rectangle& size);

    static const Rectangle& get_quest_size();
    static void get_quest_size_range(
        Rectangle& normal_quest_size,
        Rectangle& min_quest_size,
        Rectangle& max_quest_size);
    static void set_quest_size_range(
        const Rectangle& normal_quest_size,
        const Rectangle& min_quest_size,
        const Rectangle& max_quest_size);

    static void render(Surface& quest_surface);

  private:

    Video(bool disable_window, const Rectangle& wanted_quest_size);
    ~Video();

    static void shaded_render(Surface& quest_surface);
};

#endif

