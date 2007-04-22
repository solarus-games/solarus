/**
 * This module initializes the game engine and handles the screen.
 */

#include <fmod/fmod.h>
#include "zsdx.h"

/**
 * Number of times the screen is redrawn is a second.
 */
const int FRAMES_PER_SECOND = 50;
/**
 * Delay between two frames in millisecond.
 */
const int FRAME_INTERVAL = 1000 / FRAMES_PER_SECOND;

/**
 * The screen.
 */
SDL_Surface *ZSDX::screen;

/**
 * True if we are in full screen mode.
 */
bool ZSDX::fullscreen;

/**
 * The tile animation manager object.
 */
TileAnimationManager ZSDX::tile_animation_manager;

/**
 * The game resource.
 */
GameResource *ZSDX::game_resource;

/**
 * Initializes the game engine.
 */
void ZSDX::initialize(void) {

  // initialise SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

  //  SDL_ShowCursor(SDL_DISABLE);
  SDL_ShowCursor(SDL_ENABLE);
  set_fullscreen(false);
 
  color_init();

  // set the timer
  SDL_AddTimer(250, TileAnimationManager::increment_frame_counter, NULL);

  // initialize FMOD
  Music::initialize();

  // initialise the game resource
  game_resource = new GameResource();
  game_resource->create_resources();
}

/**
 * Cleans everything.
 */
void ZSDX::exit(void) {
  SDL_Quit();
  Music::exit();
  delete game_resource;
}

/**
 * Sets the full screen mode or the windowed mode.
 * @param fullscreen true for full screen mode, false for the windowed mode
 */
void ZSDX::set_fullscreen(bool fullscreen) {
  if (fullscreen) {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
  }
  else {
    screen = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
  }
  ZSDX::fullscreen = fullscreen;
}

/**
 * Returns whether the game is in full screen.
 * @return true if the game is in full screen mode, false otherwise
 */
bool ZSDX::is_fullscreen(void) {
  return fullscreen;
}

/**
 * Switches between full screen mode and windowed mode.
 */
void ZSDX::switch_fullscreen(void) {
  set_fullscreen(!fullscreen);
}
