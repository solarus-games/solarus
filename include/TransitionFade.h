/**
 * Fade in or fade out transition effect.
 */

#ifndef ZSDX_TRANSITION_FADE_H
#define ZSDX_TRANSITION_FADE_H

#include "Common.h"
#include "TransitionEffect.h"

class TransitionFade: public TransitionEffect {

 private:
  Uint32 alpha_start;
  Uint32 alpha_limit;
  Uint32 alpha_increment;
  Uint32 alpha; // current alpha value of the surface

  Uint32 next_frame_date;
  
 public:
  TransitionFade(TransitionDirection direction);
  ~TransitionFade(void);

  void start(void);
  bool is_over(void);
  void display(SDL_Surface *surface);

};

#endif
