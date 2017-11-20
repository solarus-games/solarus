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
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Hq2xFilter.h"
#include "solarus/lowlevel/Hq3xFilter.h"
#include "solarus/lowlevel/Hq4xFilter.h"
#include "solarus/lowlevel/Logger.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Rectangle.h"
#include "solarus/lowlevel/Scale2xFilter.h"
#include "solarus/lowlevel/Size.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lowlevel/VideoMode.h"
#include "solarus/lowlevel/shaders/ShaderContext.h"
#include "solarus/Arguments.h"
#include "solarus/CurrentQuest.h"
#include <memory>
#include <sstream>
#include <utility>
#include <SDL_render.h>

namespace Solarus {

namespace {

SDL_Window* main_window = nullptr;        /**< The window. */
SDL_Renderer* main_renderer = nullptr;    /**< The screen renderer. */
SDL_Texture* render_target = nullptr;     /**< The render texture used when shader modes are supported. */
SDL_PixelFormat* pixel_format = nullptr;  /**< The pixel color format to use. */
std::string rendering_driver_name;        /**< The name of the rendering driver. */
bool disable_window = false;              /**< Indicates that no window is displayed (used for unit tests). */
bool fullscreen_window = false;           /**< True if the window is in fullscreen. */
bool visible_cursor = true;               /**< True if the mouse cursor is visible. */
bool rendertarget_supported = false;      /**< True if rendering on texture is supported. */
bool shaders_enabled = false;             /**< True if shaded modes support is enabled. */
bool acceleration_enabled = false;        /**< \c true if 2D GPU acceleration is available and enabled. */
SurfacePtr scaled_surface = nullptr;      /**< The screen surface used with software-scaled modes. */

std::vector<VideoMode> all_video_modes;   /**< Display information for each supported video mode. */
const VideoMode* video_mode;              /**< Current video mode. */
const VideoMode* default_video_mode;      /**< Default video mode. */

Size normal_quest_size;                   /**< Default value of quest_size (depends on the quest). */
Size min_quest_size;                      /**< Minimum value of quest_size (depends on the quest). */
Size max_quest_size;                      /**< Maximum value of quest_size (depends on the quest). */
Size quest_size;                          /**< Size of the quest surface to render. */
Size wanted_quest_size;                   /**< Size wanted by the user. */

Size window_size;                         /**< Size of the window. The quest size is stretched and
                                           * letterboxed to fit. In fullscreen, remembers the size
                                           * to use when returning to windowed mode. */

/**
 * \brief Creates the window but does not show it.
 * \param args Command-line arguments.
 */
void create_window() {

  Debug::check_assertion(main_window == nullptr, "Window already exists");

  // Set OpenGL as the default renderer driver when available, to avoid using Direct3d.
  SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_DEFAULT);

  // Set the default OpenGL built-in shader (nearest).
  SDL_SetHint(SDL_HINT_RENDER_OPENGL_SHADERS, "0");

  std::string title = std::string("Solarus ") + SOLARUS_VERSION;
  main_window = SDL_CreateWindow(
      title.c_str(),
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      wanted_quest_size.width,
      wanted_quest_size.height,
      SDL_WINDOW_HIDDEN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
  );
  Debug::check_assertion(main_window != nullptr,
      std::string("Cannot create the window: ") + SDL_GetError());

  int acceleration_flag = acceleration_enabled ?
      SDL_RENDERER_ACCELERATED : SDL_RENDERER_SOFTWARE;

  main_renderer = SDL_CreateRenderer(
        main_window,
        -1,
        acceleration_flag
  );

  if (main_renderer == nullptr) {
      // Try without vsync.
      main_renderer = SDL_CreateRenderer(main_window, -1, acceleration_flag);
      if (main_renderer == nullptr && acceleration_enabled) {
          // Try without acceleration.
          acceleration_enabled = false;
          acceleration_flag = SDL_RENDERER_SOFTWARE;
          main_renderer = SDL_CreateRenderer(main_window, -1, acceleration_flag);
      }
  }

  Debug::check_assertion(main_renderer != nullptr,
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

  Debug::check_assertion(pixel_format != nullptr, "No compatible pixel format");

  // Check renderer's flags
  rendering_driver_name = renderer_info.name;
  rendertarget_supported = (renderer_info.flags & SDL_RENDERER_TARGETTEXTURE) != 0;
  acceleration_enabled = acceleration_enabled
    && (renderer_info.flags & SDL_RENDERER_ACCELERATED) != 0;
  if (acceleration_enabled) {
    // Solarus uses accelerated graphics as of version 1.2 with SDL2.
    Logger::info("2D acceleration: yes");
  }
  else {
    // Acceleration may be disabled because the user decided so or because the
    // system does not support it.
    // This is not a problem: the engine runs perfectly in software mode.
    Logger::info("2D acceleration: no");
  }
}

/**
 * \brief Creates the list of video modes.
 *
 * Some video modes are hardcoded, and some others may be added by the quest.
 */
void initialize_video_modes() {

  // Decide whether we enable shaders.
  shaders_enabled = rendertarget_supported && Video::is_acceleration_enabled() && ShaderContext::initialize();

  // Initialize hardcoded video modes.
  all_video_modes.emplace_back(
      "normal",
      quest_size * 2,
      nullptr,
      nullptr
  );
  all_video_modes.emplace_back(
      "scale2x",
      quest_size * 2,
      std::unique_ptr<PixelFilter>(new Scale2xFilter()),
      nullptr
  );
  all_video_modes.emplace_back(
      "hq2x",
      quest_size * 2,
      std::unique_ptr<PixelFilter>(new Hq2xFilter()),
      nullptr
  );
  all_video_modes.emplace_back(
      "hq3x",
      quest_size * 3,
      std::unique_ptr<PixelFilter>(new Hq3xFilter()),
      nullptr
  );
  all_video_modes.emplace_back(
      "hq4x",
      quest_size * 4,
      std::unique_ptr<PixelFilter>(new Hq4xFilter()),
      nullptr
  );
  // TODO If shaders are enabled, use a C++ shader version of Scale2x and Hq4x instead.

  // Initialize quest custom video modes. These can only include shaded modes.
  if (shaders_enabled) {

    // Initialize the render target
    render_target = SDL_CreateTexture(
        main_renderer,
        pixel_format->format,
        SDL_TEXTUREACCESS_TARGET,
        quest_size.width,
        quest_size.height
    );
    SDL_SetTextureBlendMode(render_target, SDL_BLENDMODE_BLEND);

    // Get all shaders of the quest's shader/videomodes folder.
    const std::map<std::string, std::string>& shader_elements = CurrentQuest::get_resources(ResourceType::SHADER);

    for (const auto& kvp: shader_elements) {

      // Load the shader and add the corresponding video mode.
      const std::string& shader_id = kvp.first;
      std::unique_ptr<Shader> video_mode_shader =
          ShaderContext::create_shader(shader_id);
      if (video_mode_shader != nullptr) {

        const std::string& video_mode_name = video_mode_shader->get_name();
        if (Video::get_video_mode_by_name(video_mode_name) != nullptr) {
          Debug::error("There is already a video mode with name '" + video_mode_name + "'");
          continue;
        }

        const Size scaled_quest_size = quest_size * video_mode_shader->get_default_window_scale();
        all_video_modes.emplace_back(
              video_mode_shader->get_name(),
              scaled_quest_size,
              nullptr,
              std::move(video_mode_shader)
        );
      }
    }
  }

  default_video_mode = &all_video_modes[0];

  // Everything is ready now.
  Video::set_default_video_mode();
}

}  // Anonymous namespace.

/**
 * \brief Initializes the video system.
 *
 * This method should be called when the program starts.
 * Options recognized:
 *   -no-video
 *   -video-acceleration=yes|no
 *   -quest-size=WIDTHxHEIGHT
 *
 * \param args Command-line arguments.
 */
void Video::initialize(const Arguments& args) {

  Debug::check_assertion(!is_initialized(), "Video system already initialized");

  // Show the SDL version.
  SDL_version sdl_version;
  SDL_GetVersion(&sdl_version);
  std::ostringstream oss;
  oss << "SDL: " << static_cast<int>(sdl_version.major) << "." << static_cast<int>(sdl_version.minor) << "." << static_cast<int>(sdl_version.patch);
  Logger::info(oss.str());

  // Check the -no-video and the -quest-size options.
  const std::string& quest_size_string = args.get_argument_value("-quest-size");
  disable_window = args.has_argument("-no-video");

  wanted_quest_size = {
      SOLARUS_DEFAULT_QUEST_WIDTH,
      SOLARUS_DEFAULT_QUEST_HEIGHT
  };

  if (!quest_size_string.empty()) {
    if (!parse_size(quest_size_string, wanted_quest_size)) {
      Debug::error(std::string("Invalid quest size: '") + quest_size_string + "'");
    }
  }

  if (args.get_argument_value("-video-acceleration") == "no") {
    acceleration_enabled = false;
  }
  else {
    // Accelerated by default.
    acceleration_enabled = true;
  }

  if (disable_window) {
    // Create a pixel format anyway to make surface and color operations work,
    // even though nothing will ever be rendered.
    pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_ABGR8888);
  }
  else {
    create_window();
  }
}

/**
 * \brief Closes the video system.
 */
void Video::quit() {

  if (!is_initialized()) {
    return;
  }

  ShaderContext::quit();

  if (is_fullscreen()) {
    // Get back on desktop before destroy the window.
    SDL_SetWindowFullscreen(main_window, 0);
  }

  all_video_modes.clear();

  if (pixel_format != nullptr) {
    SDL_FreeFormat(pixel_format);
    pixel_format = nullptr;
  }
  if (main_renderer != nullptr) {
    SDL_DestroyRenderer(main_renderer);
    main_renderer = nullptr;
  }
  if (main_window != nullptr) {
    SDL_DestroyWindow(main_window);
    main_window = nullptr;
  }

  // TODO wrap in a class
  rendering_driver_name = "";
  disable_window = false;
  fullscreen_window = false;
  rendertarget_supported = false;
  shaders_enabled = false;
  acceleration_enabled = false;
  scaled_surface = nullptr;
  video_mode = nullptr;
  default_video_mode = nullptr;
  normal_quest_size = Size();
  min_quest_size = Size();
  max_quest_size = Size();
  quest_size = Size();
  wanted_quest_size = Size();
  window_size = Size();

}

/**
 * \brief Returns whether the video system is initialized.
 * \return \c true if video is initialized.
 */
bool Video::is_initialized() {
  return video_mode != nullptr;
}

/**
 * \brief Returns the main window.
 * \return The main window, or nullptr if there is no window.
 */
SDL_Window* Video::get_window() {
  return main_window;
}

/**
 * \brief Returns the main renderer.
 * \return The main renderer, or nullptr if there is no window.
 */
SDL_Renderer* Video::get_renderer() {
  return main_renderer;
}

/**
 * \brief Returns the render texture target, if any.
 * \return The render target, or nullptr.
 */
SDL_Texture* Video::get_render_target() {
    return render_target;
  }

/**
 * \brief Returns the pixel format to use.
 * \return The pixel format to use.
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

  const PixelFilter* software_filter = nullptr;
  if (video_mode != nullptr) {
    software_filter = video_mode->get_software_filter();
  }

  return acceleration_enabled      // 2D acceleration must be available on the system.
      && software_filter == nullptr;  // No software pixel filter must be active.
}

/**
 * \brief Returns whether a video mode is supported.
 * \param mode a video mode
 * \return true if this mode is supported
 */
bool Video::is_mode_supported(const VideoMode& mode) {

  for (const VideoMode& current_mode: all_video_modes) {
    if (current_mode.get_name() == mode.get_name()) {
      return true;
    }
  }

  // The initial detection of this mode failed.
  return false;
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

  Debug::check_assertion(video_mode != nullptr, "No video mode");
  set_video_mode(*video_mode, fullscreen);
  Logger::info(std::string("Fullscreen: ") + (fullscreen ? "yes" : "no"));
}

/**
 * \brief Returns whether the mouse cursor is currently visible.
 * \return true if the mouse cursor is currently visible.
 */
bool Video::is_cursor_visible() {
  return visible_cursor;
}

/**
 * \brief Sets the mouse cursor to visible or invisible.
 * \param cursor_visible true to make the cursor visible.
 */
void Video::set_cursor_visible(bool cursor_visible) {

  visible_cursor = cursor_visible;
  Debug::check_assertion(video_mode != nullptr, "No video mode");
  set_video_mode(*video_mode, is_fullscreen());
  Logger::info(std::string("Cursor visible: ") + (cursor_visible ? "yes" : "no"));
}

/**
 * \brief Sets the default video mode.
 */
void Video::set_default_video_mode() {

  Debug::check_assertion(default_video_mode != nullptr,
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

  // Find the current video mode in the list and traverse the list from there.
  std::vector<VideoMode>::const_iterator it;
  for (it = all_video_modes.begin(); it != all_video_modes.end(); ++it) {
    if (it->get_name() == video_mode->get_name()) {
      break;
    }
  };

  const VideoMode* mode = nullptr;
  do {
    if (it != all_video_modes.end()) {
      ++it;
    }
    if (it == all_video_modes.end()) {
      it = all_video_modes.begin();
    }
    mode = &(*it);
  } while (mode == nullptr || !is_mode_supported(*mode));

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

  bool mode_changed = video_mode == nullptr
      || mode.get_name() != video_mode->get_name();

  if (!is_mode_supported(mode)) {
    return false;
  }

  Uint32 fullscreen_flag;
  if (fullscreen) {
    fullscreen_flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    window_size = get_window_size();  // Store the window size before fullscreen.
  }
  else {
    fullscreen_flag = 0;
  }

  video_mode = &mode;
  fullscreen_window = fullscreen;

  if (!disable_window) {

    scaled_surface = nullptr;

    Size render_size = quest_size;

    const PixelFilter* software_filter = mode.get_software_filter();
    if (software_filter != nullptr) {
      int factor = software_filter->get_scaling_factor();
      render_size = quest_size * factor;
      scaled_surface = Surface::create(render_size);
      scaled_surface->fill_with_color(Color::black);  // To initialize the internal surface.
    }

    // Initialize the window.
    // Set fullscreen flag first to set the size on the right mode.
    SDL_SetWindowFullscreen(main_window, fullscreen_flag);
    SDL_RenderSetLogicalSize(
        main_renderer,
        render_size.width,
        render_size.height);
    SDL_ShowCursor(visible_cursor);

    if (mode_changed) {
      reset_window_size();
    }
  }

  if (mode_changed) {
    Logger::info(std::string("Video mode: ") + video_mode->get_name());

  }

  return true;
}

/**
 * \brief Returns the current video mode.
 * \return The video mode.
 */
const VideoMode& Video::get_video_mode() {

  Debug::check_assertion(video_mode != nullptr,
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
  for (const VideoMode& video_mode: all_video_modes) {
    result.push_back(&video_mode);
  }
  return result;
}

/**
 * \brief Returns a video mode given its Lua name.
 * \param mode_name Lua name of a video mode.
 * \return The corresponding supported video mode, or nullptr if there is no
 * known supported video mode with this name.
 */
const VideoMode* Video::get_video_mode_by_name(
    const std::string& mode_name) {

  for (const VideoMode& video_mode: all_video_modes) {
    if (video_mode.get_name() == mode_name) {
      return &video_mode;
    }
  }

  return nullptr;
}

/**
 * \brief Draws the quest surface on the screen with the current video mode.
 * \param quest_surface The quest surface to render on the screen.
 */
void Video::render(const SurfacePtr& quest_surface) {

  if (disable_window) {
    return;
  }

  Debug::check_assertion(video_mode != nullptr,
      "Missing video mode");

  // See if there is a filter to apply.
  const Shader* hardware_filter = video_mode->get_hardware_filter();
  const PixelFilter* software_filter = video_mode->get_software_filter();
  if (hardware_filter != nullptr) {
    // OpenGL rendering.
    hardware_filter->render(quest_surface);
  }
  else {
    // SDL rendering, with acceleration if supported, and optionally with
    // a software filter.
    Surface* surface_to_render = nullptr;
    if (software_filter != nullptr) {
      Debug::check_assertion(scaled_surface != nullptr,
          "Missing destination surface for scaling");
      quest_surface->apply_pixel_filter(*software_filter, *scaled_surface);
      surface_to_render = scaled_surface.get();
    }
    else {
      surface_to_render = quest_surface.get();
    }

    SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
    SDL_RenderSetClipRect(main_renderer, nullptr);
    SDL_RenderClear(main_renderer);
    surface_to_render->render(main_renderer);
    SDL_RenderPresent(main_renderer);
  }
}

/**
 * \brief Returns the current text of the window title bar.
 * \return The window title.
 */
std::string Video::get_window_title() {

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
bool Video::parse_size(const std::string& size_string, Size& size) {

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

  size = { width, height };
  return true;
}

/**
 * \brief Returns the size of the quest surface to render on the screen.
 * \return The quest size.
 */
const Size& Video::get_quest_size() {
  return quest_size;
}

/**
 * \brief Gets the allowed range of quest sizes for this quest.
 * \param normal_size Gets the default size for this quest.
 * \param min_size Gets the minimum size for this quest.
 * \param max_size Gets the maximum size for this quest.
 */
void Video::get_quest_size_range(
    Size& normal_size,
    Size& min_size,
    Size& max_size) {

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
 * \param normal_size Default size for this quest.
 * \param min_size Minimum size for this quest.
 * \param max_size Maximum size for this quest.
 */
void Video::set_quest_size_range(
    const Size& normal_size,
    const Size& min_size,
    const Size& max_size) {

  Debug::check_assertion(
      normal_size.width >= min_size.width
      && normal_size.height >= min_size.height
      && normal_size.width <= max_size.width
      && normal_size.height <= max_size.height,
      "Invalid quest size range");

  normal_quest_size = normal_size;
  min_quest_size = min_size;
  max_quest_size = max_size;

  if (wanted_quest_size.width < min_size.width
      || wanted_quest_size.height < min_size.height
      || wanted_quest_size.width > max_size.width
      || wanted_quest_size.height > max_size.height) {
    std::ostringstream oss;
    oss << "Cannot use quest size "
        << wanted_quest_size.width << "x" << wanted_quest_size.height
        << ": this quest only supports "
        << min_size.width << "x" << min_size.height
        << " to "
        << max_size.width << "x" << max_size.height
        << ". Using "
        << normal_size.width << "x" << normal_size.height
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
 * \return The size of the window in pixels.
 */
Size Video::get_window_size() {

  Debug::check_assertion(main_window != nullptr, "No window");
  Debug::check_assertion(!quest_size.is_flat(), "Quest size is not initialized");

  if (is_fullscreen()) {
    // Returns the memorized window size.
    return window_size;
  }

  // Returns the current window size.
  int width = 0;
  int height = 0;
  SDL_GetWindowSize(main_window, &width, &height);

  return { width, height };
}

/**
 * \brief Sets the size of the window.
 * \param size The size of the window in pixels.
 */
void Video::set_window_size(const Size& size) {

  Debug::check_assertion(main_window != nullptr, "No window");
  Debug::check_assertion(!quest_size.is_flat(), "Quest size is not initialized");
  Debug::check_assertion(
      size.width > 0 && size.height > 0,
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
    if (width != size.width || height != size.height) {
      SDL_SetWindowSize(
          main_window,
          size.width,
          size.height
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

  Debug::check_assertion(video_mode != nullptr, "No video mode");

  set_window_size(video_mode->get_initial_window_size());
}

/**
 * \brief Gets the viewport of the renderer.
 * The viewport is the logical drawing area of the renderer.
 * x and y indicate the possible black bars.
 * \return The viewport, in renderer logical coordinates (before window scaling).
 * Use SDL_RenderGetScale() to convert it to window coordinates.
 */
Rectangle Video::get_viewport() {

  SDL_Rect viewport;

  SDL_RenderGetViewport(get_renderer(), &viewport);

  return Rectangle(viewport.x, viewport.y, viewport.w, viewport.h);
}

/**
 * \brief Converts window coordinates to quest size coordinates.
 * \param window_xy A position relative to the window, not including
 * the title bar.
 * \return The position in quest size coordinate.
 */
Point Video::window_to_quest_coordinates(const Point& window_xy) {

  Rectangle viewport = get_viewport();

  float scale_x = 0.0;
  float scale_y = 0.0;
  SDL_RenderGetScale(get_renderer(), &scale_x, &scale_y);

  const double x_position = window_xy.x - viewport.get_x() * scale_x;
  const double y_position = window_xy.y - viewport.get_y() * scale_y;
  const double quest_size_width = quest_size.width;
  const double quest_size_height = quest_size.height;
  const double window_width = viewport.get_width() * scale_x;
  const double window_height = viewport.get_height() * scale_y;

  Debug::check_assertion(!quest_size.is_flat(), "Quest size is not initialized");
  Debug::check_assertion(!viewport.is_flat(), "Viewport is not initialized");

  return Point(
      (int) (x_position * quest_size_width / window_width),
      (int) (y_position * quest_size_height / window_height));
}

/**
 * \brief Converts logical renderer coordinates to quest size coordinates.
 * \param[in] renderer_xy A position in renderer coordinates, without window scaling.
 * \param[out] quest_xy The corresponding value in quest coordinates.
 * \return \c false if the position is not inside the renderer.
 */
bool Video::renderer_to_quest_coordinates(
    const Point& renderer_xy,
    Point& quest_xy
) {

  int renderer_width = 0;
  int renderer_height = 0;
  SDL_RenderGetLogicalSize(get_renderer(), &renderer_width, &renderer_height);

  const double quest_width = quest_size.width;
  const double quest_height = quest_size.height;

  quest_xy = {
      (int) (renderer_xy.x * quest_width / renderer_width),
      (int) (renderer_xy.y * quest_height / renderer_height)
  };

  if (quest_xy.x < 0
      || quest_xy.y < 0
      || quest_xy.x >= quest_width
      || quest_xy.y >= quest_height
  ) {
    return false;
  }

  return true;
}

}
