/**
 * This module handles the transition effects between two screens.
 */

#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

class TransitionEffect {

 protected:

  TransitionDirection direction;

  TransitionEffect(TransitionDirection direction);

 public:
  virtual ~TransitionEffect(void);
  static TransitionEffect *create_transition(TransitionType type, TransitionDirection direction);

  TransitionDirection get_direction(void);

  virtual void start(void) = 0;
  virtual bool is_started(void) = 0;
  virtual bool is_over(void) = 0;
  virtual void display(SDL_Surface *surface) = 0;

};

#endif
