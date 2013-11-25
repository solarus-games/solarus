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
#include "lowlevel/Rectangle.h"
#include <list>
#include <map>

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
    static VideoManager* get_instance();

    SDL_Renderer* get_renderer();
    SDL_PixelFormat* get_pixel_format();
    void create_window();
    void show_window();
    VideoMode get_video_mode() const;
    bool set_video_mode(VideoMode mode);
    void switch_video_mode();
    void set_default_video_mode();
    bool is_mode_supported(VideoMode mode) const;
    const std::list<VideoMode> get_video_modes() const;

    static std::string get_video_mode_name(VideoMode mode);
    static VideoMode get_video_mode_by_name(const std::string& mode_name);

    bool is_fullscreen(VideoMode mode) const;
    bool is_fullscreen() const;
    void set_fullscreen(bool fullscreen);
    void switch_fullscreen();

    const std::string get_window_title() const;
    void set_window_title(const std::string& window_title);

    static bool parse_size(const std::string& size_string, Rectangle& size);

    const Rectangle& get_quest_size() const;
    void get_quest_size_range(
        Rectangle& normal_quest_size,
        Rectangle& min_quest_size,
        Rectangle& max_quest_size) const;
    void set_quest_size_range(
        const Rectangle& normal_quest_size,
        const Rectangle& min_quest_size,
        const Rectangle& max_quest_size);

    void render(Surface& quest_surface);

    static const std::string video_mode_names[];

  private:

    VideoManager(bool disable_window, const Rectangle& wanted_quest_size);
    ~VideoManager();

    void initialize_video_modes();
    void apply_pixel_filter(Surface& src_surface, Surface& dst_surface);

    static VideoManager* instance;          /**< The only instance. */

    bool disable_window;                    /**< Indicates that no window is displayed (used for unit tests). */
    std::map<VideoMode, Rectangle>
        mode_sizes;                         /**< Size of the screen surface for each supported
                                             * video mode with the current quest size. */

    SDL_Window* main_window;                /**< The window. */
    SDL_Renderer* main_renderer;            /**< The screen renderer. */
    SDL_PixelFormat* pixel_format;          /**< The pixel color format to use. */
    const PixelFilter* pixel_filter;        /**< The pixel filtering algorithm (if any) applied with
                                             * the current video mode. */
    Surface* scaled_surface;                /**< The screen surface used with scaled modes. */
  
    VideoMode video_mode;                   /**< Current display mode. */

    Rectangle normal_quest_size;            /**< Default value of quest_size (depends on the quest). */
    Rectangle min_quest_size;               /**< Minimum value of quest_size (depends on the quest). */
    Rectangle max_quest_size;               /**< Maximum value of quest_size (depends on the quest). */
    Rectangle quest_size;                   /**< Size of the quest surface to render. */
    Rectangle wanted_quest_size;            /**< Size wanted by the user. */
};

#endif

