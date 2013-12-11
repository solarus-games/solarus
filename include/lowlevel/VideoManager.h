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
#ifndef SOLARUS_VIDEO_MANAGER_H
#define SOLARUS_VIDEO_MANAGER_H

#include "Common.h"
#include <list>
#include <string>

struct SDL_PixelFormat;
struct SDL_Renderer;

/**
 * \brief Draws the window and handles the video mode.
 */
class VideoManager {

  public:

    /**
     * \brief The different possible video modes.
     */
    enum VideoMode {
      NO_MODE = -1,             /**< special value to mean no information */
      WINDOWED_STRETCHED,       /**< the quest surface is stretched into a double-size window (default) */
      WINDOWED_SCALE2X,         /**< the quest surface is scaled into a double-size window with the Scale2x algorithm */
      WINDOWED_HQ4X,            /**< the quest surface is scaled into a quadruple-size window with the hq4x algorithm */
      WINDOWED_NORMAL,          /**< the quest surface is drawn on a window of the same size */
      FULLSCREEN_NORMAL,        /**< the quest surface is drawn in fullscreen */
      FULLSCREEN_SCALE2X,       /**< the quest surface is scaled into a double-size screen with the Scale2x algorithm */
      FULLSCREEN_HQ4X,          /**< the quest surface is scaled into a double-size screen with the Scale2x algorithm */
      NB_MODES                  /**< number of existing video modes */
    };

    static void initialize(int argc, char** argv);
    static void quit();

    static SDL_PixelFormat* get_pixel_format();
    static SDL_Renderer* get_renderer();
    static bool is_acceleration_enabled();
    static void create_window();
    static void show_window();

    static VideoMode get_video_mode();
    static bool set_video_mode(VideoMode mode);
    static void update_viewport();
  
    static void switch_video_mode();
    static void set_default_video_mode();
    static bool is_mode_supported(VideoMode mode);
    static const std::list<VideoMode> get_video_modes();

    static std::string get_video_mode_name(VideoMode mode);
    static VideoMode get_video_mode_by_name(const std::string& mode_name);

    static bool is_fullscreen(VideoMode mode);
    static bool is_fullscreen();
    static void set_fullscreen(bool fullscreen);
    static void switch_fullscreen();

    static const std::string get_window_title();
    static void set_window_title(const std::string& window_title);

    static bool parse_size(const std::string& size_string, Rectangle& size);
    static void set_absolute_position(Rectangle& rect);

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

    static const std::string video_mode_names[];

};

#endif

