/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "lowlevel/IniFile.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "Configuration.h"

VideoManager* VideoManager::instance = NULL;

Rectangle VideoManager::default_mode_sizes[] = {
  Rectangle(0, 0, 640, 480),         // WINDOWED_STRETCHED
  Rectangle(0, 0, 640, 480),         // WINDOWED_SCALE2X
  Rectangle(0, 0, 320, 240),         // WINDOWED_NORMAL
  Rectangle(0, 0, 640, 480),         // FULLSCREEN_NORMAL
  Rectangle(0, 0,   0,   0),         // FULLSCREEN_WIDE
  Rectangle(0, 0, 640, 480),         // FULLSCREEN_SCALE2X
  Rectangle(0, 0,   0,   0),         // FULLSCREEN_SCALE2X_WIDE
  Rectangle(0, 0, 640, 480),         // FULLSCREEN_CENTERED
  Rectangle(0, 0, 640, 400),         // FULLSCREEN_CENTERED_WIDE
};

/**
 * @brief Initializes the video system and creates the window.
 *
 * This method should be called when the application starts.
 * If the argument -no-video is provided, no window will be displayed
 * but all surfaces will exist internally.
 *
 * @param argc command-line arguments number
 * @param argv command-line arguments
 */
void VideoManager::initialize(int argc, char** argv) {

  // check the -no-video option
  bool disable = false;
  for (argv++; argc > 1 && !disable; argv++, argc--) {
    const std::string arg = *argv;
    disable = (arg.find("-no-video") == 0);
  }

  instance = new VideoManager(disable);
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
 * @brief Constructor.
 */
VideoManager::VideoManager(bool disable_window):
  disable_window(disable_window), screen_surface(NULL) {

  // initialize the window
  IniFile ini("quest.dat", IniFile::READ);
  ini.set_group("info");
  std::string title_bar = ini.get_string_value("title_bar"); // get the window title bar text (language-independent)
  SDL_WM_SetCaption(title_bar.c_str(), NULL);
  putenv((char*) "SDL_VIDEO_CENTERED=center");
  putenv((char*) "SDL_NOMOUSE");

  // detect what widescreen resolution is supported (768*480 or 720*480)

  for (int i = 0; i < NB_MODES; i++) {
    mode_sizes[i] = default_mode_sizes[i];
  }

  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN;
  if (SDL_VideoModeOK(768, 480, 32, flags)) {
    mode_sizes[FULLSCREEN_WIDE].set_size(768, 480);
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].set_size(768, 480);
    dst_position_wide.set_xy(64, 0);
  }
  else if (SDL_VideoModeOK(720, 480, 32, flags)) {
    mode_sizes[FULLSCREEN_WIDE].set_size(720, 480);
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].set_size(720, 480);
    dst_position_wide.set_xy(40, 0);
  }

  /* debug (see the fullscreen video modes supported)
     SDL_Rect **rects = SDL_ListModes(NULL, flags);
     while (*rects != NULL) {
     std::cout << "mode " << (*rects)->w << " x " << (*rects)->h << std::endl;
     rects++;
     }
     */

  set_initial_video_mode();
}

/**
 * @brief Destructor.
 */
VideoManager::~VideoManager() {
  delete screen_surface;
}

/**
 * @brief Returns whether a video mode is supported.
 * @param mode a video mode
 * @return true if this mode is supported
 */
bool VideoManager::is_mode_supported(VideoMode mode) {

  const Rectangle* size = &mode_sizes[mode];

  if (size->get_width() == 0) {
    return false;
  }

  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  if (is_fullscreen(mode)) {
    flags |= SDL_FULLSCREEN;
  }

  return SDL_VideoModeOK(size->get_width(), size->get_height(), 32, flags) != 0;
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
 * @brief Sets the initial video mode.
 *
 * The initial video mode is read from the configuration file if existing.
 * Otherwise, the default video mode is chosen.
 */
void VideoManager::set_initial_video_mode() {

  int value = Configuration::get_value("video_mode", -1);

  if (value < 0 || value >= NB_MODES) {
    set_default_video_mode();
  }
  else {
    set_video_mode((VideoMode) value);
  }
}

/**
 * @brief Sets the default video mode.
 */
void VideoManager::set_default_video_mode() {
  set_video_mode(WINDOWED_STRETCHED);
}

/**
 * @brief Sets the video mode.
 *
 * The specified video mode must be supported.
 *
 * @param mode the video mode
 */
void VideoManager::set_video_mode(VideoMode mode) {

  const Rectangle &size = mode_sizes[mode];

  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  int show_cursor;
  if (is_fullscreen(mode)) {
    flags |= SDL_FULLSCREEN;
    show_cursor = SDL_DISABLE;
  }
  else {
    show_cursor = SDL_ENABLE;
  }

  if (size.get_width() > 640) {
    dst_position_centered.set_xy(dst_position_wide.get_x() + 160, 0);
    width = size.get_width();
    offset = dst_position_wide.get_x();
  }
  else {
    dst_position_centered.set_xy(160, (mode == FULLSCREEN_CENTERED_WIDE) ? 80 : 120);
    width = 640;
    offset = 0;
  }
  end_row_increment = 2 * offset + width;

  if (!disable_window) {
    SDL_Surface* screen_internal_surface = SDL_SetVideoMode(size.get_width(), size.get_height(), 32, flags);

    Debug::check_assertion(screen_internal_surface != NULL, StringConcat() << "Cannot create the video surface for mode " << mode);

    SDL_ShowCursor(show_cursor);
    this->screen_surface = new Surface(screen_internal_surface);
  }
  this->video_mode = mode;

  // write the configuration file
  Configuration::set_value("video_mode", mode);
}

/**
 * @brief Returns the current video mode.
 * @return the video mode
 */
VideoManager::VideoMode VideoManager::get_video_mode() {
  return video_mode;
}

/**
 * @brief Returns whether a video mode is in fullscreen.
 * @param mode a video mode
 * @return true if this video mode is in fullscreen
 */
bool VideoManager::is_fullscreen(VideoMode mode) {
  return mode >= FULLSCREEN_NORMAL;
}

/**
 * @brief Returns whether the current video mode is in fullscreen.
 * @return true if this video mode is in fullscreen
 */
bool VideoManager::is_fullscreen() {
  return is_fullscreen(get_video_mode());
}

/**
 * @brief Switch from windowed to fullscreen or from fullscreen to windowed,
 * keeping an equivalent video mode.
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
      WINDOWED_STRETCHED,     // FULLSCREEN_CENTERED
      WINDOWED_STRETCHED,     // FULLSCREEN_CENTERED_WIDE
  };

  set_video_mode(next_modes[get_video_mode()]);
}

/**
 * @brief Blits a surface on the screen with the current video mode.
 * @param src_surface the source surface to display on the screen
 */
void VideoManager::display(Surface *src_surface) {

  if (disable_window) {
    return;
  }

  switch (video_mode) {

    case WINDOWED_NORMAL:
      blit(src_surface, screen_surface);
      break;

    case WINDOWED_STRETCHED:
    case FULLSCREEN_NORMAL:
    case FULLSCREEN_WIDE:
      blit_stretched(src_surface, screen_surface);
      break;

    case WINDOWED_SCALE2X:
    case FULLSCREEN_SCALE2X:
    case FULLSCREEN_SCALE2X_WIDE:
      blit_scale2x(src_surface, screen_surface);
      break;

    case FULLSCREEN_CENTERED:
    case FULLSCREEN_CENTERED_WIDE:
      blit_centered(src_surface, screen_surface);
      break;

    default:
      Debug::die(StringConcat() << "Unknown video mode " << video_mode);
      break;
  }

  SDL_Flip(screen_surface->get_internal_surface());
}

/**
 * @brief Blits a 320*240 surface on a 320*240 surface.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit(Surface* src_surface, Surface* dst_surface) {
  src_surface->blit(dst_surface);
}

/**
 * @brief Blits a 320*240 surface on a 640*480 or 640*400 surface, centering the image.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_centered(Surface* src_surface, Surface* dst_surface) {
  src_surface->blit(dst_surface, dst_position_centered);
}

/**
 * @brief Blits a 320*240 surface on a 640*480, 768*480 or 720*480 surface, stretching the image.
 *
 * Two black side bars are added if the destination surface is wider than 640.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_stretched(Surface* src_surface, Surface* dst_surface) {

  SDL_Surface *src_internal_surface = src_surface->get_internal_surface();
  SDL_Surface *dst_internal_surface = dst_surface->get_internal_surface();

  SDL_LockSurface(src_internal_surface);
  SDL_LockSurface(dst_internal_surface);

  uint32_t* src = (uint32_t*) src_internal_surface->pixels;
  uint32_t* dst = (uint32_t*) dst_internal_surface->pixels;

  int p = offset;
  for (int i = 0; i < 240; i++) {

    for (int j = 0; j < 320; j++) {
      dst[p] = dst[p + 1] = dst[p + width] = dst[p + width + 1] = *src;
      p += 2;
      src++;
    }

    p += end_row_increment;
  }

  SDL_UnlockSurface(dst_internal_surface);
  SDL_UnlockSurface(src_internal_surface);
}

/**
 * @brief Blits a 320*240 surface on a 640*480, 768*480 or 720*480 surface.
 *
 * The image is scaled with an implementation of the Scale2x algorithm.
 * Two black side bars if the destination surface is wider than 640.
 *
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_scale2x(Surface* src_surface, Surface* dst_surface) {

  SDL_Surface* src_internal_surface = src_surface->get_internal_surface();
  SDL_Surface* dst_internal_surface = dst_surface->get_internal_surface();

  SDL_LockSurface(src_internal_surface);
  SDL_LockSurface(dst_internal_surface);

  uint32_t *src = (uint32_t*) src_internal_surface->pixels;
  uint32_t *dst = (uint32_t*) dst_internal_surface->pixels;

  int b, d, e = 0, f,  h;
  int e1 = offset, e2, e3, e4;
  for (int row = 0; row < 240; row++) {
    for (int col = 0; col < 320; col++) {

      // compute a to i

      b = e - 320;
      d = e - 1;
      f = e + 1;
      h = e + 320;

      if (row == 0)   { b = e; }
      if (row == 239) { h = e; }
      if (col == 0)   { d = e; }
      if (col == 319) { f = e; }

      // compute e1 to e4
      e2 = e1 + 1;
      e3 = e1 + width;
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

  SDL_UnlockSurface(dst_internal_surface);
  SDL_UnlockSurface(src_internal_surface);
}

