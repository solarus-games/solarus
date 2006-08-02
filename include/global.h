#ifndef ZSDX_GLOBAL_H
#define ZSDX_GLOBAL_H

#include <SDL/SDL.h>
#include "animation_frame_handler.h"

struct Global {
  SDL_Surface *screen;
  AnimationFrameHandler animation_frame_handler;
};

extern Global zsdx_global;

#endif
