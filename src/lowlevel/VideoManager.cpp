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
const VideoManager::VideoMode forced_mode =
  VideoManager::VideoMode(SOLARUS_SCREEN_FORCE_MODE);
#else
// Make all modes available.
const VideoManager::VideoMode forced_mode = VideoManager::NO_MODE;
#endif

// List of fullscreen resolutions available on the system.
std::vector<Rectangle> available_fullscreen_resolutions;

}

/**
 * \brief Lua name of each value of the VideoMode enum.
 */
const std::string VideoManager::video_mode_names[] = {
  "windowed_stretched",
  "windowed_scale2x",
  "windowed_normal",
  "fullscreen_normal",
  "fullscreen_scale2x",
  ""  // Sentinel.
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
 * \return the only video manager
 */
VideoManager* VideoManager::get_instance() {
  return instance;
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
  main_window(NULL),
  main_renderer(NULL),
  screen_surface(NULL),
  screen_texture(NULL),
  outset_title(std::string("Solarus ") + SOLARUS_VERSION),
  video_mode(NO_MODE),
  wanted_quest_size(wanted_quest_size) {
}

/**
 * \brief Destructor.
 */
VideoManager::~VideoManager() {

  if(is_fullscreen()) {
    // Get back on desktop before destroy the window.
    SDL_SetWindowFullscreen(main_window, 0);
  }
  delete screen_surface;
  if (screen_texture != NULL) {
    SDL_DestroyTexture(screen_texture);
  }
  if (main_renderer != NULL) {
    SDL_DestroyRenderer(main_renderer);
  }
  if (main_window != NULL) {
    SDL_DestroyWindow(main_window);
  }
}

/**
 * \brief Create the window.
 */
void VideoManager::create_window() {
  
  // Initialize the window.
  Rectangle window_size = mode_sizes[video_mode];
  main_window = SDL_CreateWindow(outset_title.c_str(), 
    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    window_size.get_width(), window_size.get_height(), 
    SDL_WINDOW_SHOWN);
  if(!main_window)
    Debug::die(std::string("Cannot create the window : ") + SDL_GetError());
  
  main_renderer = SDL_CreateRenderer(main_window, -1, 0);
  if(!main_renderer)
    Debug::die(std::string("Cannot create the renderer : ") + SDL_GetError());
  
  set_video_mode(video_mode);
}

/**
 * \brief Returns whether a video mode is supported.
 * \param mode a video mode
 * \return true if this mode is supported
 */
bool VideoManager::is_mode_supported(VideoMode mode) const {

  if (mode == NO_MODE) {
    return false;
  }

  if (forced_mode != NO_MODE && mode != forced_mode) {
    return false;
  }

  std::map<VideoMode, Rectangle>::const_iterator it = mode_sizes.find(mode);
  if (it == mode_sizes.end()) {
    // The initial detection of this mode failed.
    return false;
  }

  const Rectangle& size = it->second;

  if (size.is_flat()) {
    Debug::die(StringConcat() <<
        "Uninitialized size for video mode " << get_video_mode_name(video_mode));
  }

  return true;
}

/**
 * \brief Returns whether a video mode is in fullscreen.
 * \param mode A video mode.
 * \return true if this video mode is in fullscreen.
 */
bool VideoManager::is_fullscreen(VideoMode mode) const {
  return mode >= FULLSCREEN_NORMAL;
}

/**
 * \brief Returns whether the current video mode is in fullscreen.
 * \return true if the current video mode is in fullscreen.
 */
bool VideoManager::is_fullscreen() const {
  return is_fullscreen(get_video_mode());
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

  static const VideoMode next_modes[] = {
      FULLSCREEN_NORMAL,      // WINDOWED_STRETCHED
      FULLSCREEN_SCALE2X,     // WINDOWED_SCALE2X
      FULLSCREEN_NORMAL,      // WINDOWED_NORMAL
      WINDOWED_STRETCHED,     // FULLSCREEN_NORMAL
      WINDOWED_SCALE2X,       // FULLSCREEN_SCALE2X
  };

  VideoMode mode = next_modes[get_video_mode()];
  if (is_mode_supported(mode)) {
    set_video_mode(mode);
  }
}

/**
 * \brief Sets the next video mode.
 */
void VideoManager::switch_video_mode() {

  VideoMode mode = video_mode;
  do {
    mode = (VideoMode) ((mode + 1) % NB_MODES);
  } while (!is_mode_supported(mode));
  set_video_mode(mode);
}

/**
 * \brief Sets the default video mode.
 */
void VideoManager::set_default_video_mode() {

  if (forced_mode != NO_MODE) {
    video_mode = forced_mode;
  }
  else {
    video_mode = WINDOWED_STRETCHED;
  }
}

/**
 * \brief Sets the video mode.
 * \param mode The video mode to set.
 * \return true in case of success, false if this mode is not supported.
 */
bool VideoManager::set_video_mode(VideoMode mode) {

  if (!is_mode_supported(mode)) {
    return false;
  }
  
  if(!main_window) {
    // If the window isn't created yet, just store the video mode.
    this->video_mode = mode;
    return true;
  }

  int show_cursor;
  Uint32 fullscreen_flag;
  if (is_fullscreen(mode)) {
    fullscreen_flag = SDL_WINDOW_FULLSCREEN;
    show_cursor = SDL_DISABLE;
  }
  else {
    fullscreen_flag = 0;
    show_cursor = SDL_ENABLE;
  }
  
  const Rectangle& mode_size = mode_sizes[mode];

  if (!disable_window) {
    // Get the render texture source size.
    const Rectangle& render_size = mode == WINDOWED_SCALE2X || mode == FULLSCREEN_SCALE2X ?
        mode_sizes[WINDOWED_SCALE2X] :
        mode_sizes[WINDOWED_NORMAL];
    
    double src_width = double(render_size.get_width());
    double src_height = double(render_size.get_height());
    double dst_width = double(mode_size.get_width());
    double dst_height = double(mode_size.get_height());
    
    // Get the render texture position and destination size on the window.
    double ratio = std::min(dst_width/src_width, dst_height/src_height);
    render_position = Rectangle(
        (dst_width - (src_width*ratio)) / 2,
        (dst_height - (src_height*ratio)) / 2,
        src_width * ratio,
        src_height * ratio);
    
    // Initialize the video mode.
    SDL_Surface* screen_internal_surface = SDL_CreateRGBSurface(0, 
      render_size.get_width(),
      render_size.get_height(), 
      32,
      0x00FF0000,
      0x0000FF00,
      0x000000FF,
      0xFF000000);
    
    delete this->screen_surface;
    this->screen_surface = new Surface(screen_internal_surface);
    
    if(screen_texture)
      SDL_DestroyTexture(screen_texture);
    screen_texture = SDL_CreateTexture(main_renderer,
      SDL_PIXELFORMAT_ARGB8888,
      SDL_TEXTUREACCESS_STREAMING,
      render_size.get_width(),
      render_size.get_height());
    
    SDL_SetWindowFullscreen(main_window, fullscreen_flag);
    SDL_SetWindowSize(main_window, mode_size.get_width(), mode_size.get_height());
    SDL_ShowCursor(show_cursor);
  }
  this->video_mode = mode;

  return true;
}

/**
 * \brief Returns the current video mode.
 * \return The video mode.
 */
VideoManager::VideoMode VideoManager::get_video_mode() const {
  return video_mode;
}

/**
 * \brief Returns a list of all supported video modes.
 * \return The list of supported video modes.
 */
const std::list<VideoManager::VideoMode> VideoManager::get_video_modes() const {

  std::list<VideoManager::VideoMode> modes;
  for (int i = 0; i < NB_MODES; i++) {
    VideoMode mode = VideoMode(i);
    if (is_mode_supported(mode)) {
      modes.push_back(mode);
    }
  }
  return modes;
}

/**
 * \brief Returns the name of a video mode.
 * \param mode A video mode.
 * \return The name of this mode, or an empty string if the mode is NO_MODE.
 */
std::string VideoManager::get_video_mode_name(VideoMode mode) {

  if (mode == NO_MODE) {
    return "";
  }

  return video_mode_names[mode];
}

/**
 * \brief Returns a video mode given its Lua name.
 * \param mode_name Lua name of a video mode.
 * \return The corresponding video mode, or NO_MODE if the name is invalid.
 */
VideoManager::VideoMode VideoManager::get_video_mode_by_name(const std::string& mode_name) {

  for (int i = 0; i < NB_MODES; i++) {
    if (video_mode_names[i] == mode_name) {
      return VideoMode(i);
    }
  }
  return NO_MODE;
}

/**
 * \brief Draws the quest surface on the screen with the current video mode.
 * \param quest_surface The quest surface to draw on the screen.
 */
void VideoManager::draw(Surface& quest_surface) {

  if (disable_window) {
    return;
  }
  
  if (video_mode == WINDOWED_SCALE2X
      || video_mode == FULLSCREEN_SCALE2X) {
    draw_scale2x(quest_surface);
  }
  else {
    draw_unscaled(quest_surface);
  }
  
  // Update the internal texture with the internal surface, and render it.
  SDL_Surface* screen_sdl_surface = screen_surface->get_internal_surface();
  SDL_UpdateTexture(screen_texture, NULL, screen_sdl_surface->pixels, screen_sdl_surface->pitch);
  SDL_RenderClear(main_renderer);
  SDL_RenderCopy(main_renderer, screen_texture, NULL, render_position.get_internal_rect());
  SDL_RenderPresent(main_renderer);
}
  
/**
 * \brief Draws the quest surface on the screen at its size.
 *
 * Black bars are added if the screen is bigger than the quest size.
 *
 * \param quest_surface The quest surface to draw.
 */
void VideoManager::draw_unscaled(Surface& quest_surface) {

  quest_surface.draw(*screen_surface);
}

/**
 * \brief Draws the quest surface on the screen, scaled the image by
 * a factor of 2 with the Scale2x algorithm.
 *
 * Black bars are added if the screen is bigger than twice the quest size.
 *
 * \param quest_surface The quest surface to draw.
 */
void VideoManager::draw_scale2x(Surface& quest_surface) {

    SDL_Surface* src_internal_surface = quest_surface.get_internal_surface();
    SDL_Surface* dst_internal_surface = screen_surface->get_internal_surface();

    SDL_LockSurface(src_internal_surface);
    SDL_LockSurface(dst_internal_surface);

    uint32_t* src = (uint32_t*) src_internal_surface->pixels;
    uint32_t* dst = (uint32_t*) dst_internal_surface->pixels;

    const int end_row_increment = dst_internal_surface->w;

    int e1 = 0;
    int e2, e3, e4;
    int b, d, e = 0, f,  h;
    for (int row = 0; row < quest_size.get_height(); row++) {
        for (int col = 0; col < quest_size.get_width(); col++) {

            // compute a to i

            b = e - quest_size.get_width();
            d = e - 1;
            f = e + 1;
            h = e + quest_size.get_width();

            if (row == 0) {
              b = e;
            }
            if (row == quest_size.get_height() - 1) {
              h = e;
            }
            if (col == 0) {
              d = e;
            }
            if (col == quest_size.get_width() - 1) {
              f = e;
            }

            // compute e1 to e4
            e2 = e1 + 1;
            e3 = e1 + dst_internal_surface->w;
            e4 = e3 + 1;

            // compute the color

            if (src[b] != src[h] && src[d] != src[f]) {
                dst[e1] = quest_surface.get_mapped_pixel((src[d] == src[b]) ? d : e, dst_internal_surface->format);
                dst[e2] = quest_surface.get_mapped_pixel((src[b] == src[f]) ? f : e, dst_internal_surface->format);
                dst[e3] = quest_surface.get_mapped_pixel((src[d] == src[h]) ? d : e, dst_internal_surface->format);
                dst[e4] = quest_surface.get_mapped_pixel((src[h] == src[f]) ? f : e, dst_internal_surface->format);
            }
            else {
                dst[e1] = dst[e2] = dst[e3] = dst[e4] = quest_surface.get_mapped_pixel(e, dst_internal_surface->format);
            }
            e1 += 2;
            e++;
        }
        e1 += end_row_increment;
    }

    SDL_UnlockSurface(dst_internal_surface);
    SDL_UnlockSurface(src_internal_surface);
}

/**
 * \brief Returns the current text of the window title bar.
 * \return The window title.
 */
const std::string VideoManager::get_window_title() const {

  if(main_window) {
    return SDL_GetWindowTitle(main_window);
  }
  
  return outset_title;
}

/**
 * \brief Sets the text of the window title bar.
 * \param window_title The window title to set.
 */
void VideoManager::set_window_title(const std::string& window_title) {

  if(main_window) {
    SDL_SetWindowTitle(main_window, window_title.c_str());
  }
  else {
    // If the window isn't created yet, just store the title.
    outset_title = window_title;
  }
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

  Debug::check_assertion(screen_texture == NULL,
      "The screen is already created");

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

  // Everything is ready now.
  initialize_video_modes();
  set_default_video_mode();
}

/**
 * \brief Returns the closest fullscreen resolution greater than or equal to
 * the specified size.
 * \param surface_size The size of the surface to fit.
 * \return The lowest fullscreen resolution that can contain this surface.
 */
Rectangle VideoManager::find_closest_fullscreen_resolution(const Rectangle& surface_size) {
  
  // Find the closest fullscreen resolution that can contain the requested size.
  SDL_DisplayMode target = {surface_size.get_width(), surface_size.get_height(), 0, 0, 0};
  SDL_DisplayMode closest;
  
  if(!SDL_GetClosestDisplayMode(0, &target, &closest)) {
    Debug::die(StringConcat() << "No suitable display mode was found for size" 
      << surface_size.get_width() << "x" << surface_size.get_height());
  }
     
  return Rectangle(0, 0, closest.w, closest.h);
}
     
/**
 * \brief Detects the available resolutions and initializes the properties
 * of video modes.
 */
void VideoManager::initialize_video_modes() {

  const Rectangle twice_quest_size(
      0, 0, quest_size.get_width() * 2, quest_size.get_height() * 2);

  mode_sizes[WINDOWED_STRETCHED] = twice_quest_size;
  mode_sizes[WINDOWED_SCALE2X] = twice_quest_size;
  mode_sizes[WINDOWED_NORMAL] = quest_size;
  
  mode_sizes[FULLSCREEN_SCALE2X] = find_closest_fullscreen_resolution(twice_quest_size);
  mode_sizes[FULLSCREEN_NORMAL] = find_closest_fullscreen_resolution(quest_size);
}

