#include "VideoManager.h"
#include "Color.h"

/**
 * Constructor.
 */
VideoManager::VideoManager(void) {

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
  return video_mode == FULLSCREEN;
}

/**
 * Sets the next video mode.
 */
void VideoManager::switch_video_mode(void) {
  int mode = (video_mode + 1) % 4;
  set_video_mode((VideoMode) mode);
}

/**
 * Sets the current video mode.
 * @param mode the video mode
 * @param the screen surface created, as returned by SDL_SetVideoMode; you don't have to free it
 */
SDL_Surface * VideoManager::set_video_mode(VideoMode mode) {

  SDL_Surface *screen;

  switch (mode) {

  case FULLSCREEN:
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);
    break;

  case WINDOWED_320_240:
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_ShowCursor(SDL_ENABLE);
    break;

  case WINDOWED_640_480:
  case WINDOWED_640_480_SCALE2X:
    screen = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_ShowCursor(SDL_ENABLE);
    break;
  }

  this->video_mode = mode;

  return screen;
}

/**
 * Returns the current video mode.
 * @return the video mode
 */
VideoManager::VideoMode VideoManager::get_video_mode(void) {
  return video_mode;
}

/**
 * Blits a surface on another surface with the current video mode.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::display(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  SDL_FillRect(dst_surface, NULL, Color::black);

  switch (video_mode) {

  case FULLSCREEN:
  case WINDOWED_320_240:
    display_320(src_surface, dst_surface);
    break;

  case WINDOWED_640_480:
    display_640(src_surface, dst_surface);
    break;

  case WINDOWED_640_480_SCALE2X:
    display_640_scale2x(src_surface, dst_surface);
    break;
  }
}

/**
 * Redraws the current screen with a 320*240 resolution.
 */
void VideoManager::display_320(SDL_Surface *src_surface, SDL_Surface *dst_surface) {
  SDL_BlitSurface(src_surface, NULL, dst_surface, NULL);
}

/**
 * Redraws the current screen with a 640*480 resolution and without smoothing.
 */
void VideoManager::display_640(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  Uint32 *src = (Uint32*) src_surface->pixels;
  Uint32 *dst = (Uint32*) dst_surface->pixels;

  int k = 0;
  for (int i = 0; i < 240; i++) {
    for (int j = 0; j < 320; j++) {
      int p = 640 * 2 * i + 2 * j;
      dst[p] = src[k];
      dst[p + 1] = src[k];
      dst[p + 640] = src[k];
      dst[p + 641] = src[k];

      k++;
    }
  }
}

/**
 * Redraws the current screen with a 640*480 resolution and with
 * an implementation of the Scale2x algorithm.
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
      if (row == 239) { a = g; b = h; c = i; }
      if (col == 0)   { a = b; d = e; g = h; }
      if (col == 319) { c = b; f = e; i = h; }

      // compute e1 to e4
      e1 = 640 * 2 * row + 2 * col;
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
