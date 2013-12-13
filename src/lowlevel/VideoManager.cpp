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

const std::string VideoManager::normal_mode_name = "solarus_default";
const std::string VideoManager::forced_mode_name = SOLARUS_SCREEN_FORCE_MODE;  // TODO remove

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
 * \brief Get the default rendering driver for the current platform.
 * \return a string containing the rendering driver name.
 */
std::string VideoManager::get_rendering_driver_name() {
  
  return rendering_driver_name;
}

/**
 * \brief Constructor.
 * \param disable_window true to entirely disable the displaying.
 * \param wanted_quest_size Size of the quest as requested by the user.
 */
VideoManager::VideoManager(
    bool disable_window,
    const Rectangle& wanted_quest_size):
  main_window(NULL),
  main_renderer(NULL),
  render_target(NULL),
  pixel_format(NULL),
  disable_window(disable_window),
  fullscreen(false),
  rendertarget_supported(false),
  shaders_supported(false),
  renderer_accelerated(false),
  scaled_surface(NULL),
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

  for(int i=0 ; i<all_video_modes.size() ; ++i) {
    if (all_video_modes.at(i)->shader != NULL) {
      delete all_video_modes.at(i)->shader;
    }
    delete all_video_modes.at(i);
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
}

/**
 * \brief Creates the window.
 */
void VideoManager::create_window() {

  Debug::check_assertion(main_window == NULL, "Window already exists");

  // Set OpenGL as the default renderer driver when available, to avoid using Direct3d.
  SDL_SetHintWithPriority(SDL_HINT_RENDER_DRIVER, "opengl", SDL_HINT_DEFAULT);

  // Set the default OpenGL built-in shader (nearest)
  SDL_SetHint(SDL_HINT_RENDER_OPENGL_SHADERS, "0");

  main_window = SDL_CreateWindow(
      (std::string("Solarus ") + SOLARUS_VERSION).c_str(),
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      wanted_quest_size.get_width(),
      wanted_quest_size.get_height(),
      SDL_WINDOW_HIDDEN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  Debug::check_assertion(main_window != NULL,
      std::string("Cannot create the window: ") + SDL_GetError());

  main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (main_renderer == NULL) {
      // Try without vsync.
      main_renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED);
  }

  Debug::check_assertion(main_renderer != NULL,
      std::string("Cannot create the renderer: ") + SDL_GetError());
  SDL_SetRenderDrawBlendMode(main_renderer, SDL_BLENDMODE_BLEND); // Allow blending mode for direct drawing primitives.

  // Get the first renderer format which is a packed32 type and supports alpha channel
  SDL_RendererInfo renderer_info;
  SDL_GetRendererInfo(main_renderer, &renderer_info);
  for (unsigned i = 0; i < renderer_info.num_texture_formats; ++i) {

    if (SDL_PIXELTYPE(renderer_info.texture_formats[i]) == SDL_PIXELTYPE_PACKED32
        && SDL_ISPIXELFORMAT_ALPHA(renderer_info.texture_formats[i])) {
      pixel_format = SDL_AllocFormat(renderer_info.texture_formats[i]);
      break;
    }
  }

  Debug::check_assertion(pixel_format != NULL, "No compatible pixel format");

  renderer_accelerated = (renderer_info.flags & SDL_RENDERER_ACCELERATED) != 0;
  if (renderer_accelerated) {
    std::cout << "2D acceleration: yes" << std::endl;
  }
  else {
    std::cout << "2D acceleration: no" << std::endl;
  }

  update_viewport();
  
  // Check renderer's flags
  rendertarget_supported = (renderer_info.flags & SDL_RENDERER_TARGETTEXTURE) != 0;
  rendering_driver_name = renderer_info.name;
}

/**
 * \brief Show the window.
 */
void VideoManager::show_window() {

  SDL_ShowWindow(main_window);
}

/**
<<<<<<< HEAD
 * \brief Returns whether 2D hardware acceleration is currently enabled.
 *
 * 2D acceleration is enabled if the system supports it, except when the
 * current video mode needs to work in software mode
 * (typically because of a pixel filter implemented in software).
 *
 * \return \c true if GPU acceleration is active.
 */
bool VideoManager::is_acceleration_enabled() const {

  void* pixel_filter = NULL;  // TODO
  return renderer_accelerated  // 2D acceleration must be available on the system.
      && pixel_filter == NULL;  // For now pixel filters are all implemented in software.
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
    mode = get_video_mode_by_name(normal_mode_name);
  }
  
  set_video_mode(mode);
}

/**
 * \brief Sets the next video mode.
 */
void VideoManager::switch_video_mode() {

  /* TODO remove
  std::vector<VideoMode*>::const_iterator it = all_video_modes.find(video_mode);
  VideoMode* mode;
  do {
    if (it == all_video_modes.end())
      it = all_video_modes.begin();
    mode = *(++it);
  } while (!is_mode_supported(mode));
  set_video_mode(mode);
  */
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

    viewport = mode->window_size;
    Rectangle render_size = quest_size;

/*
    // Initalize the scaling mode.
    if (mode == WINDOWED_SCALE2X || mode == FULLSCREEN_SCALE2X) {
      pixel_filter = &scale2x_filter;
    }
    else if (mode == WINDOWED_HQ4X || mode == FULLSCREEN_HQ4X) {
      pixel_filter = &hq4x_filter;
    }
    else {
      pixel_filter = NULL;
    }

    if (scaled_surface != NULL) {
      RefCountable::unref(scaled_surface);
      scaled_surface = NULL;
    }
    if (pixel_filter != NULL) {
      int factor = pixel_filter->get_scaling_factor();
      render_size.set_size(render_size.get_width() * factor, render_size.get_height() * factor);
      scaled_surface = Surface::create(render_size);
      scaled_surface->set_software_destination(true);
      scaled_surface->fill_with_color(Color::get_black());  // To initialize the internal surface.
      RefCountable::ref(scaled_surface);
    }
*/

    // Initialize the window.
    // Set fullscreen flag first to set the size on the right mode.
    SDL_SetWindowFullscreen(main_window, fullscreen_flag);
    SDL_SetWindowSize(main_window, viewport.get_width(), viewport.get_height());
    SDL_RenderSetLogicalSize(main_renderer, render_size.get_width(), render_size.get_height());
    SDL_ShowCursor(show_cursor);
    if (!fullscreen_flag) {
      SDL_SetWindowPosition(main_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    update_viewport();
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
 * \return The name of this mode.
 */
std::string VideoManager::get_video_mode_name(VideoMode& mode) {

  return mode.name;
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
  
  Debug::warning("No video mode for name : " + mode_name);
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

  /* TODO
  Surface* surface_to_render = NULL;
  if (pixel_filter != NULL) {
    Debug::check_assertion(scaled_surface != NULL,
        "Missing destination surface for scaling");
    quest_surface.apply_pixel_filter(*pixel_filter, *scaled_surface);
    surface_to_render = scaled_surface;
  }
  else {
    surface_to_render = &quest_surface;
  }

  SDL_RenderSetClipRect(main_renderer, NULL);
  SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
  SDL_RenderClear(main_renderer);
  surface_to_render->render(main_renderer);
  SDL_RenderPresent(main_renderer);
  */

  // Perform accelerated render ...
  if (shaders_supported) {
    shaded_render(quest_surface);
  }
  // ... or software one.
  else
  {
    // Do the software render.
    //SDL_RenderPresent(main_renderer);
  }
}

/**
 * \brief Draws the quest surface on the screen in a shader-allowed context.
 * It will perform the render using OpenGL API directly.
 */
void VideoManager::shaded_render(Surface& quest_surface) {

  // Initialize the render.
  float rendering_width, rendering_height;
  VideoMode* video_mode = get_instance()->video_mode;

  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity(); // Clear the window

  SDL_SetRenderDrawColor(main_renderer, 0, 0, 0, 255);
  SDL_RenderSetClipRect(main_renderer, NULL);
  SDL_RenderClear(main_renderer); // Clear the render target

  // Draw on the render target.
  quest_surface.render(main_renderer);

  // Render on the window using OpenGL, to apply a shader if we have to.
  glViewport(viewport.get_x(), viewport.get_y(), viewport.get_width(), viewport.get_height());

  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  SDL_GL_BindTexture(render_target, &rendering_width, &rendering_height);
  if (video_mode->shader != NULL) {
    //video_mode->shader->apply();
  }

  glBegin(GL_QUADS);
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f(-1.0f, 1.0f, 0.0f); // Top left
  glTexCoord2f(rendering_width, 0.0f);
  glVertex3f(1.0f, 1.0f, 0.0f); // Top right
  glTexCoord2f(rendering_width, rendering_height);
  glVertex3f(1.0f, -1.0f, 0.0f); // Bottom right
  glTexCoord2f(0.0f, rendering_height);
  glVertex3f(-1.0f, -1.0f, 0.0f); // Bottom left
  glEnd();

  // Restore default states.
  if (video_mode->shader != NULL) {
    //Shader::restore_default_shader_program();
  }
  SDL_GL_UnbindTexture(render_target);
  glDisable(GL_TEXTURE_2D);

  // And swap the window.
  SDL_GL_SwapWindow(main_window);
}

/**
 * \brief Update the internal viewport used with the better one.
 */
void VideoManager::update_viewport() {
  
  SDL_RenderGetViewport(main_renderer, viewport.get_internal_rect());
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
 * \brief Return a rectangle with absolute value (eq. relative to the point 0,0 of the window).
 * This is a workaround function. Some SDL context functions are relative to the window instead
 * of the viewport (SDL_RenderSetClipRect).
 * Use this function to get the expected behavior.
 * \param rect A rectangle with a position relative to the viewport.
 */
void VideoManager::set_absolute_position(Rectangle& rect) {

  const Rectangle& viewport = get_instance()->viewport;
  rect.add_xy(viewport.get_x(), viewport.get_y());
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
 * \param allow_shaded_modes true to skip shaded modes loading.
 */
void VideoManager::initialize_video_modes(bool allow_shaded_modes) {

  // Initialize non-shaded video mode...
  const Rectangle quest_size_2(0, 0, quest_size.get_width() * 2, quest_size.get_height() * 2);
  all_video_modes.push_back(new VideoMode(normal_mode_name, quest_size_2, NULL));
  
  // ... and shaded ones if shaders and render target are supported.
  shaders_supported = allow_shaded_modes && rendertarget_supported;
  if (shaders_supported) {

    // Initialize the render target
    render_target = SDL_CreateTexture(
        main_renderer,
        pixel_format->format,
        SDL_TEXTUREACCESS_TARGET,
        quest_size.get_width(),
        quest_size.get_height());
    SDL_SetTextureBlendMode(render_target, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(main_renderer, render_target);
    
    // Get all shaders of the quest's shader/filters/driver folder.
    std::vector<std::string> shader_names = 
        FileTools::data_files_enumerate("shaders/filters/" + get_rendering_driver_name(), false, true);

    for(int i=0 ; i<shader_names.size() ; ++i) {

      if (shader_names.at(i) == normal_mode_name) {
        Debug::warning("Forbidden video mode name : " + shader_names.at(i));
        continue;
      }

      // Load the shader and add the corresponding video mode.
      Shader* video_mode_shader = new Shader(shader_names.at(i));
      const Rectangle scaled_quest_size(0, 0, 
          double(quest_size.get_width()) * video_mode_shader->get_logical_scale(),
          double(quest_size.get_height()) * video_mode_shader->get_logical_scale());
      all_video_modes.push_back( new VideoMode(shader_names.at(i), scaled_quest_size, video_mode_shader) );
    }
  }

  // Everything is ready now.
  set_default_video_mode();
}
