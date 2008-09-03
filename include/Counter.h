#ifndef ZSDX_COUNTER_H
#define ZSDX_COUNTER_H

#include "Common.h"

/**
 * This class displays a counter on a surface.
 * The counter can only display positive integer numbers.
 */
class Counter {

 private:

  unsigned int nb_digits;
  bool fill_with_zeros;

  unsigned int value;
  SDL_Surface *surface_drawn;
  SDL_Rect destination_position;
  SDL_Surface *img_digits;

  void rebuild_with_value(unsigned int value);
 
 public:
  
  Counter(unsigned int nb_digits, bool fill_with_zeros,
	      int x, int y);
  ~Counter(void);

  unsigned int get_value(void);
  void set_value(unsigned int value);
  void increase(void);
  void decrease(void);

  void display(SDL_Surface *destination);
  void display(SDL_Surface *destination, int x, int y);
};

#endif
