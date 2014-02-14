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
#include "lowlevel/Video.h"
#include "lowlevel/VideoMode.h"
#include "lowlevel/Scale2xFilter.h"
#include "lowlevel/Hq2xFilter.h"
#include "lowlevel/Hq3xFilter.h"
#include "lowlevel/Hq4xFilter.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "CommandLine.h"
#include <map>
#include <algorithm>
#include <sstream>

namespace solarus {

namespace {

SDL_Window* main_window = NULL;           /**< The window. */
SDL_Renderer* main_renderer = NULL;       /**< The screen renderer. */
SDL_Texture* render_target = NULL;        /**< The render texture used when shader modes are supported. */
SDL_PixelFormat* pixel_format = NULL;     /**< The pixel color format to use. */
std::string rendering_driver_name;        /**< The name of the rendering driver. */
bool disable_window = false;              /**< Indicates that no window is displayed (used for unit tests). */
bool fullscreen_window = false;           /**< True if the window is in fullscreen. */
bool rendertarget_supported = false;      /**< True if rendering on texture is supported. */
bool shaders_supported = false;           /**< True if shaded modes and rendering on texture are supported. */
bool shaders_enabled = false;             /**< True if shaded modes support is enabled. */
bool acceleration_enabled = false;        /**< \c true if 2D GPU acceleration is available and enabled. */
Surface* scaled_surface = NULL;           /**< The screen surface used with software-scaled modes. */

std::vector<VideoMode*>
    all_video_modes;                      /**< Display information for each supported video mode. */
const VideoMode* video_mode;              /**< Current video mode. */
const VideoMode* default_video_mode;      /**< Default video mode. */

Rectangle normal_quest_size;              /**< Default value of quest_size (depends on the quest). */
Rectangle min_quest_size;                 /**< Minimum value of quest_size (depends on the quest). */
Rectangle max_quest_size;                 /**< Maximum value of quest_size (depends on the quest). */
Rectangle quest_size;                     /**< Size of the quest surface to render. */
Rectangle wanted_quest_size;              /**< Size wanted by the user. */

Rectangle window_size;                    /**< Size of the window. The quest size is stretched and
                                           * letterboxed to fit. In fullscreen, remembers the size
                                           * to use when returning to windowed mode. */

/**
 * \brief Creates the window but does not show it.
 * \param args Command-line arguments.
 */
void create_window(const CommandLine& args) {

  Debug::check_assertion(main_window == NULL, "Window already exists");

#if SOLARUS_HAVE_OPENGL == 1
  // Set OpenGL as the default renderer driver when available, to avoid using Direct3d.
  SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_DEFAULT);

  // Set the default OpenGL built-in shader (nearest)
  SDL_SetHint(SDL_HINT_RENDER_OPENGL_SHADERS, "0");
#endif

  main_window = SDL_CreateWindow(
      (std::string("Solarus ") + SOLARUS_VERSION).c_str(),
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      wanted_quest_size.get_width(),
      wanted_quest_size.get_height(),
      SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE
#if SOLARUS_HAVE_OPENGL == 1
      | SDL_WINDOW_OPENGL
#endif
  );
  Debug::check_assertion(main_window != NULL,
      std::string("Cannot create the window: ") + SDL_GetError());

  int acceleration_flag = 0;
  if (args.get_argument_value("-video-acceleration") == "no") {
    acceleration_enabled = false;
    acceleration_flag = SDL_RENDERER_SOFTWARE;
  }
  else {
    // Accelerated by default.
    acceleration_enabled = true;
    acceleration_flag = SDL_RENDERER_ACCELERATED;
  }
  main_renderer = SDL_CreateRenderer(main_window, -1, acceleration_flag
#if SOLARUS_HAVE_OPENGL != 1
      | SDL_RENDERER_PRESENTVSYNC
#endif
  );

  if (main_renderer == NULL) {
      // Try without vsync.
      main_renderer = SDL_CreateRenderer(main_window, -1, acceleration_flag);
      if (main_renderer == NULL && acceleration_enabled) {
          // Try without acceleration.
          acceleration_enabled = false;
          acceleration_flag = SDL_RENDERER_SOFTWARE;
          main_renderer = SDL_CreateRenderer(main_window, -1, acceleration_flag);
      }
  }

  Debug::check_assertion(main_renderer != NULL,
      std::string("Cannot create the renderer: ") + SDL_GetError());

  // Allow blending mode for direct drawing primitives.
  SDL_SetRenderDrawBlendMode(main_renderer, SDL_BLENDMODE_BLEND);

  // Get the first renderer format which supports alpha channel and is not a unique format.
  SDL_RendererInfo renderer_info;
  SDL_GetRendererInfo(main_renderer, &renderer_info);
  for (unsigned i = 0; i < renderer_info.num_texture_formats; ++i) {

    if (!SDL_ISPIXELFORMAT_FOURCC(renderer_info.texture_formats[i])
        && SDL_ISPIXELFORMAT_ALPHA(renderer_info.texture_formats[i])) {
      pixel_format = SDL_AllocFormat(renderer_info.texture_formats[i]);
      break;
    }
  }

  Debug::check_assertion(pixel_format != NULL, "No compatible pixel format");

  // Check renderer's flags
  rendering_driver_name = renderer_info.name;
  rendertarget_supported = (renderer_info.flags & SDL_RENDERER_TARGETTEXTURE) != 0;
  acceleration_enabled = acceleration_enabled
    && (renderer_info.flags & SDL_RENDERER_ACCELERATED) != 0;
  if (acceleration_enabled) {
    // Solarus uses accelerated graphics as of version 1.2 with SDL2.
    std::cout << "2D acceleration: yes" << std::endl;
  }
  else {
    // Acceleration may be disabled because the user decided so or because the
    // system does not support it.
    // This is not a problem: the engine runs perfectly in software mode.
    std::cout << "2D acceleration: no" << std::endl;
  }
}

/**
 * \brief Creates the list of video modes.
 *
 * Some video modes are hardcoded, and some others may be added by the quest.
 */
void initialize_video_modes() {

  // Decide whether we enable shaders.
  shaders_supported = rendertarget_supported && Video::is_acceleration_enabled() && Shader::initialize();
  shaders_enabled = shaders_supported;

  // Initialize hardcoded video modes.
  const Rectangle quest_size_2(0, 0, quest_size.get_width() * 2, quest_size.get_height() * 2);
  const Rectangle quest_size_3(0, 0, quest_size.get_width() * 3, quest_size.get_height() * 3);
  const Rectangle quest_size_4(0, 0, quest_size.get_width() * 4, quest_size.get_height() * 4);
  all_video_modes.push_back(new VideoMode("normal", quest_size_2, NULL, NULL));
  all_video_modes.push_back(new VideoMode("scale2x", quest_size_2, new Scale2xFilter(), NULL));
  all_video_modes.push_back(new VideoMode("hq2x", quest_size_2, new Hq2xFilter(), NULL));
  all_video_modes.push_back(new VideoMode("hq3x", quest_size_3, new Hq3xFilter(), NULL));
  all_video_modes.push_back(new VideoMode("hq4x", quest_size_4, new Hq4xFilter(), NULL));
  default_video_mode = all_video_modes[0];
  // TODO If shaders are enabled, use a C++ shader version of Scale2x and Hq4x instead.

  // Initialize quest custom video modes. These can only include shaded modes.
  if (shaders_enabled) {

    // Initialize the render target
    render_target = SDL_CreateTexture(
        main_renderer,
        pixel_format->format,
        SDL_TEXTUREACCESS_TARGET,
        quest_size.get_width(),
        quest_size.get_height());
    SDL_SetTextureBlendMode(render_target, SDL_BLENDMODE_BLEND);

    // Get all shaders of the quest's shader/filters folder.
    std::vector<std::string> shader_names =
        FileTools::data_files_enumerate("shaders/filters/", true, false);
    // FIXME don't enumerate data files, use the quest resource system like always.

    for (unsigned i = 0; i < shader_names.size(); ++i) {

      // Load the shader and add the corresponding video mode.
      Shader* video_mode_shader = Shader::create(shader_names.at(i));
      if (video_mode_shader != NULL) {

        const std::string& video_mode_name = video_mode_shader->get_name();
        if (Video::get_video_mode_by_name(video_mode_name) != NULL) {
          Debug::error("There is already a video mode with name '" + video_mode_name);
          delete video_mode_shader;
          continue;
        }

        const Rectangle scaled_quest_size(0, 0,
            int(quest_size.get_width() * video_mode_shader->get_window_scale()),
            int(quest_size.get_height() * video_mode_shader->get_window_scale()));
        all_video_modes.push_back(new VideoMode(
              video_mode_shader->get_name(),
              scaled_quest_size,
              NULL,
              video_mode_shader
        ));
      }
    }
  }

  // Everything is ready now.
  Video::set_default_video_mode();
}

};

/**
 * \brief Initializes the video system.
 *
 * This method should be called when the program starts.
 * Options recognized:
 *   -no-video
 *   -video-acceleration=yes|no
 *   -quest-size=<width>x<height>
 *
 * \param args Command-line arguments.
 */
void Video::initialize(const CommandLine& args) {

  // Check the -no-video and the -quest-size options.
  const std::string& quest_size_string = args.get_argument_value("-quest-size");
  disable_window = args.has_argument("-no-video");

  wanted_quest_size = Rectangle(0, 0,
      SOLARUS_DEFAULT_QUEST_WIDTH, SOLARUS_DEFAULT_QUEST_HEIGHT);

  if (!quest_size_string.empty()) {
    if (!parse_size(quest_size_string, wanted_quest_size)) {
      Debug::error(std::string("Invalid quest size: '") + quest_size_string + "'");
    }
  }

  if (!disable_window) {
    create_window(args);
  }
}

/**
 * \brief Closes the video system.
 */
void Video::quit() {

  if (is_fullscreen()) {
    // Get back on desktop before destroy the window.
    SDL_SetWindowFullscreen(main_window, 0);
  }

  for (unsigned i = 0; i < all_video_modes.size(); ++i) {
    delete all_video_modes[i];
  }
  all_video_modes.clear();

  if (pixel_format != NULL) {
    SDL_FreeFormat(pixel_format);
  }
  if (main_renderer != NULL) {
    SDL_DestroyRenderer(main_renderer);
  }
  if (main_window != NULL) {
    SDL_DestroyWindow(main_window);
  }
}

/**
 * \brief Returns the main window.
 * \return the main window.
 */
SDL_Window* Video::get_window() {
  return main_window;
}

/**
 * \brief Returns the main renderer.
 * \return the main renderer.
 */
SDL_Renderer* Video::get_renderer() {
  return main_renderer;
}
  
/**
 * \brief Returns the render texture target, if any.
 * \return the render target, or NULL.
 */
SDL_Texture* Video::get_render_target() {
    return render_target;
  }

/**
 * \brief Returns the pixel format to use.
 * \return the pixel format to use.
 */
SDL_PixelFormat* Video::get_pixel_format() {
  return pixel_format;
}

/**
 * \brief Get the default rendering driver for the current platform.
 * \return a string containing the rendering driver name.
 */
const std::string& Video::get_rendering_driver_name() {

  return rendering_driver_name;
}

/**
 * \brief Show the window.
 */
void Video::show_window() {

  SDL_ShowWindow(main_window);
}

/**
 * \brief Returns whether 2D hardware acceleration is currently enabled.
 *
 * 2D acceleration is enabled if the system supports it, except when the
 * current video mode needs to work in software mode
 * (typically because of a pixel filter implemented in software).
 *
 * \return \c true if GPU acceleration is active.
 */
bool Video::is_acceleration_enabled() {

  const PixelFilter* software_filter = NULL;
  if (video_mode != NULL) {
    software_filter = video_mode->get_software_filter();
  }

  return acceleration_enabled      // 2D acceleration must be available on the system.
      && software_filter == NULL;  // No software pixel filter must be active.
}

/**
 * \brief Returns whether a video mode is supported.
 * \param mode a video mode
 * \return true if this mode is supported
 */
bool Video::is_mode_supported(const VideoMode& mode) {

  std::vector<VideoMode*>::const_iterator it = std::find(
      all_video_modes.begin(), all_video_modes.end(), &mode);

  if (it == all_video_modes.end()) {
    // The initial detection of this mode failed.
    return false;
  }

  if ((*it)->get_initial_window_size().is_flat()) {
    Debug::die(std::string(
        "Uninitialized size for video mode ") + mode.get_name());
  }

  return true;
}

/**
 * \brief Returns whether the current video mode is in fullscreen.
 * \return true if the current video mode is in fullscreen.
 */
bool Video::is_fullscreen() {
  return fullscreen_window;
}

/**
 * \brief Sets the video mode to fullscreen or windowed,
 * keeping an equivalent resolution.
 * \param fullscreen true to make fullscreen.
 */
void Video::set_fullscreen(bool fullscreen) {

  Debug::check_assertion(video_mode != NULL, "No video mode");
  set_video_mode(*video_mode, fullscreen);
}

/**
 * \brief Sets the default video mode.
 */
void Video::set_default_video_mode() {

  Debug::check_assertion(default_video_mode != NULL,
      "Default video mode was not initialized");

  set_video_mode(*default_video_mode, false);
}

/**
 * \brief Sets the next video mode.
 *
 * The fullscreen flag is preserved.
 * The window size is reset to the default size of the new video mode.
 */
void Video::switch_video_mode() {

  if (all_video_modes.size() <= 1) {
    return;
  }

  std::vector<VideoMode*>::const_iterator it = std::find(
      all_video_modes.begin(), all_video_modes.end(), video_mode
  );
  VideoMode* mode = NULL;
  do {
    if (it == all_video_modes.end()) {
      it = all_video_modes.begin();
    }
    else {
      ++it;
    }
    mode = *it;
  } while (mode == NULL || !is_mode_supported(*mode));

  set_video_mode(*mode);
}

/**
 * \brief Sets the video mode, keeping the fullscreen setting unchanged.
 * \param mode The video mode to set.
 * \return true in case of success, false if this mode is not supported.
 */
bool Video::set_video_mode(const VideoMode& mode) {

  return set_video_mode(mode, is_fullscreen());
}

/**
 * \brief Sets the video mode and the fullscreen setting.
 *
 * If the mode changes, the window size is reset to its default value.
 *
 * \param mode The video mode to set.
 * \param fullscreen \c true to set the window to fullscreen.
 * \return true in case of success, false if this mode is not supported.
 */
bool Video::set_video_mode(const VideoMode& mode, bool fullscreen) {

  bool mode_changed = video_mode == NULL
      || mode.get_name() != video_mode->get_name();

  if (!is_mode_supported(mode)) {
    return false;
  }

  int show_cursor;
  Uint32 fullscreen_flag;
  if (fullscreen) {
    fullscreen_flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    show_cursor = SDL_DISABLE;
    window_size = get_window_size();  // Store the window size before fullscreen.
  }
  else {
    fullscreen_flag = 0;
    show_cursor = SDL_ENABLE;
  }

  if (!disable_window) {

    RefCountable::unref(scaled_surface);
    scaled_surface = NULL;

    Rectangle render_size(quest_size);

    const PixelFilter* software_filter = mode.get_software_filter();
    if (software_filter != NULL) {
      int factor = software_filter->get_scaling_factor();
      render_size.set_size(
          quest_size.get_width() * factor,
          quest_size.get_height() * factor
      );
      scaled_surface = Surface::create(render_size);
      RefCountable::ref(scaled_surface);
      scaled_surface->fill_with_color(Color::get_black());  // To initialize the internal surface.
    }

    // Initialize the window.
    // Set fullscreen flag first to set the size on the right mode.
    SDL_SetWindowFullscreen(main_window, fullscreen_flag);
    if (!fullscreen && is_fullscreen()) {
      SDL_SetWindowSize(
          main_window,
          window_size.get_width(),
          window_size.get_height()
      );
      SDL_SetWindowPosition(main_window,
          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    SDL_RenderSetLogicalSize(
        main_renderer,
        render_size.get_width(),
        render_size.get_height());
    SDL_ShowCursor(show_cursor);
  }

  video_mode = &mode;
  fullscreen_window = fullscreen;

  if (mode_changed) {
    reset_window_size();
  }

  return true;
}

/**
 * \brief Returns the current video mode.
 * \return The video mode.
 */
const VideoMode& Video::get_video_mode() {

  Debug::check_assertion(video_mode != NULL,
      "Video mode not initialized");
  return *video_mode;
}

/**
 * \brief Returns a list of all supported video modes.
 * \return The list of supported video modes.
 */
std::vector<const VideoMode*> Video::get_video_modes() {

  // Return a copy of all_video_modes with const elements.
  std::vector<const VideoMode*> result;
  std::vector<VideoMode*>::const_iterator it;
  for (it = all_video_modes.begin(); it != all_video_modes.end(); ++it) {
    result.push_back(*it);
  }
  return result;
}

/**
 * \brief Returns a video mode given its Lua name.
 * \param mode_name Lua name of a video mode.
 * \return The corresponding supported video mode, or NULL if there is no
 * known supported video mode with this name.
 */
const VideoMode* Video::get_video_mode_by_name(
    const std::string& mode_name) {

  for (unsigned i = 0; i < all_video_modes.size(); ++i) {
    if (all_video_modes[i]->get_name() == mode_name) {
      return all_video_modes[i];
    }
  }
  
  return NULL;
}

/**
 * \brief Draws the quest surface on the screen with the current video mode.
 * \param quest_surface The quest surface to render on the screen.
 */
void Video::render(Surface& quest_surface) {

  if (disable_window) {
    return;
  }

  Debug::check_assertion(video_mode != NULL,
      "Missing video mode");

  // See if there is a filter to apply.
  Shader* hardware_filter = video_mode->get_hardware_filter();
  const PixelFilter* software_filter = video_mode->get_software_filter();
  if (hardware_filter != NULL) {
    // OpenGL rendering.
    hardware_filter->render(quest_surface);
  }
  else {
    // SDL rendering, with acceleration if supported, and optionally with
    // a software filter.
    Surface* surface_to_render = NULL;
    if (software_filter != NULL) {
      Debug::check_assertion(scaled_surface != NULL,
          "Missing destination surface for scaling");
      quest_surface.apply_pixel_filter(*software_filter, *scaled_surface);
      surface_to_render = scaled_surface;
    }
    else {
      surface_to_render = &quest_surface;
    }

    SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
    SDL_RenderSetClipRect(main_renderer, NULL);
    SDL_RenderClear(main_renderer);
    surface_to_render->render(main_renderer);
    SDL_RenderPresent(main_renderer);
  }
}

/**
 * \brief Returns the current text of the window title bar.
 * \return The window title.
 */
const std::string Video::get_window_title() {

  return SDL_GetWindowTitle(main_window);
}

/**
 * \brief Sets the text of the window title bar.
 * \param window_title The window title to set.
 */
void Video::set_window_title(const std::string& window_title) {

  SDL_SetWindowTitle(main_window, window_title.c_str());
}

/**
 * \brief Gets the width and the height values from a size string of the form
 * "320x240".
 * \param size_string The input string.
 * \param size The resulting size. Unchanged in case of failure.
 * \return true in case of success, false if the string is not a valid size.
 */
bool Video::parse_size(const std::string& size_string, Rectangle& size) {

  size_t index = size_string.find('x');
  if (index == std::string::npos || index + 1 >= size_string.size()) {
    return false;
  }

  const std::string& width_string = size_string.substr(0, index);
  const std::string& height_string = size_string.substr(index + 1);

  int width = 0;
  int height = 0;
  std::istringstream iss(width_string);
  if (!(iss >> width) || width < 0) {
    return false;
  }

  iss.str(height_string);
  iss.clear();
  if (!(iss >> height) || height < 0) {
    return false;
  }

  size.set_size(width, height);
  return true;
}

/**
 * \brief Returns the size of the quest surface to render on the screen.
 * \return The quest size.
 */
const Rectangle& Video::get_quest_size() {
  return quest_size;
}

/**
 * \brief Gets the allowed range of quest sizes for this quest.
 * \param normal_quest_size Gets the default size for this quest.
 * \param min_quest_size Gets the minimum size for this quest.
 * \param max_quest_size Gets the maximum size for this quest.
 */
void Video::get_quest_size_range(
    Rectangle& normal_size,
    Rectangle& min_size,
    Rectangle& max_size) {

  normal_size = normal_quest_size;
  min_size = min_quest_size;
  max_size = max_quest_size;
}

/**
 * \brief Sets the allowed range of quest sizes for this quest.
 *
 * This function sets the actual quest size and finishes the initialization
 * by creating the window.
 *
 * \param normal_quest_size Default size for this quest.
 * \param min_quest_size Minimum size for this quest.
 * \param max_quest_size Maximum size for this quest.
 */
void Video::set_quest_size_range(
    const Rectangle& normal_size,
    const Rectangle& min_size,
    const Rectangle& max_size) {

  Debug::check_assertion(
      normal_size.get_width() >= min_size.get_width()
      && normal_size.get_height() >= min_size.get_height()
      && normal_size.get_width() <= max_size.get_width()
      && normal_size.get_height() <= max_size.get_height(),
      "Invalid quest size range");

  normal_quest_size = normal_size;
  min_quest_size = min_size;
  max_quest_size = max_size;

  if (wanted_quest_size.get_width() < min_size.get_width()
      || wanted_quest_size.get_height() < min_size.get_height()
      || wanted_quest_size.get_width() > max_size.get_width()
      || wanted_quest_size.get_height() > max_size.get_height()) {
    std::ostringstream oss;
    oss << "Cannot use quest size "
        << wanted_quest_size.get_width() << "x" << wanted_quest_size.get_height()
        << ": this quest only supports "
        << min_size.get_width() << "x" << min_size.get_height()
        << " to "
        << max_size.get_width() << "x" << max_size.get_height()
        << ". Using "
        << normal_size.get_width() << "x" << normal_size.get_height()
        << " instead.";
    Debug::warning(oss.str());
    quest_size = normal_size;
  }
  else {
    quest_size = wanted_quest_size;
  }

  // We know the quest size: finish the initialization.
  initialize_video_modes();
}

/**
 * \brief Returns the size of the window.
 * \return The size of the window in pixels. The x and y value of the
 * returned rectangle are set to zero.
 */
Rectangle Video::get_window_size() {

  Debug::check_assertion(main_window != NULL, "No window");
  Debug::check_assertion(!quest_size.is_flat(), "Quest size is not initialized");

  if (is_fullscreen()) {
    // Returns the memorized window size.
    return window_size;
  }

  // Returns the current window size.
  int width = 0;
  int height = 0;
  SDL_GetWindowSize(main_window, &width, &height);

  return Rectangle(0, 0, width, height);
}

/**
 * \brief Sets the size of the window.
 * \param window_size The size of the window in pixels.
 * The x and y values of the rectangle are ignored.
 */
void Video::set_window_size(const Rectangle& size) {

  Debug::check_assertion(main_window != NULL, "No window");
  Debug::check_assertion(!quest_size.is_flat(), "Quest size is not initialized");
  Debug::check_assertion(
      size.get_width() > 0 && size.get_height() > 0,
      "Wrong window size"
  );

  if (is_fullscreen()) {
    // Store the size to remember it during fullscreen.
    window_size = size;
  }
  else {
    int width = 0;
    int height = 0;
    SDL_GetWindowSize(main_window, &width, &height);
    if (width != size.get_width() || height != size.get_height()) {
      SDL_SetWindowSize(
          main_window,
          size.get_width(),
          size.get_height()
      );
      SDL_SetWindowPosition(
          main_window,
          SDL_WINDOWPOS_CENTERED,
          SDL_WINDOWPOS_CENTERED
      );
    }
  }
}

/**
 * \brief Restores the default size of the window for the current video mode.
 */
void Video::reset_window_size() {

  Debug::check_assertion(video_mode != NULL, "No video mode");

  set_window_size(video_mode->get_initial_window_size());
}

}
