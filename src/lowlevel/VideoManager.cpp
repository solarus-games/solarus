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

/**
 * @brief Returns the lowest fullscreen resolution greater than or equal to
 * the specified size.
 * @param surface_size The size of the surface to fit.
 * @return The lowest fullscreen resolution that can contain this surface, or
 * an rectangle of size 0.
 */
Rectangle find_lowest_fullscreen_resolution(const Rectangle& surface_size) {

  // Find the lowest fullscreen resolution that can contain the requested size.
  std::vector<Rectangle>::reverse_iterator it;
  for (it = available_fullscreen_resolutions.rbegin();
      it != available_fullscreen_resolutions.rend();
      ++it) {
    // Reverse iterator because SDL_ListModes gives resolutions sorted from
    // the highest one to the lowest one.
    const Rectangle& resolution = *it;
    if (resolution.get_width() >= surface_size.get_width()
        && resolution.get_height() >= surface_size.get_height()) {
      // Found one.
      return resolution;
    }
  }

  return Rectangle();
}

/**
 * @brief Returns a fullscreen resolution whose width is larger than the
 * specified width and whose height is minimal.
 * @param surface_size The size of the surface to fit.
 * @return The lowest wide fullscreen resolution that can contain this surface,
 * or a rectangle of size 0.
 */
Rectangle find_wide_fullscreen_resolution(const Rectangle& surface_size) {

  int best_height = 1e6;
  Rectangle best_resolution;

  std::vector<Rectangle>::iterator it;
  for (it = available_fullscreen_resolutions.begin();
      it != available_fullscreen_resolutions.end();
      ++it) {
    const Rectangle& resolution = *it;
    if (resolution.get_width() > surface_size.get_width()
        && resolution.get_height() >= surface_size.get_height()
        && resolution.get_height() < best_height) {
      // Found a better one.
      best_height = resolution.get_height();
      best_resolution = resolution;
    }
  }

  return best_resolution;
}

}

/**
 * @brief Lua name of each value of the VideoMode enum.
 */
const std::string VideoManager::video_mode_names[] = {
  "windowed_stretched",
  "windowed_scale2x",
  "windowed_normal",
  "fullscreen_normal",
  "fullscreen_wide",
  "fullscreen_scale2x",
  "fullscreen_scale2x_wide",
  ""  // Sentinel.
};

/**
 * @brief Initializes the video system and creates the window.
 *
 * This method should be called when the application starts.
 * Options "-no-video" and "-quest-size=<width>x<height>" are recognized.
 *
 * @param argc Command-line arguments number.
 * @param argv Command-line arguments.
 */
void VideoManager::initialize(int argc, char **argv) {
  // TODO pass options as an std::map<string> instead.

  // check the -no-video and the -quest-size options.
  bool disable = false;
  std::string quest_size_string;
  for (argv++; argc > 1; argv++, argc--) {
    const std::string arg = *argv;
    if (arg == "-no-video") {
      disable = true;
    }
    else if (arg.find("-quest_size=") == 0) {
      quest_size_string = arg.substr(12);
    }
  }

  instance = new VideoManager(disable);

  if (!quest_size_string.empty()) {
    Rectangle quest_size;
    if (!parse_size(quest_size_string, quest_size)) {
      Debug::error(std::string("Invalid quest size: '") + quest_size_string + "'");
    }
    else {
      instance->set_quest_size(quest_size);
    }
  }
}

/**
 * @brief Closes the video system.
 */
void VideoManager::quit() {
  delete instance;
}

/**
 * @brief Returns the video manager.
 * @return the only video manager
 */
VideoManager* VideoManager::get_instance() {
  return instance;
}

/**
 * @brief Returns the appropriate SDL_Surface flag depending on the requested
 * display mode and what OS is running.
 * @param the display mode which you wanted to know the SDL_Surface flag to use with.
 * @return the better SDL_Surface flag to use
 */
uint32_t VideoManager::get_surface_flag(const VideoMode mode) const {

  uint32_t flag = SDL_SWSURFACE;

  if (is_fullscreen(mode)) {
    flag |= SDL_FULLSCREEN;
  }

#if defined(SOLARUS_SCREEN_DOUBLEBUF) && SOLARUS_SCREEN_DOUBLEBUF == 1
  flag |= SDL_DOUBLEBUF;
#endif

  return flag;
}

/**
 * @brief Constructor.
 */
VideoManager::VideoManager(bool disable_window):
  disable_window(disable_window),
  video_mode(NO_MODE),
  screen_surface(NULL),
  intermediate_screen_surface(NULL),
  enlargment_factor(1),
  offset_x(0),
  offset_y(0),
  normal_quest_size(0, 0, 320, 240),
  min_quest_size(normal_quest_size),
  max_quest_size(normal_quest_size),
  quest_size(normal_quest_size) {

  // Initialize the window.
  const std::string& window_title = std::string("Solarus ") + SOLARUS_VERSION;
  set_window_title(window_title);
  putenv((char*) "SDL_VIDEO_CENTERED=center");
  putenv((char*) "SDL_NOMOUSE");
}

/**
 * @brief Destructor.
 */
VideoManager::~VideoManager() {

  delete screen_surface;
  delete intermediate_screen_surface;
}

/**
 * @brief Returns whether a video mode is supported.
 * @param mode a video mode
 * @return true if this mode is supported
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

#if defined(SOLARUS_FULLSCREEN_FORCE_OK) && SOLARUS_FULLSCREEN_FORCE_OK == 1
  if (is_fullscreen(mode)) {
    return true;
  }
#endif

  int flags = get_surface_flag(mode);
  return SDL_VideoModeOK(size.get_width(), size.get_height(), 32, flags) != 0;
}

/**
 * @brief Returns whether a video mode is in fullscreen.
 * @param mode A video mode.
 * @return true if this video mode is in fullscreen.
 */
bool VideoManager::is_fullscreen(VideoMode mode) const {
  return mode >= FULLSCREEN_NORMAL;
}

/**
 * @brief Returns whether the current video mode is in fullscreen.
 * @return true if the current video mode is in fullscreen.
 */
bool VideoManager::is_fullscreen() const {
  return is_fullscreen(get_video_mode());
}

/**
 * @brief Sets the video mode to fullscreen or windowed,
 * keeping an equivalent resolution.
 * @param fullscreen true to make fullscreen.
 */
void VideoManager::set_fullscreen(bool fullscreen) {

  if (fullscreen != is_fullscreen()) {
    switch_fullscreen();
  }
}

/**
 * @brief Switches from windowed to fullscreen or from fullscreen to windowed,
 * keeping an equivalent resolution.
 */
void VideoManager::switch_fullscreen() {

  static const VideoMode next_modes[] = {
      FULLSCREEN_NORMAL,      // WINDOWED_STRETCHED
      FULLSCREEN_SCALE2X,     // WINDOWED_SCALE2X
      FULLSCREEN_NORMAL,      // WINDOWED_NORMAL
      WINDOWED_STRETCHED,     // FULLSCREEN_NORMAL
      WINDOWED_STRETCHED,     // FULLSCREEN_WIDE
      WINDOWED_SCALE2X,       // FULLSCREEN_SCALE2X
      WINDOWED_SCALE2X,       // FULLSCREEN_SCALE2X_WIDE
  };

  VideoMode mode = next_modes[get_video_mode()];
  if (is_mode_supported(mode)) {
    set_video_mode(mode);
  }
}

/**
 * @brief Sets the next video mode.
 */
void VideoManager::switch_video_mode() {

  VideoMode mode = video_mode;
  do {
    mode = (VideoMode) ((mode + 1) % NB_MODES);
  } while (!is_mode_supported(mode));
  set_video_mode(mode);
}

/**
 * @brief Sets the default video mode.
 */
void VideoManager::set_default_video_mode() {

  VideoMode mode;
  if (forced_mode != NO_MODE) {
    mode = forced_mode;
  }
  else {
    mode = WINDOWED_STRETCHED;
  }

  set_video_mode(mode);
}

/**
 * @brief Sets the video mode.
 * @param mode The video mode to set.
 * @return true in case of success, false if this mode is not supported.
 */
bool VideoManager::set_video_mode(VideoMode mode) {

  if (!is_mode_supported(mode)) {
    return false;
  }

  int flags = get_surface_flag(mode);
  int show_cursor;
  if (is_fullscreen(mode)) {
    show_cursor = SDL_DISABLE;
  }
  else {
    show_cursor = SDL_ENABLE;
  }

  const Rectangle& mode_size = mode_sizes[mode];
  Rectangle scaled_quest_size(
      quest_size.get_width() * enlargment_factor,
      quest_size.get_height() * enlargment_factor);

  if (mode_size.get_width() < 2 * quest_size.get_width()
      || mode_size.get_height() < 2 * quest_size.get_height()) {
    // The quest surface will be rendered directly.
    enlargment_factor = 1;
  }
  else {
    // The quest surface will be rendered stretched or scaled.
    enlargment_factor = 2;
  }

  // Add black bars if needed.
  offset_x = (mode_size.get_width() - scaled_quest_size.get_width()) / 2;
  offset_y = (mode_size.get_height() - scaled_quest_size.get_height()) / 2;

  if (!disable_window) {
    SDL_Surface* screen_internal_surface = SDL_SetVideoMode(
        mode_size.get_width(),
        mode_size.get_height(),
        SOLARUS_COLOR_DEPTH,
        flags);

    Debug::check_assertion(screen_internal_surface != NULL, StringConcat() <<
        "Cannot create the video surface for mode " << get_video_mode_name(mode));

    SDL_ShowCursor(show_cursor);
    delete this->screen_surface;
    this->screen_surface = new Surface(screen_internal_surface);

#if defined(SOLARUS_SCREEN_INTERMEDIATE_SURFACE) && SOLARUS_SCREEN_INTERMEDIATE_SURFACE != 0
    delete this->intermediate_screen_surface;
    this->intermediate_screen_surface = new Surface(screen_surface->get_size());
#endif
  }
  this->video_mode = mode;

  return true;
}

/**
 * @brief Returns the current video mode.
 * @return The video mode.
 */
VideoManager::VideoMode VideoManager::get_video_mode() const {
  return video_mode;
}

/**
 * @brief Returns a list of all supported video modes.
 * @return The list of supported video modes.
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
 * @brief Returns the name of a video mode.
 * @param mode A video mode.
 * @return The name of this mode, or an empty string if the mode is NO_MODE.
 */
std::string VideoManager::get_video_mode_name(VideoMode mode) {

  if (mode == NO_MODE) {
    return "";
  }

  return video_mode_names[mode];
}

/**
 * @brief Returns a video mode given its Lua name.
 * @param mode_name Lua name of a video mode.
 * @return The corresponding video mode, or NO_MODE if the name is invalid.
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
 * @brief Draws the quest surface on the screen with the current video mode.
 * @param quest_surface The quest surface to draw on the screen.
 */
void VideoManager::draw(Surface& quest_surface) {

  if (disable_window) {
    return;
  }

  if (enlargment_factor == 1) {
    draw_unscaled(quest_surface);
  }
  else if (video_mode == WINDOWED_SCALE2X
      || video_mode == FULLSCREEN_SCALE2X
      || video_mode == FULLSCREEN_SCALE2X_WIDE) {
    draw_scale2x(quest_surface);
  }
  else {
    draw_stretched(quest_surface);
  }

  SDL_Flip(screen_surface->get_internal_surface());
}

/**
 * @brief Draws the quest surface on the screen at its size.
 *
 * Black bars are added if the screen is bigger than the quest size.
 *
 * @param quest_surface The quest surface to draw.
 */
void VideoManager::draw_unscaled(Surface& quest_surface) {

  if (offset_x == 0 && offset_y == 0) {
    quest_surface.draw(*screen_surface);
  }
  else {
    quest_surface.draw(*screen_surface, Rectangle(offset_x, offset_y));
  }
}

/**
 * @brief Draws the quest surface on the screen, stretching the image by
 * a factor of 2.
 *
 * Black bars are added if the screen is bigger than twice the quest size.
 *
 * @param quest_surface The quest surface to draw.
 */
void VideoManager::draw_stretched(Surface& quest_surface) {

    SDL_Surface* src_internal_surface = quest_surface.get_internal_surface();
    SDL_Surface* dst_internal_surface = screen_surface->get_internal_surface();
    SDL_Surface* surface_to_draw = dst_internal_surface;

#if defined(SOLARUS_SCREEN_INTERMEDIATE_SURFACE) && SOLARUS_SCREEN_INTERMEDIATE_SURFACE != 0
    // On some systems, an intermediate surface is used as a workaround to rendering problems.
    surface_to_draw = intermediate_screen_surface->get_internal_surface();
#endif

    SDL_LockSurface(src_internal_surface);
    SDL_LockSurface(surface_to_draw);

    uint32_t* src = static_cast<uint32_t*>(src_internal_surface->pixels);
    uint32_t* dst = static_cast<uint32_t*>(surface_to_draw->pixels);

    const int width = dst_internal_surface->w;
    const int end_row_increment = 2 * offset_x + width;
    int p = width * offset_y + offset_x;
    for (int i = 0; i < quest_size.get_height(); i++) {
        for (int j = 0; j < quest_size.get_width(); j++) {
            dst[p] = dst[p + 1] = dst[p + width] = dst[p + width + 1] = *src;
            p += 2;
            src++;
        }

        p += end_row_increment;
    }

    SDL_UnlockSurface(surface_to_draw);
    SDL_UnlockSurface(src_internal_surface);

#if defined(SOLARUS_SCREEN_INTERMEDIATE_SURFACE) && SOLARUS_SCREEN_INTERMEDIATE_SURFACE != 0
    SDL_BlitSurface(surface_to_draw, NULL, dst_internal_surface, NULL);
#endif
}

/**
 * @brief Draws the quest surface on the screen, scaled the image by
 * a factor of 2 with the Scale2x algorithm.
 *
 * Black bars are added if the screen is bigger than twice the quest size.
 *
 * @param quest_surface The quest surface to draw.
 */
void VideoManager::draw_scale2x(Surface& quest_surface) {

    SDL_Surface* src_internal_surface = quest_surface.get_internal_surface();
    SDL_Surface* dst_internal_surface = screen_surface->get_internal_surface();
    SDL_Surface* surface_to_draw = dst_internal_surface;

#if defined(SOLARUS_SCREEN_INTERMEDIATE_SURFACE) && SOLARUS_SCREEN_INTERMEDIATE_SURFACE != 0
    // On some systems, an intermediate surface is used as a workaround to rendering problems.
    surface_to_draw = intermediate_screen_surface->get_internal_surface();
#endif

    SDL_LockSurface(src_internal_surface);
    SDL_LockSurface(surface_to_draw);

    uint32_t* src = (uint32_t*) src_internal_surface->pixels;
    uint32_t* dst = (uint32_t*) surface_to_draw->pixels;

    const int end_row_increment = 2 * offset_x + dst_internal_surface->w;

    int e1 = dst_internal_surface->w * offset_y + offset_x;
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
                dst[e1] = (src[d] == src[b]) ? src[d] : src[e];
                dst[e2] = (src[b] == src[f]) ? src[f] : src[e];
                dst[e3] = (src[d] == src[h]) ? src[d] : src[e];
                dst[e4] = (src[h] == src[f]) ? src[f] : src[e];
            }
            else {
                dst[e1] = dst[e2] = dst[e3] = dst[e4] = src[e];
            }
            e1 += 2;
            e++;
        }
        e1 += end_row_increment;
    }

    SDL_UnlockSurface(surface_to_draw);
    SDL_UnlockSurface(src_internal_surface);

#if defined(SOLARUS_SCREEN_INTERMEDIATE_SURFACE) && SOLARUS_SCREEN_INTERMEDIATE_SURFACE != 0
    SDL_BlitSurface(surface_to_draw, NULL, dst_internal_surface, NULL);
#endif
}

/**
 * @brief Returns the current text of the window title bar.
 * @return The window title.
 */
const std::string VideoManager::get_window_title() const {

  char* window_title;
  char* icon;
  SDL_WM_GetCaption(&window_title, &icon);
  return window_title;
}

/**
 * @brief Sets the text of the window title bar.
 * @param window_title The window title to set.
 */
void VideoManager::set_window_title(const std::string& window_title) {

  SDL_WM_SetCaption(window_title.c_str(), NULL);
}

/**
 * @brief Gets the width and the height values from a size string of the form
 * "320x240".
 * @param size_string The input string.
 * @param size The resulting size.
 * @return true in case of success, false if the string is not a valid size.
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
  if (!(iss >> height || height < 0)) {
    return false;
  }

  size.set_size(width, height);
  return true;
}

/**
 * @brief Returns the size of the quest surface to render on the screen.
 * @return The quest size.
 */
const Rectangle& VideoManager::get_quest_size() const {
  return quest_size;
}

/**
 * @brief Sets the size of the quest surface to render on the screen.
 *
 * This function should be called before video modes are initialized.
 *
 * @param quest_size The quest size to set.
 */
void VideoManager::set_quest_size(Rectangle& quest_size) {

  Debug::check_assertion(screen_surface == NULL,
      "The screen is already created");

  this->quest_size = quest_size;
}

/**
 * @brief Gets the allowed range of quest sizes for this quest.
 * @param normal_quest_size Gets the default size for this quest.
 * @param min_quest_size Gets the minimum size for this quest.
 * @param max_quest_size Gets the maximum size for this quest.
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
 * @brief Sets the allowed range of quest sizes for this quest.
 *
 * This function sets the final quest size if necessary
 * (because if may have become invalid) and creates the screen.
 *
 * @param normal_quest_size Default size for this quest.
 * @param min_quest_size Minimum size for this quest.
 * @param max_quest_size Maximum size for this quest.
 */
void VideoManager::set_quest_size_range(
    const Rectangle& normal_quest_size,
    const Rectangle& min_quest_size,
    const Rectangle& max_quest_size) {

  Debug::check_assertion(screen_surface == NULL,
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

  if (quest_size.get_width() < min_quest_size.get_width()
      || quest_size.get_height() < min_quest_size.get_height()
      || quest_size.get_width() > max_quest_size.get_width()
      || quest_size.get_height() > max_quest_size.get_height()) {
    quest_size = normal_quest_size;
  }

  // Everything is ready now.
  initialize_video_modes();
  set_default_video_mode();
}

/**
 * @brief Detects the available resolutions and initializes the properties
 * of video modes.
 */
void VideoManager::initialize_video_modes() {

  const Rectangle twice_quest_size(
      0, 0, quest_size.get_width() * 2, quest_size.get_height() * 2);

  mode_sizes[WINDOWED_STRETCHED] = twice_quest_size;
  mode_sizes[WINDOWED_SCALE2X] = twice_quest_size;
  mode_sizes[WINDOWED_NORMAL] =    quest_size;

  // Get the fullscreen video modes supported by the system.
  int fullscreen_flags = get_surface_flag(FULLSCREEN_NORMAL);
  SDL_Rect** rects = SDL_ListModes(NULL, fullscreen_flags);
  while (*rects != NULL) {
    available_fullscreen_resolutions.push_back(
        Rectangle(0, 0, (*rects)->w, (*rects)->h));
     rects++;
  }

  // Find a fullscreen resolution well suited for our quest size.
  Rectangle non_wide_resolution;
  Rectangle twice_non_wide_resolution;
  if (SDL_VideoModeOK(quest_size.get_width(), quest_size.get_height(),
      32, fullscreen_flags)) {
    // First try directly the resolution of the quest size.
    non_wide_resolution.set_size(quest_size);
  }

  if (SDL_VideoModeOK(twice_quest_size.get_width(), twice_quest_size.get_height(),
      32, fullscreen_flags)) {
    // The resolution of twice the quest size will be used by the scaled
    // video mode.
    twice_non_wide_resolution.set_size(twice_quest_size);

    if (non_wide_resolution.is_flat()) {
      // Use twice the size also for the non-scaled mode because many systems
      // accept 640x480 but not 320x240.
      // The image will be stretched by a factor of two and the user
      // won't see a difference.
      non_wide_resolution = twice_non_wide_resolution;
    }
  }

  if (non_wide_resolution.is_flat()) {
    // Find the lowest fullscreen resolution that can contain the quest size.
    // The image will be centered on a black background.
    non_wide_resolution = find_lowest_fullscreen_resolution(quest_size);
    if (non_wide_resolution.is_flat()) {
      // No fullscreen resolution was detected. This is weird.
      // Never mind: the user will play with a window. But there might be
      // something wrong with our detection of fullscreen resolutions.
      Debug::error("No available fullscreen resolution was detected.");
    }
  }
  if (!non_wide_resolution.is_flat()) {
    mode_sizes[FULLSCREEN_NORMAL] = non_wide_resolution;
  }

  if (twice_non_wide_resolution.is_flat()) {
    twice_non_wide_resolution = find_lowest_fullscreen_resolution(twice_quest_size);
  }
  if (!twice_non_wide_resolution.is_flat()) {
    mode_sizes[FULLSCREEN_SCALE2X] = twice_non_wide_resolution;
  }

  // Now let's find a wider resolution that can also contain the quest.
  // This will look better on wide screens if the quest size is 4:3.
  Rectangle wide_resolution = find_wide_fullscreen_resolution(quest_size);
  Rectangle twice_wide_resolution = find_wide_fullscreen_resolution(twice_quest_size);
  if (!wide_resolution.is_flat()) {
    // Maybe there is a better resolution when we show twice the size
    // (same reason as above).
    if (!twice_wide_resolution.is_flat()
        && twice_wide_resolution.get_height() / 2 < wide_resolution.get_height()) {
      // Twice the size is better because we will have less horizontal
      // black bars (or none).
      wide_resolution = twice_wide_resolution;
    }
  }

  if (!wide_resolution.is_flat()) {
    mode_sizes[FULLSCREEN_WIDE] = wide_resolution;
  }
  if (!twice_wide_resolution.is_flat()) {
    mode_sizes[FULLSCREEN_SCALE2X_WIDE] = wide_resolution;
  }

  for (int i = 0; i < NB_MODES; i++) {
    VideoMode mode = VideoMode(i);
    Debug::check_assertion(mode_sizes.find(mode) != mode_sizes.end(),
        StringConcat() << "Video mode " << get_video_mode_name(mode) << " was not initialized");
  }
}

