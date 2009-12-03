/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 *
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "lowlevel/VideoManager.h"
#include "lowlevel/Color.h"
#include "lowlevel/FileTools.h"
#include "SDL_Config/SDL_config_lib.h" // ini parsing
#include "StringResource.h"

VideoManager *VideoManager::instance = NULL;

SDL_Rect VideoManager::default_mode_sizes[] = {
  {0, 0, 640, 480},         // WINDOWED_STRETCHED
  {0, 0, 640, 480},         // WINDOWED_SCALE2X
  {0, 0, 320, 240},         // WINDOWED_NORMAL
  {0, 0, 640, 480},         // FULLSCREEN_NORMAL
  {0, 0, 0, 0},             // FULLSCREEN_WIDE
  {0, 0, 640, 480},         // FULLSCREEN_SCALE2X
  {0, 0, 0, 0},             // FULLSCREEN_SCALE2X_WIDE
  {0, 0, 640, 480},         // FULLSCREEN_CENTERED
  {0, 0, 640, 400},         // FULLSCREEN_CENTERED_WIDE
};

/**
 * Initializes the video system and creates the window.
 */
void VideoManager::initialize(void) {
  instance = new VideoManager();
}

/**
 * Closes the video system.
 */
void VideoManager::quit(void) {
  delete instance;
}

/**
 * Returns the video manager.
 * @return the only video manager
 */
VideoManager * VideoManager::get_instance(void) {
  return instance;
}

/**
 * Constructor.
 */
VideoManager::VideoManager(void) {

  // initialize the window
  const std::string &title_bar = StringResource::get_string("video_manager.title_bar");
  SDL_WM_SetCaption(title_bar.c_str(), NULL);
  putenv((char*) "SDL_VIDEO_CENTERED=center");
  putenv((char*) "SDL_NOMOUSE");

  // detect what widescreen resolution is supported (768*480 or 720*480)
  
  for (int i = 0; i < NB_MODES; i++) {
    mode_sizes[i] = default_mode_sizes[i];
  }
  
  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN;
  if (SDL_VideoModeOK(768, 480, 32, flags)) {
    mode_sizes[FULLSCREEN_WIDE].w = 768;
    mode_sizes[FULLSCREEN_WIDE].h = 480;
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].w = 768;
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].h = 480;
    dst_position_wide.x = 64;
    dst_position_wide.y = 0;
  }
  else if (SDL_VideoModeOK(720, 480, 32, flags)) {
    mode_sizes[FULLSCREEN_WIDE].w = 720;
    mode_sizes[FULLSCREEN_WIDE].h = 480;
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].w = 720;
    mode_sizes[FULLSCREEN_SCALE2X_WIDE].h = 480;
    dst_position_wide.x = 40;
    dst_position_wide.y = 0;
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
 * Destructor.
 */
VideoManager::~VideoManager(void) {

}

/**
 * Returns whether a video mode is supported.
 * @param mode a video mode
 * @return true if this mode is supported
 */
bool VideoManager::is_mode_supported(VideoMode mode) {

  const SDL_Rect *size = &mode_sizes[mode];

  if (size->w == 0) {
    return false;
  }

  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  if (is_fullscreen(mode)) {
    flags |= SDL_FULLSCREEN;
  }

  return SDL_VideoModeOK(size->w, size->h, 32, flags) != 0;
}

/**
 * Returns whether a video mode is in fullscreen.
 * @param mode a video mode
 * @return true if this video mode is in fullscreen
 */
bool VideoManager::is_fullscreen(VideoMode mode) {
  return mode >= FULLSCREEN_NORMAL;
}

/**
 * Sets the next video mode.
 */
void VideoManager::switch_video_mode(void) {

  VideoMode mode = video_mode;
  do {
    mode = (VideoMode) ((mode + 1) % NB_MODES);
  } while (!is_mode_supported(mode));
  set_video_mode(mode);
}

/**
 * Sets the initial video mode.
 * The initial video mode is read from the configuration file if existing.
 * Otherwise, the default video mode is chosen.
 */
void VideoManager::set_initial_video_mode(void) {

  const std::string file_name = "config.ini";
  if (!FileTools::data_file_exists(file_name)) {
    // configuration file not found: pick the default video mode
    set_default_video_mode();
  }
  else {
    // parse the ini file
    CFG_File ini;
    SDL_RWops *rw = FileTools::data_file_open_rw(file_name);

    if (CFG_OpenFile_RW(rw, &ini) != CFG_OK) {
      DIE("Cannot parse configuration file");
    }

    if (CFG_SelectGroup("configuration", 0) != CFG_OK) {
      set_default_video_mode();
    }

    int value = CFG_ReadInt("video_mode", -1);

    FileTools::data_file_close_rw(rw);
    CFG_CloseFile(&ini);

    if (value == -1 || value < 0 || value >= NB_MODES) {
      set_default_video_mode();
    }
    else {
      set_video_mode((VideoMode) value);
    }
  }
}

/**
 * Sets the default video mode.
 */
void VideoManager::set_default_video_mode(void) {
  set_video_mode(WINDOWED_STRETCHED);
}

/**
 * Sets the video mode.
 * The specified video mode must be supported.
 * @param mode the video mode
 */
void VideoManager::set_video_mode(VideoMode mode) {

  const SDL_Rect *size = &mode_sizes[mode];

  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  int show_cursor;
  if (is_fullscreen(mode)) {
    flags |= SDL_FULLSCREEN;
    show_cursor = SDL_DISABLE;
  }
  else {
    show_cursor = SDL_ENABLE;
  }

  if (size->w > 640) {
    dst_position_centered.x = dst_position_wide.x + 160;
    dst_position_centered.y = 0;
    width = size->w;
    offset = dst_position_wide.x;
  }
  else {
    dst_position_centered.x = 160;
    dst_position_centered.y = (mode == FULLSCREEN_CENTERED_WIDE) ? 80 : 120;
    width = 640;
    offset = 0;
  }
  end_row_increment = 2 * offset + width;

  SDL_Surface *screen_surface = SDL_SetVideoMode(size->w, size->h, 32, flags);

  if (screen_surface == NULL) {
    DIE("Cannot create the video surface for mode " << mode);
  }

  SDL_ShowCursor(show_cursor);
  this->video_mode = mode;
  this->screen_surface = screen_surface;

  // write the ini file
  CFG_File ini;
  if (CFG_OpenFile(NULL, &ini) != CFG_OK) {
    DIE("Cannot create ini object for configuration file");
  }

  CFG_SelectGroup("configuration", 1);
  CFG_WriteInt("video_mode", mode);

  SDL_RWops *rw = FileTools::data_file_new_rw(64000);
  if (CFG_SaveFile_RW(rw) != CFG_OK) {
    DIE("Cannot build the new configuration information");
  }
  CFG_CloseFile(&ini);

  FileTools::data_file_save_rw(rw, "config.ini");
  FileTools::data_file_close_rw(rw);
}

/**
 * Returns the current video mode.
 * @return the video mode
 */
VideoManager::VideoMode VideoManager::get_video_mode(void) {
  return video_mode;
}

/**
 * Blits a surface on the screen with the current video mode.
 * @param src_surface the source surface
 */
void VideoManager::display(SDL_Surface *src_surface) {

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
    DIE("Unknown video mode " << video_mode);
    break;
  }

  SDL_Flip(screen_surface);
}

/**
 * Blits a 320*240 surface on a 320*240 surface.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit(SDL_Surface *src_surface, SDL_Surface *dst_surface) {
  SDL_BlitSurface(src_surface, NULL, dst_surface, NULL);
}

/**
 * Blits a 320*240 surface on a 640*480 or 640*400 surface, centering the image.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_centered(SDL_Surface *src_surface, SDL_Surface *dst_surface) {
  SDL_BlitSurface(src_surface, NULL, dst_surface, &dst_position_centered);
}

/**
 * Blits a 320*240 surface on a 640*480, 768*480 or 720*480 surface, stretching the image
 * and adding two black side bars if the destination surface is wider than 640.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_stretched(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  SDL_LockSurface(src_surface);
  SDL_LockSurface(dst_surface);

  uint32_t *src = (uint32_t*) src_surface->pixels;
  uint32_t *dst = (uint32_t*) dst_surface->pixels;

  int p = offset;
  for (int i = 0; i < 240; i++) {

    for (int j = 0; j < 320; j++) {
      dst[p] = dst[p + 1] = dst[p + width] = dst[p + width + 1] = *src;
      p += 2;
      src++;
    }

    p += end_row_increment;
  }

  SDL_UnlockSurface(dst_surface);
  SDL_UnlockSurface(src_surface);
}

/**
 * Blits a 320*240 surface on a 640*480, 768*480 or 720*480 surface, scaling the image with
 * an implementation of the Scale2x algorithm.
 * Two black side bars if the destination surface is wider than 640.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_scale2x(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  SDL_LockSurface(src_surface);
  SDL_LockSurface(dst_surface);

  uint32_t *src = (uint32_t*) src_surface->pixels;
  uint32_t *dst = (uint32_t*) dst_surface->pixels;

  int a, b, c, d, e = 0, f, g, h, i;
  int e1 = offset, e2, e3, e4;
  for (int row = 0; row < 240; row++) {
    for (int col = 0; col < 320; col++) {

      // compute a to i

      a = e - 321;
      b = e - 320;
      c = e - 319;

      d = e - 1;
      f = e + 1;

      g = e + 319;
      h = e + 320;
      i = e + 321;

      if (row == 0)   { a = d; b = e; c = f; }
      if (row == 239) { g = d; h = e; i = f; }
      if (col == 0)   { a = b; d = e; g = h; }
      if (col == 319) { c = b; f = e; i = h; }

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

  SDL_UnlockSurface(dst_surface);
  SDL_UnlockSurface(src_surface);
}

