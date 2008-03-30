#ifndef ZSDX_COUNTER_VIEW_H
#define ZSDX_COUNTER_VIEW_H

#include "Common.h"

/**
 * This class displays a counter on a surface.
 * The counter can only display positive integer numbers.
 */
class CounterView {

 private:

  unsigned int nb_digits;
  unsigned int value;
  SDL_Surface *surface_drawn;
  SDL_Rect destination_position;
  SDL_Surface *img_digits;
 
 public:
  
  CounterView(unsigned int max_digits, int x, int y);
  ~CounterView(void);

  unsigned int get_value(void);
  void set_value(unsigned int value);
  void increment(void);
  void decrement(void);

  void display(SDL_Surface *destination);
};

#endif
