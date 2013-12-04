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
#include "lowlevel/VideoManager.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <vector>

VideoManager* VideoManager::instance = NULL;

namespace {

// Forcing a video mode at compilation time.
#if defined(SOLARUS_SCREEN_FORCE_MODE) && SOLARUS_SCREEN_FORCE_MODE != -1
// Force a unique video mode at compilation time.
  const std::string forced_mode_name = SOLARUS_SCREEN_FORCE_MODE;
#else
// Make all modes available.
  const std::string forced_mode_name = "";
#endif
};

/**
 * \brief Initializes the video system and creates the window.
 *
 * This method should be called when the application starts.
 * Options "-no-video" and "-quest-size=<width>x<height>" are recognized.
 *
 * \param argc Command-line arguments number.
 * \param argv Command-line arguments.
 */
void VideoManager::initialize(int argc, char **argv) {
  // TODO pass options as an std::set<string> instead.
  
  // check the -no-video and the -quest-size options.
  bool disable = false;
  std::string quest_size_string;
  for (argv++; argc > 1; argv++, argc--) {
    const std::string arg = *argv;
    if (arg == "-no-video") {
      disable = true;
    }
    else if (arg.find("-quest-size=") == 0) {
      quest_size_string = arg.substr(12);
    }
  }

  Rectangle wanted_quest_size(0, 0,
      SOLARUS_DEFAULT_QUEST_WIDTH, SOLARUS_DEFAULT_QUEST_HEIGHT);

  if (!quest_size_string.empty()) {
    if (!parse_size(quest_size_string, wanted_quest_size)) {
      Debug::error(std::string("Invalid quest size: '") + quest_size_string + "'");
    }
  }
  
  // Set OpenGL as default renderer driver when available, to avoid to use direct3d.
  SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_DEFAULT);
  
  instance = new VideoManager(disable, wanted_quest_size);
}

/**
 * \brief Closes the video system.
 */
void VideoManager::quit() {
  delete instance;
}

/**
 * \brief Returns the video manager.
 * \return the only video manager.
 */
VideoManager* VideoManager::get_instance() {
  return instance;
}

/**
 * \brief Returns the main window.
 * \return the main window.
 */
SDL_Window* VideoManager::get_window() {
  return main_window;
}

/**
 * \brief Returns the main renderer.
 * \return the main renderer.
 */
SDL_Renderer* VideoManager::get_renderer() {
  return main_renderer;
}

/**
 * \brief Returns the pixel format to use.
 * \return the pixel format to use.
 */
SDL_PixelFormat* VideoManager::get_pixel_format() {
  return pixel_format;
}

/**
 * \brief Constructor.
 * \brief disable_window true to entirely disable the displaying.
 * \param wanted_quest_size Size of the quest as requested by the user.
 */
VideoManager::VideoManager(
    bool disable_window,
    const Rectangle& wanted_quest_size):
  disable_window(disable_window),
  fullscreen(false),
  main_window(NULL),
  main_renderer(NULL),
  pixel_format(NULL),
  video_mode(NULL),
  wanted_quest_size(wanted_quest_size) {
    
    create_window();
}

/**
 * \brief Destructor.
 */
VideoManager::~VideoManager() {

  if (is_fullscreen()) {
    // Get back on desktop before destroy the window.
    SDL_SetWindowFullscreen(main_window, 0);
  }

  if (pixel_format != NULL) {
    SDL_FreeFormat(pixel_format);
  }
  if (main_renderer != NULL) {
    SDL_DestroyRenderer(main_renderer);
  }
  if (main_window != NULL) {
    SDL_DestroyWindow(main_window);
  }
  for(int i=0 ; i<supported_shaders.size() ; ++i) {
    delete supported_shaders.at(i);
  }
  for(int i=0 ; i<all_video_modes.size() ; ++i) {
    delete all_video_modes.at(i);
  }
}

/**
 * \brief Create the window.
 */
void VideoManager::create_window() {
  
  main_window = SDL_CreateWindow(
      (std::string("Solarus ") + SOLARUS_VERSION).c_str(),
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      wanted_quest_size.get_width(),
      wanted_quest_size.get_height(),
      SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL);
  Debug::check_assertion(main_window != NULL,
      std::string("Cannot create the window: ") + SDL_GetError());
  
  main_renderer = SDL_CreateRenderer(main_window, -1, 0);
  Debug::check_assertion(main_renderer != NULL,
      std::string("Cannot create the renderer: ") + SDL_GetError());
  SDL_SetRenderDrawBlendMode(main_renderer, SDL_BLENDMODE_BLEND); // Allow blending mode for direct drawing primitives.

  // Get the first renderer format which is a packed32 type and support alpha channel
  SDL_RendererInfo renderer_info;
  SDL_GetRendererInfo(main_renderer, &renderer_info);
  for(int i = 0; i < renderer_info.num_texture_formats; ++i) {
    
    if(SDL_PIXELTYPE(renderer_info.texture_formats[i]) == SDL_PIXELTYPE_PACKED32
        && SDL_ISPIXELFORMAT_ALPHA(renderer_info.texture_formats[i])) {
      pixel_format = SDL_AllocFormat(renderer_info.texture_formats[i]);
      break;
    }
  }
  
  Debug::check_assertion(pixel_format != NULL, "No compatible pixel format");
}

/**
 * \brief Show the window.
 */
void VideoManager::show_window() {
  SDL_ShowWindow(main_window);
}

/**
 * \brief Returns whether the current video mode is in fullscreen.
 * \return true if the current video mode is in fullscreen.
 */
bool VideoManager::is_fullscreen() const {
  
  return fullscreen;
}

/**
 * \brief Sets the video mode to fullscreen or windowed,
 * keeping an equivalent resolution.
 * \param fullscreen true to make fullscreen.
 */
void VideoManager::set_fullscreen(bool fullscreen) {

  if (fullscreen != is_fullscreen()) {
    switch_fullscreen();
  }
}

/**
 * \brief Switches from windowed to fullscreen or from fullscreen to windowed,
 * keeping an equivalent resolution.
 */
void VideoManager::switch_fullscreen() {

  fullscreen = !fullscreen;
  
  if (is_mode_supported(video_mode)) {
    set_video_mode(video_mode);
  }
}

/**
 * \brief Sets the default video mode.
 */
void VideoManager::set_default_video_mode() {

  VideoMode* mode;
  if (forced_mode_name != "") {
    mode = get_video_mode_by_name(forced_mode_name);
  }
  else {
    mode = get_video_mode_by_name("normal");
  }
  
  set_video_mode(mode);
}

/**
 * \brief Sets the next video mode.
 */
void VideoManager::switch_video_mode() {
  
  std::vector<VideoMode*>::const_iterator it = find(all_video_modes.begin(), all_video_modes.end(), video_mode);
  VideoMode* mode;
  do {
    if (it == all_video_modes.end())
      it = all_video_modes.begin();
    mode = *(++it);
  } while (!is_mode_supported(mode));
  set_video_mode(mode);
}

/**
 * \brief Returns whether a video mode is supported.
 * \param mode a video mode
 * \return true if this mode is supported
 */
bool VideoManager::is_mode_supported(VideoMode* mode) const {
  
  if (mode == NULL) {
    return false;
  }
  
  if (forced_mode_name != "" && mode->name != forced_mode_name) {
    return false;
  }
  
  std::vector<VideoMode*>::const_iterator it = all_video_modes.begin();
  for(; it != all_video_modes.end(); ++it) {
    if(*it == mode) {
      if ((*it)->window_size.is_flat()) {
        Debug::die(StringConcat() <<
            "Uninitialized size for video mode " << mode->name);
      }
      break;
    }
  }
  if (it == all_video_modes.end()) {
    // The initial detection of this mode failed.
    return false;
  }
  
  return true;
}

/**
 * \brief Sets the video mode.
 * \param mode The video mode to set.
 * \return true in case of success, false if this mode is not supported.
 */
bool VideoManager::set_video_mode(VideoMode* mode) {

  if (!is_mode_supported(mode)) {
    return false;
  }

  int show_cursor;
  Uint32 fullscreen_flag;
  if (is_fullscreen()) {
    fullscreen_flag = SDL_WINDOW_FULLSCREEN_DESKTOP;
    show_cursor = SDL_DISABLE;
  }
  else {
    fullscreen_flag = 0;
    show_cursor = SDL_ENABLE;
  }

  if (!disable_window) {

    const Rectangle& window_size = mode->window_size;
    Rectangle render_size = quest_size;

    // Initialize the window.
    // Set fullscreen flag first to set the size on the right mode.
    SDL_SetWindowFullscreen(main_window, fullscreen_flag);
    SDL_SetWindowSize(main_window, window_size.get_width(), window_size.get_height());
    SDL_RenderSetLogicalSize(main_renderer, render_size.get_width(), render_size.get_height());
    SDL_ShowCursor(show_cursor);
    if(!fullscreen_flag) {
      SDL_SetWindowPosition(main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
  }
  this->video_mode = mode;

  return true;
}

/**
 * \brief Returns the current video mode.
 * \return The video mode.
 */
VideoManager::VideoMode* VideoManager::get_video_mode() const {
  return video_mode;
}

/**
 * \brief Returns a list of all supported video modes.
 * \return The list of supported video modes.
 */
const std::vector<VideoManager::VideoMode*>& VideoManager::get_video_modes() const {
  
  return all_video_modes;
}

/**
 * \brief Returns the name of a video mode.
 * \param mode A video mode.
 * \return The name of this mode, or an empty string if the mode is NO_MODE.
 */
std::string VideoManager::get_video_mode_name(VideoMode* mode) {

  if (mode)
    return mode->name;
  
  return "";
}

/**
 * \brief Returns a video mode given its Lua name.
 * \param mode_name Lua name of a video mode.
 * \return The corresponding video mode, or NO_MODE if the name is invalid.
 */
VideoManager::VideoMode* VideoManager::get_video_mode_by_name(const std::string& mode_name) {

  std::vector<VideoMode*> all_video_modes = get_instance()->all_video_modes;
  for (int i = 0; i < all_video_modes.size(); ++i) {
    if (all_video_modes.at(i)->name == mode_name) {
      return all_video_modes.at(i);
    }
  }
  return NULL;
}

/**
 * \brief Draws the quest surface on the screen with the current video mode.
 * \param quest_surface The quest surface to render on the screen.
 */
void VideoManager::render(Surface& quest_surface) {

  if (disable_window) {
    return;
  }

  SDL_RenderSetClipRect(main_renderer, NULL);
  SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
  SDL_RenderClear(main_renderer);
  quest_surface.render(main_renderer);
  
  // Render to the window, and apply a shader if we have to.
  VideoMode* video_mode = get_instance()->video_mode;
  if (video_mode->shader != NULL)
    video_mode->shader->render_present_shaded(main_renderer);
  else 
    SDL_RenderPresent(main_renderer);
}

/**
 * \brief Returns the current text of the window title bar.
 * \return The window title.
 */
const std::string VideoManager::get_window_title() const {

  return SDL_GetWindowTitle(main_window);
}

/**
 * \brief Sets the text of the window title bar.
 * \param window_title The window title to set.
 */
void VideoManager::set_window_title(const std::string& window_title) {

  SDL_SetWindowTitle(main_window, window_title.c_str());
}

/**
 * \brief Gets the width and the height values from a size string of the form
 * "320x240".
 * \param size_string The input string.
 * \param size The resulting size. Unchanged in case of failure.
 * \return true in case of success, false if the string is not a valid size.
 */
bool VideoManager::parse_size(const std::string& size_string, Rectangle& size) {

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
const Rectangle& VideoManager::get_quest_size() const {
  return quest_size;
}

/**
 * \brief Gets the allowed range of quest sizes for this quest.
 * \param normal_quest_size Gets the default size for this quest.
 * \param min_quest_size Gets the minimum size for this quest.
 * \param max_quest_size Gets the maximum size for this quest.
 */
void VideoManager::get_quest_size_range(
    Rectangle& normal_quest_size,
    Rectangle& min_quest_size,
    Rectangle& max_quest_size) const {

  normal_quest_size = this->normal_quest_size;
  min_quest_size = this->min_quest_size;
  max_quest_size = this->max_quest_size;
}

/**
 * \brief Sets the allowed range of quest sizes for this quest.
 *
 * This function sets the actual quest size and creates the screen.
 *
 * \param normal_quest_size Default size for this quest.
 * \param min_quest_size Minimum size for this quest.
 * \param max_quest_size Maximum size for this quest.
 */
void VideoManager::set_quest_size_range(
    const Rectangle& normal_quest_size,
    const Rectangle& min_quest_size,
    const Rectangle& max_quest_size) {

  Debug::check_assertion(
      normal_quest_size.get_width() >= min_quest_size.get_width()
      && normal_quest_size.get_height() >= min_quest_size.get_height()
      && normal_quest_size.get_width() <= max_quest_size.get_width()
      && normal_quest_size.get_height() <= max_quest_size.get_height(),
      "Invalid quest size range");

  this->normal_quest_size = normal_quest_size;
  this->min_quest_size = min_quest_size;
  this->max_quest_size = max_quest_size;

  if (wanted_quest_size.get_width() < min_quest_size.get_width()
      || wanted_quest_size.get_height() < min_quest_size.get_height()
      || wanted_quest_size.get_width() > max_quest_size.get_width()
      || wanted_quest_size.get_height() > max_quest_size.get_height()) {
    Debug::warning(StringConcat() <<
        "Cannot use quest size "
        << wanted_quest_size.get_width() << "x" << wanted_quest_size.get_height()
        << ": this quest only supports "
        << min_quest_size.get_width() << "x" << min_quest_size.get_height()
        << " to "
        << max_quest_size.get_width() << "x" << max_quest_size.get_height()
        << ". Using "
        << normal_quest_size.get_width() << "x" << normal_quest_size.get_height()
        << " instead.");
    quest_size = normal_quest_size;
  }
  else {
    quest_size = wanted_quest_size;
  }
}

/**
 * \brief Detects the available shaders and initialize all needed video modes.
 * Fullscreen modes all are at the top of the list.
 * \param skip_shaded_modes true to skip shaded modes loading.
 */
void VideoManager::initialize_video_modes(bool skip_shaded_modes) {

  // Initialize non-shaded video modes.
  const Rectangle quest_size_2(0, 0, quest_size.get_width() * 2, quest_size.get_height() * 2);
  all_video_modes.push_back(new VideoMode("normal", quest_size_2, NULL));
  
  if (!skip_shaded_modes) {
    //TODO remove the following, get all shaders of the quest's shader/driver folder and initialize them.
    Shader* video_mode_shader = new Shader("scale2x");
    add_shader(*video_mode_shader);
  
    // Add a mode for each shader.
    for(int i=0 ; i<supported_shaders.size() ; ++i) {
      const Rectangle scaled_quest_size(0, 0, 
          double(quest_size.get_width()) * supported_shaders.at(i)->get_logical_scale(),
          double(quest_size.get_height()) * supported_shaders.at(i)->get_logical_scale());
      all_video_modes.push_back(new VideoMode(supported_shaders.at(i)->get_name(),
        scaled_quest_size,
        supported_shaders.at(i)));
    }
  }
  
  // Everything is ready now.
  set_default_video_mode();
}

/**
 * \brief Add a Shader instance to the vector of supported shaders.
 * \param shader a Shader instance.
 */
void VideoManager::add_shader(Shader& shader) {
  supported_shaders.push_back(&shader);
}

