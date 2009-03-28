#ifndef ZSDX_TRANSITION_H
#define ZSDX_TRANSITION_H

#include "Common.h"

/**
 * A transition effect between two screens (maps or not).
 */
class Transition {

 public:

  /**
   * Style of transition between two screens.
   */
  enum Style {
    IMMEDIATE = 0,  // no transition between the two screens
    FADE      = 1,  // fade in and fade out
    SCROLLING = 2,  // scrolling between two maps
  };

  /**
   * Direction of the transition.
   */
  enum Direction {
    IN,
    OUT,
  };

 protected:

  Direction direction;           /**< direction of the transition (in or out) */
  SDL_Surface *previous_surface; /**< during an in transition, this is the surface that was displayed 
				  * when the out transition was played */

  Transition(Direction direction);

 public:
  virtual ~Transition(void);
  static Transition *create(Style style, Direction direction);

  Direction get_direction(void);
  void set_previous_surface(SDL_Surface *previous_surface);
  virtual bool needs_previous_surface(void);

  virtual void start(void) = 0;
  virtual bool is_started(void) = 0;
  virtual bool is_over(void) = 0;
  virtual void display(SDL_Surface *surface) = 0;
};

#endif
