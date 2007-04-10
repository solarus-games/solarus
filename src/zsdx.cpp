#include <fmod/fmod.h>
#include "zsdx.h"
using namespace std;
#include <iostream>

ZSDX zsdx;

GameResource *game_resource = zsdx.get_game_resource();

ZSDX::ZSDX(void) {
  // initialise SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  //  SDL_ShowCursor(SDL_DISABLE);
  SDL_ShowCursor(SDL_ENABLE);
  set_fullscreen(false);
 
  color_init();

  // set the timer
  SDL_AddTimer(250, tile_animation_next_frame, NULL);

  // initialize FMOD
  Music::initialize();

  // initialise the game resource
  game_resource = new GameResource();
  game_resource->create_resources();
}

ZSDX::~ZSDX(void) {
  SDL_Quit();
  Music::exit();
  delete game_resource;
}

void ZSDX::set_fullscreen(bool fullscreen) {
  if (fullscreen) {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  }
  else {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  }
  this->fullscreen = fullscreen;
}

bool ZSDX::is_fullscreen(void) {
  return fullscreen;
}

void ZSDX::switch_fullscreen(void) {
  set_fullscreen(!fullscreen);
}
