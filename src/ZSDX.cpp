/**
 * This module initializes the game engine and handles the screen.
 */

#include <fmod/fmod.h>
#include "ZSDX.h"

/**
 * Number of times the screen is redrawn is a second.
 */
const int FRAMES_PER_SECOND = 50;

/**
 * Delay between two frames in millisecond.
 */
const int FRAME_INTERVAL = 1000 / FRAMES_PER_SECOND;

/**
 * The game.
 */
Game *ZSDX::game;

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

  // initialize the game resource
  game_resource = new GameResource();
  game_resource->create_resources();
  
  // create the game
  game = new Game();
}

/**
 * Cleans everything.
 */
void ZSDX::exit(void) {
  SDL_Quit();
  Music::exit();
  delete game_resource;
  delete game;
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

/**
 * Launches the game.
 */
void ZSDX::main(void) {

  // initialize the game engine
  initialize();

  // first map
  game->set_current_map(MAP_LINKHOUSE);
  //  game->set_current_map(MAP_LINKHOUSESECRETROOM);
  //  game->set_current_map(MAP_RUPEEHOUSE);

  game->play();

  // close the game engine
  exit();
}

/**
 * Entry point of the program.
 */
int main(int argc, char **argv) {

  ZSDX::main();
  
  return 0;
}
