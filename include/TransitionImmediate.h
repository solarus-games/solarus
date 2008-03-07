/**
 * Immediate transition effect.
 */

#ifndef ZSDX_TRANSITION_IMMEDIATE_H
#define ZSDX_TRANSITION_IMMEDIATE_H

#include "Common.h"
#include "TransitionEffect.h"

class TransitionImmediate: public TransitionEffect {

 public:
  TransitionImmediate(TransitionDirection direction);
  ~TransitionImmediate(void);

  void start(void);
  bool is_over(void);
  void display(SDL_Surface *surface);

};

#endif
