/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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

VideoManager *VideoManager::instance = NULL;

// Resolutions.
#ifdef CAANOO
const VideoManager::VideoMode* VideoManager::proposed_modes = {
  VideoManager::WINDOWED_NORMAL,
  VideoManager::NO_MODE
};
#elif defined(PANDORA)
const VideoManager::VideoMode* VideoManager::proposed_modes = {
  VideoManager::FULLSCREEN_SCALE2X,
  VideoManager::NO_MODE
};
#else
const VideoManager::VideoMode* VideoManager::proposed_modes = NULL;
#endif

// Size of the screen in each video mode.
Rectangle VideoManager::default_mode_sizes[] = {
  Rectangle(0, 0, SOLARUS_SCREEN_WIDTH * 2, SOLARUS_SCREEN_HEIGHT * 2),  // WINDOWED_STRETCHED
  Rectangle(0, 0, SOLARUS_SCREEN_WIDTH * 2, SOLARUS_SCREEN_HEIGHT * 2),  // WINDOWED_SCALE2X
  Rectangle(0, 0, SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT),          // WINDOWED_NORMAL
  Rectangle(0, 0, SOLARUS_SCREEN_WIDTH * 2, SOLARUS_SCREEN_HEIGHT * 2),  // FULLSCREEN_NORMAL
  Rectangle(0, 0, 0, 0),                                             // FULLSCREEN_WIDE
  Rectangle(0, 0, SOLARUS_SCREEN_WIDTH * 2, SOLARUS_SCREEN_HEIGHT * 2),  // FULLSCREEN_SCALE2X
  Rectangle(0, 0, 0, 0),                                             // FULLSCREEN_SCALE2X_WIDE
};

// Properties of SDL surfaces.
#ifdef __APPLE__
// on Mac OS X the SDL hardware surfaces are buggy
const int VideoManager::surface_flags = SDL_SWSURFACE;
#else
const int VideoManager::surface_flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
#endif

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
void VideoManager::initialize(int argc, char **argv) {

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
  disable_window(disable_window),
  screen_surface(NULL) {

  // initialize the window
  IniFile ini("quest.dat", IniFile::READ);
  ini.set_group("info");
  std::string title_bar = ini.get_string_value("title_bar"); // get the window title bar text (language-independent)
  SDL_WM_SetCaption(title_bar.c_str(), NULL);
  putenv((char*) "SDL_VIDEO_CENTERED=center");
  putenv((char*) "SDL_NOMOUSE");

  // detect what widescreen resolution is supported (16:10 or 15:10)

  for (int i = 0; i < NB_MODES; i++) {
    mode_sizes[i] = default_mode_sizes[i];
  }
  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN;
  if (SDL_VideoModeOK(768, 480, 32, flags)) {
    mode_sizes[FULLSCREEN_WIDE].set_size(768, 480);
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].set_size(768, 480);
    dst_position_wide.set_xy((768 - SOLARUS_SCREEN_WIDTH * 2) / 2, 0);
  }
  else if (SDL_VideoModeOK(720, 480, 32, flags)) {
    mode_sizes[FULLSCREEN_WIDE].set_size(720, 480);
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].set_size(720, 480);
    dst_position_wide.set_xy((720 - SOLARUS_SCREEN_WIDTH * 2) / 2, 0);
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

  if (proposed_modes != NULL) {
    bool found = false;
    int i = 0;
    while (proposed_modes[i] != NO_MODE && !found) {
      found = (mode == proposed_modes[i]);
    }
    if (!found) {
      return false;
    }
  }

  const Rectangle* size = &mode_sizes[mode];

  if (size->get_width() == 0) {
    return false;
  }

  int flags = surface_flags;
  if (is_fullscreen(mode)) {
    flags |= SDL_FULLSCREEN;
  }

  return SDL_VideoModeOK(size->get_width(), size->get_height(), 32, flags) != 0;
}

/**
 * @brief Returns whether a video mode is in fullscreen.
 * @param mode A video mode.
 * @return true if this video mode is in fullscreen.
 */
bool VideoManager::is_fullscreen(VideoMode mode) {
  return mode >= FULLSCREEN_NORMAL;
}

/**
 * @brief Returns whether the current video mode is in fullscreen.
 * @return true if the current video mode is in fullscreen.
 */
bool VideoManager::is_fullscreen() {
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

  VideoMode mode;
  if (proposed_modes != NULL) {
    mode = proposed_modes[0];
  }
  else {
    mode = WINDOWED_STRETCHED;
  }

  set_video_mode(mode);
}

/**
 * @brief Sets the video mode.
 *
 * The specified video mode is supposed to be supported.
 *
 * @param mode the video mode
 */
void VideoManager::set_video_mode(VideoMode mode) {

  int flags = surface_flags;
  int show_cursor;
  if (is_fullscreen(mode)) {
    flags |= SDL_FULLSCREEN;
    show_cursor = SDL_DISABLE;
  }
  else {
    show_cursor = SDL_ENABLE;
  }

  const Rectangle& size = mode_sizes[mode];
  if (size.get_width() > SOLARUS_SCREEN_WIDTH * 2) {
    // Wide screen resolution with two black side bars.
    offset = dst_position_wide.get_x();
  }
  else {
    // No side bars.
    offset = 0;
  }
  width = size.get_width();
  end_row_increment = 2 * offset + width;

  if (!disable_window) {
    SDL_Surface* screen_internal_surface = SDL_SetVideoMode(
        size.get_width(), size.get_height(), SOLARUS_COLOR_DEPTH, flags);

    Debug::check_assertion(screen_internal_surface != NULL, StringConcat() << "Cannot create the video surface for mode " << mode);

    SDL_ShowCursor(show_cursor);
    delete this->screen_surface;
    this->screen_surface = new Surface(screen_internal_surface);
  }
  this->video_mode = mode;

  // Write the configuration file.
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
 * @brief Returns a list of all supported video modes.
 * @return The list of supported video modes.
 */
const std::list<VideoManager::VideoMode> VideoManager::get_video_modes() {

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
 * @brief Blits a surface on the screen with the current video mode.
 * @param src_surface the source surface to display on the screen
 */
void VideoManager::display(Surface& src_surface) {

  if (disable_window) {
    return;
  }

  switch (video_mode) {

    case WINDOWED_NORMAL:
      blit(src_surface, *screen_surface);
      break;

    case WINDOWED_STRETCHED:
    case FULLSCREEN_NORMAL:
    case FULLSCREEN_WIDE:
      blit_stretched(src_surface, *screen_surface);
      break;

    case WINDOWED_SCALE2X:
    case FULLSCREEN_SCALE2X:
    case FULLSCREEN_SCALE2X_WIDE:
      blit_scale2x(src_surface, *screen_surface);
      break;

    default:
      Debug::die(StringConcat() << "Unknown video mode " << video_mode);
      break;
  }

  SDL_Flip(screen_surface->get_internal_surface());
}

/**
 * @brief Blits a SOLARUS_SCREEN_WIDTH*SOLARUS_SCREEN_HEIGHT surface on a
 * SOLARUS_SCREEN_WIDTH*SOLARUS_SCREEN_HEIGHT surface.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit(Surface& src_surface, Surface& dst_surface) {
  src_surface.display(dst_surface);
}

/**
 * @brief Blits a SOLARUS_SCREEN_WIDTH*SOLARUS_SCREEN_HEIGHT surface on a
 * double-size surface, stretching the image.
 *
 * Two black side bars are added if the destination surface is wider than SOLARUS_SCREEN_WIDTH * 2.
 *
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_stretched(Surface& src_surface, Surface& dst_surface) {

  SDL_Surface* src_internal_surface = src_surface.get_internal_surface();
  SDL_Surface* dst_internal_surface = dst_surface.get_internal_surface();
  SDL_Surface* surface_to_draw;

#ifndef __APPLE__
  surface_to_draw = dst_internal_surface;
#else
  /* On Mac OS X an intermediate surface is needed.
   * FIXME: creating a new surface at each blit is probably a horrible loss
   * of performance */
  surface_to_draw = SDL_CreateRGBSurface(SDL_SWSURFACE,
      dst_internal_surface->w, dst_internal_surface->h, bits_per_pixel, 0, 0, 0, 0);
#endif

  SDL_LockSurface(src_internal_surface);
  SDL_LockSurface(surface_to_draw);

  uint32_t* src = (uint32_t*) src_internal_surface->pixels;
  uint32_t* dst = (uint32_t*) surface_to_draw->pixels;

  int p = offset;
  for (int i = 0; i < SOLARUS_SCREEN_HEIGHT; i++) {
    for (int j = 0; j < SOLARUS_SCREEN_WIDTH; j++) {
      dst[p] = dst[p + 1] = dst[p + width] = dst[p + width + 1] = *src;
      p += 2;
      src++;
    }

    p += end_row_increment;
  }

  SDL_UnlockSurface(surface_to_draw);
  SDL_UnlockSurface(src_internal_surface);
  
#ifdef __APPLE__
  SDL_BlitSurface(surface_to_draw, NULL, dst_internal_surface, NULL);
  SDL_FreeSurface(surface_to_draw);
#endif
}

/**
 * @brief Blits a SOLARUS_SCREEN_WIDTH*SOLARUS_SCREEN_HEIGHT surface on a
 * double-size surface.
 *
 * The image is scaled with an implementation of the Scale2x algorithm.
 * Two black side bars if the destination surface is wider than
 * SOLARUS_SCREEN_WIDTH * 2.
 *
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_scale2x(Surface& src_surface, Surface& dst_surface) {

  SDL_Surface* src_internal_surface = src_surface.get_internal_surface();
  SDL_Surface* dst_internal_surface = dst_surface.get_internal_surface();
  SDL_Surface* surface_to_draw;

#ifndef __APPLE__
  surface_to_draw = dst_internal_surface;
#else
  /* On Mac OS X an intermediate surface is needed.
   * FIXME: creating a new surface at each blit is probably a horrible loss
   * of performance */
  surface_to_draw = SDL_CreateRGBSurface(SDL_SWSURFACE,
      dst_internal_surface->w, dst_internal_surface->h, bits_per_pixel, 0, 0, 0, 0);
#endif

  SDL_LockSurface(src_internal_surface);
  SDL_LockSurface(surface_to_draw);

  uint32_t* src = (uint32_t*) src_internal_surface->pixels;
  uint32_t* dst = (uint32_t*) surface_to_draw->pixels;

  int b, d, e = 0, f,  h;
  int e1 = offset, e2, e3, e4;
  for (int row = 0; row < SOLARUS_SCREEN_HEIGHT; row++) {
    for (int col = 0; col < SOLARUS_SCREEN_WIDTH; col++) {

      // compute a to i

      b = e - SOLARUS_SCREEN_WIDTH;
      d = e - 1;
      f = e + 1;
      h = e + SOLARUS_SCREEN_WIDTH;

      if (row == 0)   { b = e; }
      if (row == SOLARUS_SCREEN_HEIGHT - 1) { h = e; }
      if (col == 0)   { d = e; }
      if (col == SOLARUS_SCREEN_WIDTH - 1) { f = e; }

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

  SDL_UnlockSurface(surface_to_draw);
  SDL_UnlockSurface(src_internal_surface);
 
#ifdef __APPLE__
  SDL_BlitSurface(surface_to_draw, NULL, dst_internal_surface, NULL);
  SDL_FreeSurface(surface_to_draw);
#endif
}

