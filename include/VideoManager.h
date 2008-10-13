#ifndef ZSDX_VIDEO_MANAGER_H
#define ZSDX_VIDEO_MANAGER_H

#include "Common.h"

/**
 * Displays the window and handles the resolution mode.
 *
 */
class VideoManager {

 public:

  /**
   * The different possible video modes.
   */
  enum VideoMode {
    WINDOWED_640_480,
    WINDOWED_640_480_SCALE2X,
    WINDOWED_320_240,
    FULLSCREEN,
  };

 private:

  VideoMode video_mode;

  void display_320(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void display_640(SDL_Surface *src_surface, SDL_Surface *dst_surface);
  void display_640_scale2x(SDL_Surface *src_surface, SDL_Surface *dst_surface);

 public:

  VideoManager(void);
  ~VideoManager(void);

  void switch_video_mode(void);
  SDL_Surface * set_video_mode(VideoMode mode);
  VideoMode get_video_mode(void);
  bool is_fullscreen(void);

  void display(SDL_Surface *src_surface, SDL_Surface *dst_surface);

};

#endif
