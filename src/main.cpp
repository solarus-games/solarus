#include "datatypes.h"
#include "animation_frame_handler.h"
#include "global.h"
#include "game_resource.h"

Global zsdx_global;

void zsdx_init(void) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_ShowCursor(SDL_DISABLE);
  
  //zsdx_global.screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  zsdx_global.screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);

  // set the timer
  SDL_AddTimer(250, animation_next_frame, NULL);

  // init the game resource
  create_game_resource();
}

void zsdx_exit(void) {
  SDL_Quit();

  // free the game resource
  destroy_game_resource();
}
