#include <SDL/SDL.h>
#include "animation_frame_handler.h"
#include "global.h"

// // TODO: remove that
// #include <iostream>

Uint32 animation_next_frame(Uint32 interval, void *param) {
//   std::cout << "timer" << std::endl;
  zsdx_global.animation_frame_handler.increment_frame_counter();
  return interval; // should be 250
}
