#include "VideoManager.h"
#include "Color.h"

const SDL_Rect VideoManager::video_mode_sizes[] = {
  {0, 0, 640, 480},         // WINDOWED_640_480_STRETCHED,
  {0, 0, 640, 480},         // WINDOWED_640_480_SCALE2X,
  {0, 0, 320, 240},         // WINDOWED_320_240,
  {0, 0, 320, 240},         // FULLSCREEN_320_240,
  {0, 0, 720, 480},         // FULLSCREEN_720_480_STRETCHED,
  {0, 0, 640, 480},         // FULLSCREEN_640_480_SCALE2X,
  {0, 0, 720, 480},         // FULLSCREEN_720_480_SCALE2X,
  {0, 0, 640, 480},         // FULLSCREEN_640_480_CENTERED,
  {0, 0, 720, 480},         // FULLSCREEN_720_480_CENTERED,
};

SDL_Rect VideoManager::dst_position_wide = {(720 - 640) / 2, 0};

/**
 * Constructor.
 */
VideoManager::VideoManager(void) {

  // make the window centered 
  SDL_WM_SetCaption("Zelda Solarus Deluxe", NULL);
  SDL_putenv((char*) "SDL_VIDEO_CENTERED=center");

  dst_position_centered.x = 160;
  dst_position_centered.y = 120;

  /*
  // get the video modes supported
  SDL_Rect **video_modes = SDL_ListModes(NULL, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);

  if (video_modes == (SDL_Rect**) -1) {
    cout << "All fullscreen video modes are supported" << endl;
  }
  else {
    for (int i = 0; video_modes[i] != NULL; i++) {
      cout << video_modes[i]->w << " x " << video_modes[i]->h << endl;
    }
  }
  */
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

  const SDL_Rect *size = &video_mode_sizes[mode];
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
  return mode >= FULLSCREEN_320_240;
}

/**
 * Returns whether a video mode is a wide screen mode.
 * @param mode a video mode
 * @return true if this video mode is a wide screen mode
 */
bool VideoManager::is_wide(VideoMode mode) {
  return mode == FULLSCREEN_720_480_STRETCHED
    || mode == FULLSCREEN_720_480_SCALE2X
    || mode == FULLSCREEN_720_480_CENTERED;
}

/**
 * Sets the next video mode.
 */
void VideoManager::switch_video_mode(void) {

  VideoMode mode;
  do {
    mode = (VideoMode) ((video_mode + 1) % NB_MODES);
  } while (!is_mode_supported(mode));

  set_video_mode(mode);
}

/**
 * Sets the default video mode.
 */
void VideoManager::set_default_video_mode(void) {
  set_video_mode(WINDOWED_320_240);
  //  set_video_mode(WINDOWED_640_480_STRETCHED);
}

/**
 * Sets the video mode.
 * The specified video mode must be supported.
 * @param mode the video mode
 */
void VideoManager::set_video_mode(VideoMode mode) {

  const SDL_Rect *size = &video_mode_sizes[mode];

  int flags = SDL_HWSURFACE | SDL_DOUBLEBUF;
  int show_cursor;
  if (is_fullscreen(mode)) {
    flags |= SDL_FULLSCREEN;
    show_cursor = SDL_DISABLE;
  }
  else {
    show_cursor = SDL_ENABLE;
  }

  if (is_wide(mode)) {
    dst_position_centered.x = dst_position_wide.x + 160;
    width = 720;
    offset = dst_position_wide.x;
  }
  else {
    dst_position_centered.x = 160;
    width = 640;
    offset = 0;
  }
  end_row_increment = 2 * offset + width;

  screen_surface = SDL_SetVideoMode(size->w, size->h, 32, flags);
  SDL_ShowCursor(show_cursor);

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

  SDL_FillRect(screen_surface, NULL, Color::black);

  switch (video_mode) {

  case WINDOWED_320_240:
  case FULLSCREEN_320_240:
    blit(src_surface, screen_surface);
    break;

  case WINDOWED_640_480_STRETCHED:
  case FULLSCREEN_720_480_STRETCHED:
    blit_stretched(src_surface, screen_surface);
    break;

  case WINDOWED_640_480_SCALE2X:
  case FULLSCREEN_640_480_SCALE2X:
  case FULLSCREEN_720_480_SCALE2X:
    blit_scale2x(src_surface, screen_surface);
    break;

  case FULLSCREEN_640_480_CENTERED:
  case FULLSCREEN_720_480_CENTERED:
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
 * Blits a 320*240 surface on a 640*480 surface, centering the image.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_centered(SDL_Surface *src_surface, SDL_Surface *dst_surface) {
  SDL_BlitSurface(src_surface, NULL, dst_surface, &dst_position_centered);
}

/**
 * Blits a 320*240 surface on a 640*480 surface, stretching the image.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_stretched(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  SDL_LockSurface(src_surface);
  SDL_LockSurface(dst_surface);

  Uint32 *src = (Uint32*) src_surface->pixels;
  Uint32 *dst = (Uint32*) dst_surface->pixels;

  int p = offset;
  for (int i = 0; i < 480; i += 2) {

    for (int j = 0; j < 640; j += 2) {
      dst[p] = dst[p + 1] = dst[p + width] = dst[p + width + 1] = *src;
      p += 2;
      src++;
    }

    p += end_row_increment;
  }

  SDL_UnlockSurface(src_surface);
  SDL_UnlockSurface(dst_surface);
}

/**
 * Blits a 320*240 surface on a 640*480 surface, scaling the image with
 * an implementation of the Scale2x algorithm.
 * @param src_surface the source surface
 * @param dst_surface the destination surface
 */
void VideoManager::blit_scale2x(SDL_Surface *src_surface, SDL_Surface *dst_surface) {

  SDL_LockSurface(src_surface);
  SDL_LockSurface(dst_surface);

  Uint32 *src = (Uint32*) src_surface->pixels;
  Uint32 *dst = (Uint32*) dst_surface->pixels;

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

  SDL_UnlockSurface(src_surface);
  SDL_UnlockSurface(dst_surface);
}
