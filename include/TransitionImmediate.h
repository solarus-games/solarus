#ifndef ZSDX_TRANSITION_IMMEDIATE_H
#define ZSDX_TRANSITION_IMMEDIATE_H

#include "Common.h"
#include "Transition.h"

/**
 * Immediate transition effect.
 */
class TransitionImmediate: public Transition {

 public:
  TransitionImmediate(Direction direction);
  ~TransitionImmediate(void);

  void start(void);
  bool is_started(void);
  bool is_over(void);
  void display(SDL_Surface *surface);

};

#endif
