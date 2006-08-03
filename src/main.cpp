#include "datatypes.h"
#include "global.h"
#include "animation_frame_handler.h"

Global zsdx_global;

void zsdx_init(void) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  
  zsdx_global.screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  //zsdx_global.screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

  // set the timer
  SDL_AddTimer(250, animation_next_frame, NULL);
}

void zsdx_exit(void) {
  SDL_Quit();
}
