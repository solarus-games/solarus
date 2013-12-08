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
#include "lowlevel/Shader.h"
#include <vector>
#include <map>

/**
 * \brief Draws the window and handles the video mode.
 */
class VideoManager {

  public:

    /**
     * \brief Structure of a video mode.
     */
    typedef struct _VideoMode {
      std::string name;                 /**< Name of the video mode. */
      Rectangle window_size;            /**< Final size of the window when selecting this video mode. */
      Shader* shader;                   /**< Shader to use. NULL or a pointer to one of the supported shaders. */
      
      _VideoMode(std::string name, Rectangle window_size, Shader* shader) :
          name(name),
          window_size(window_size),
          shader(shader) {};
    } VideoMode;

    static void initialize(int argc, char** argv);
    static void quit();
    static VideoManager* get_instance();
  
    SDL_Window* get_window();
    SDL_Renderer* get_renderer();
    SDL_PixelFormat* get_pixel_format();
    void create_window();
    void show_window();
    void update_viewport();
  
    void initialize_video_modes(bool skip_shaded_modes = false);
    VideoMode* get_video_mode() const;
    const std::vector<VideoMode*>& get_video_modes() const;
    bool is_mode_supported(VideoMode* mode) const;
    bool set_video_mode(VideoMode* mode);
    void set_default_video_mode();
    void switch_video_mode();

    static const std::string get_rendering_driver_name();
    static std::string get_video_mode_name(VideoMode* mode);
    static VideoMode* get_video_mode_by_name(const std::string& mode_name);

    bool is_fullscreen() const;
    void set_fullscreen(bool fullscreen);
    void switch_fullscreen();

    const std::string get_window_title() const;
    void set_window_title(const std::string& window_title);

    static bool parse_size(const std::string& size_string, Rectangle& size);
    static void set_absolute_position(Rectangle& rect);

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

  private:

    VideoManager(bool disable_window, const Rectangle& wanted_quest_size);
    ~VideoManager();

    void add_shader(Shader& shader);

    static VideoManager* instance;           /**< The only instance. */

    SDL_Window* main_window;                 /**< The window. */
    SDL_Renderer* main_renderer;             /**< The screen renderer. */
    SDL_PixelFormat* pixel_format;           /**< The pixel color format to use. */
    Rectangle viewport;                      /**< The position of the drawable area on the window. */
  
    bool disable_window;                     /**< Indicates that no window is displayed (used for unit tests). */
    bool fullscreen;                         /**< True if fullscreen display. */
    std::vector<Shader*> supported_shaders;  /**< Shaders that can be applied when rendering. */
    std::vector<VideoMode*> all_video_modes; /**< Display informations for each supported video mode. */
    VideoMode* video_mode;                   /**< Current display mode, pointer to an element of all_video_modes. */
    const std::string normal_mode_name;      /**< Non-shaded mode name. It will be forbidden for shaded ones. */

    Rectangle normal_quest_size;             /**< Default value of quest_size (depends on the quest). */
    Rectangle min_quest_size;                /**< Minimum value of quest_size (depends on the quest). */
    Rectangle max_quest_size;                /**< Maximum value of quest_size (depends on the quest). */
    Rectangle quest_size;                    /**< Size of the quest surface to render. */
    Rectangle wanted_quest_size;             /**< Size wanted by the user. */
};

#endif

