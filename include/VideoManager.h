#ifndef ZSDX_VIDEO_MANAGER_H
#define ZSDX_VIDEO_MANAGER_H

#include "Common.h"

/**
 * Displays the window and handles the resolution mode.
 */
class VideoManager {

 public:

  /**
   * The different possible video modes.
   */
  enum VideoMode {
    WINDOWED_640_480_STRETCHED,
    WINDOWED_640_480_SCALE2X,
    WINDOWED_320_240,
    FULLSCREEN_320_240,
    FULLSCREEN_720_480_STRETCHED,
    FULLSCREEN_640_480_SCALE2X,
    FULLSCREEN_720_480_SCALE2X,
    FULLSCREEN_640_480_CENTERED,
    FULLSCREEN_720_480_CENTERED,
    NB_MODES
  };

 private:

  static const SDL_Rect video_mode_sizes[NB_MODES];
  static SDL_Rect dst_position_wide;

  VideoMode video_mode;
  SDL_Surface *screen_surface;

  SDL_Rect dst_position_centered;
  int width;
  int offset;
  int end_row_increment;

  bool is_mode_supported(VideoMode mode);
  bool is_fullscreen(VideoMode mode);
  bool is_wide(VideoMode mode);

  void blit(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void blit_stretched(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void blit_scale2x(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void blit_centered(SDL_Surface *src_surface, SDL_Surface *dst_surface);

 public:

  VideoManager(void);
  ~VideoManager(void);

  void switch_video_mode(void);
  void set_default_video_mode(void);
  void set_video_mode(VideoMode mode);
  VideoMode get_video_mode(void);

  void display(SDL_Surface *src_surface);
};

#endif
