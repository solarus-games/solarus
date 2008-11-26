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
    WINDOWED_STRETCHED,         /**< the 320*240 game surface is stretched into a 640*480 window (default) */
    WINDOWED_SCALE2X,           /**< the 320*240 game surface is scaled into a 640*480 window with the Scale2x algorithm */
    WINDOWED_NORMAL,            /**< the 320*240 game surface is directly displayed on a 320*240 window */
    FULLSCREEN_NORMAL,          /**< the 320*240 game surface is directly displayed on a 320*240 screen */
    FULLSCREEN_WIDE,            /**< the 320*240 game surface is stretched into a 640*480 surface
				 * which is displayed on a widescreen resolution if possible (768*480 or 720*480) with two black side bars */
    FULLSCREEN_SCALE2X,         /**< the 320*240 game surface is scaled into a 640*480 screen with the Scale2x algorithm */
    FULLSCREEN_SCALE2X_WIDE,    /**< the 320*240 game surface is scaled into a 640*480 surface with the Scale2x algorithm
				 * and this surface is displayed on a widescreen resolution if possible
				 * (768*480 or 720*480) with two black side bars) */
    FULLSCREEN_CENTERED,        /**< the 320*240 game surface is displayed as a box inside a 640*480 screen */
    FULLSCREEN_CENTERED_WIDE,   /**< the 320*240 game surface is displayed as a box inside a 640*400 screen */
    NB_MODES
  };

 private:

  static const SDL_Rect video_mode_sizes[NB_MODES];
  static SDL_Rect dst_position_wide; /**< position of the game surface on the 720*480 video surface */

  VideoMode video_mode;
  SDL_Surface *screen_surface;

  SDL_Rect dst_position_centered;
  int width;
  int offset;
  int end_row_increment;

//  bool is_mode_supported(VideoMode mode);
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
  bool set_video_mode(VideoMode mode);
  VideoMode get_video_mode(void);

  void display(SDL_Surface *src_surface);
};

#endif
