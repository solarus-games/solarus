#include "VideoManager.h"
#include "Color.h"

/**
 * Constructor.
 */
VideoManager::VideoManager(void) {

  // make the window centered 
  SDL_WM_SetCaption("Zelda Solarus Deluxe", NULL);
  SDL_putenv((char*) "SDL_VIDEO_CENTERED=center");

  /* compute the ideal fullscreen resolution depending on the ratio:
   * on large screens, some side borders will be added
   */
  const SDL_VideoInfo *video_info = SDL_GetVideoInfo();
  double ratio = (double) video_info->current_h / (double) video_info->current_w;

  int side_border_width = 0;
  int side_border_height = 0;
  if (ratio < 0.75) {
    side_border_width = (int) (240.0 / ratio) - 320;
  }
  /*
  else if (ratio > 0.75) {
    side_border_height = (int) (320.0 * ratio) - 240;
  }
  */

  width_320 = 320 + side_border_width;
  height_240 = 240 + side_border_height;

  width_640 = width_320 * 2;
  height_480 = height_240 * 2;

  // if the ideal video mode is not supported, try more common values
  if (!SDL_VideoModeOK(width_320, height_240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN)) {

    if (ratio < 0.75 && SDL_VideoModeOK(384, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN)) {
      width_320 = 384;
      side_border_width = 32;
    }
    else {
      width_320 = 320;
      side_border_width = 0;
    }
    height_240 = 240;
  }

  if (!SDL_VideoModeOK(width_640, height_480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN)) {
    if (ratio < 0.75 && SDL_VideoModeOK(768, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN)) {
      width_640 = 768;
      side_border_width = 64;
    }
    else {
      width_640 = 640;
      side_border_width = 0;
    }
    height_480 = 480;
  }

  windowed_dst_position.x = 0;
  windowed_dst_position.y = 0;

  fullscreen_stretched_dst_position.x = side_border_width / 2;
  fullscreen_stretched_dst_position.y = side_border_height / 2;

  fullscreen_centered_dst_position.x = side_border_width / 2 + 160;
  fullscreen_centered_dst_position.y = side_border_height / 2 + 120;

}

/**
 * Destructor.
 */
VideoManager::~VideoManager(void) {

}

/**
 * Returns whether the game is in full screen.
 * @return true if the game is in full screen mode, false otherwise
 */
bool VideoManager::is_fullscreen(void) {
  return video_mode >= FULLSCREEN_320_240;
}

/**
 * Sets the next video mode.
 */
void VideoManager::switch_video_mode(void) {
  int mode = (video_mode + 1) % NB_MODES;
  set_video_mode((VideoMode) mode);
}

/**
 * Sets the default video mode.
 */
void VideoManager::set_default_video_mode(void) {
  set_video_mode(WINDOWED_640_480);
  //  set_video_mode(WINDOWED_320_240);
}

/**
 * Sets the video mode.
 * @param mode the video mode
 */
void VideoManager::set_video_mode(VideoMode mode) {

  switch (mode) {

  case FULLSCREEN_320_240:
    dst_position = &fullscreen_stretched_dst_position;
    screen_surface = SDL_SetVideoMode(width_320, height_240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);
    break;

  case FULLSCREEN_640_480_SCALE2X:
    dst_position = &fullscreen_stretched_dst_position;
    screen_surface = SDL_SetVideoMode(width_640, height_480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);
    break;

  case FULLSCREEN_640_480_BORDER:
    dst_position = &fullscreen_centered_dst_position;
    screen_surface = SDL_SetVideoMode(width_640, height_480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);
    break;

  case WINDOWED_320_240:
    dst_position = &windowed_dst_position;
    screen_surface = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_ShowCursor(SDL_ENABLE);
    break;

  case WINDOWED_640_480:
  case WINDOWED_640_480_SCALE2X:
    dst_position = &windowed_dst_position;
    screen_surface = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_ShowCursor(SDL_ENABLE);
    break;

  default:
    break;
  }

  this->video_mode = mode;
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

  SDL_FillRect(src_surface, NULL, Color::white);

  SDL_FillRect(screen_surface, NULL, Color::black);

  switch (video_mode) {

  case WINDOWED_320_240:
  case FULLSCREEN_320_240:
    display_320(src_surface, screen_surface);
    break;

  case WINDOWED_640_480:
    display_640(src_surface, screen_surface);
    break;

  case WINDOWED_640_480_SCALE2X:
  case FULLSCREEN_640_480_SCALE2X:
    display_640_scale2x(src_surface, screen_surface);
    break;

  case FULLSCREEN_640_480_BORDER:
    display_640_border(src_surface, screen_surface);
    break;

  default:
    break;
  }

  SDL_Flip(screen_surface);
}

/**
 * Blits a 320*240 surface on a 320*240 surface.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::display_320(SDL_Surface *src_surface, SDL_Surface *dst_surface) {
  SDL_BlitSurface(src_surface, NULL, dst_surface, dst_position);
}

/**
 * Blits a 320*240 surface on a 640*480 surface, and the image is centered.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::display_640_border(SDL_Surface *src_surface, SDL_Surface *dst_surface) {
  SDL_BlitSurface(src_surface, NULL, dst_surface, dst_position);
}

/**
 * Blits a 320*240 surface on a 640*480 surface, stretching the image.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::display_640(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  Uint32 *src = (Uint32*) src_surface->pixels;
  Uint32 *dst = (Uint32*) dst_surface->pixels;

  int k = 0;
  for (int i = 0; i < 240; i++) {
    for (int j = 0; j < 320; j++) {
      int p = 640 * 2 * i + 2 * j + dst_position->x;
      dst[p] = src[k];
      dst[p + 1] = src[k];
      dst[p + 640] = src[k];
      dst[p + 641] = src[k];

      k++;
    }
  }
}

/**
 * Blits a 320*240 surface on a 640*480 surface, scaling the image with
 * an implementation of the Scale2x algorithm.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::display_640_scale2x(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  Uint32 *src = (Uint32*) src_surface->pixels;
  Uint32 *dst = (Uint32*) dst_surface->pixels;

  int a, b, c, d, e = 0, f, g, h, i;
  int e1, e2, e3, e4;
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
      e1 = 640 * 2 * row + 2 * col + dst_position->x;
      e2 = e1 + 1;
      e3 = e1 + 640;
      e4 = e1 + 641;

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

      e++;
    }
  }
}
