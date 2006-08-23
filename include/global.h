#ifndef ZSDX_GLOBAL_H
#define ZSDX_GLOBAL_H

#include <SDL/SDL.h>
#include "animation_frame_handler.h"

// TODO: find a better place for this data and remove that file?

/* Number of times the screen is redrawn is a second
 */
extern const int FRAMES_PER_SECOND;

/* Delay between two frames in millisecond
 * FRAME_DELAY = 1000 / FRAMES_PER_SECOND
 */
extern const int FRAME_DELAY;

/* Structure with global data for the program
 */
struct Global {
  SDL_Surface *screen;
  AnimationFrameHandler animation_frame_handler;
};

/* Variable to get the global data from anywhere 
 */
extern Global zsdx_global;

#endif
