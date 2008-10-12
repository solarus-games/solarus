#include "ZSDX.h"
#include "FileTools.h"
#include "Color.h"
#include "Music.h"
#include "Sound.h"
#include "Game.h"
#include "ResourceManager.h"
#include "Savegame.h"
#include "TextSurface.h"
#include "Random.h"
#include "menus/TitleScreen.h"
#include "menus/SelectionMenuSelectFile.h"

/**
 * Number of times the screen is redrawn is a second.
 */
const int FRAMES_PER_SECOND = 50;

/**
 * Delay between two frames in millisecond.
 */
const int FRAME_INTERVAL = 1000 / FRAMES_PER_SECOND;

/**
 * Global variable to get the ZSDX instance
 * from anywhere in the code.
 */
ZSDX *zsdx = NULL;

/**
 * Initializes the game engine.
 */
ZSDX::ZSDX(void) {

  // initialize SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);
  SDL_WM_SetCaption("Zelda Solarus Deluxe", NULL);

  SDL_ShowCursor(SDL_ENABLE);
  set_fullscreen(false);
  SDL_EnableUNICODE(SDL_ENABLE);
  SDL_EnableKeyRepeat(0, 0);

  // initialize the color system
  Color::initialize();

  // initiliaze the resource manager
  ResourceManager::initialize();

  // initiliaze the random number generator
  Random::initialize();

  // initialize the text displaying
  TextSurface::initialize();

  // initialize the audio system
  Music::initialize();

  // create the first screen
  current_screen = new TitleScreen();
  exiting = false;
  game = NULL;
}

/**
 * Cleans everything.
 */
ZSDX::~ZSDX(void) {
  delete current_screen;
  ResourceManager::quit();
  TextSurface::quit();
  SDL_Quit();
  Sound::quit();
}

/**
 * Sets the current game.
 * @param game the game
 */
void ZSDX::set_game(Game *game) {
  this->game = game;
}

/**
 * Sets the full screen mode or the windowed mode.
 * @param fullscreen true for full screen mode, false for the windowed mode
 */
void ZSDX::set_fullscreen(bool fullscreen) {
  if (fullscreen) {
    root_surface = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_FULLSCREEN);
    SDL_ShowCursor(SDL_DISABLE);
  }
  else {
    root_surface = SDL_SetVideoMode(320, 240, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    SDL_ShowCursor(SDL_ENABLE);
  }
  this->fullscreen = fullscreen;
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
 * Returns whether the user just closed the window.
 * When this function returns true, you should stop immediately
 * whatever you are doing, free your memory and let the program quit.
 * @return true if the user wants to exit the program
 */
bool ZSDX::is_exiting(void) {
  return exiting;
}

/**
 * Sets whether the user wants to quit the program.
 */
void ZSDX::set_exiting(void) {
  exiting = true;
}

/**
 * The main function. The SDL loop is executed here.
 * The SDL events are forwarded to the current screen.
 * The current screen is redrawn when necessary.
 */
void ZSDX::main(void) {

  // SDL main loop
  SDL_Event event;
  Uint32 now, last_frame_date = 0;
  int delay;

  while (!is_exiting()) {

    // handle the SDL events
    if (SDL_PollEvent(&event)) {
      handle_event(event);
    }

    // update the current screen
    current_screen->update();

    // go to another screen?
    if (current_screen->is_screen_finished()) {
      Screen *next_screen = current_screen->get_next_screen();
      delete current_screen;

      if (next_screen != NULL) {
	current_screen = next_screen;
      }
      else {
	current_screen = new TitleScreen();
      }
    }
    else {

      // display everything each time frame
      now = SDL_GetTicks();
      delay = (last_frame_date + FRAME_INTERVAL) - now;
      if (delay <= 0) { // delay is the time before the next display
	last_frame_date = now;
	display_current_screen();
      }
      else if (delay >= 10) { // if we have time, let's sleep
	SDL_Delay(10);
      }
    }
  }
}

/**
 * This function handles an SDL event.
 * It handles the events common to all screens:
 * closing the window and pressing escape or F5.
 * The handle_event() method of the current screen
 * is then called.
 */
void ZSDX::handle_event(const SDL_Event &event) {

  // handle the common events
  switch (event.type) {
    
    // quit if the user closes the window
  case SDL_QUIT:
    exiting = true;
    break;
	
    // a key is pressed
  case SDL_KEYDOWN:
    switch (event.key.keysym.sym) {

      // TODO temporary (escape: quit)
    case SDLK_ESCAPE:
      exiting = true;
      break;

      // F5: full screen / windowed mode
    case SDLK_F5:
      switch_fullscreen();
      break;

    default:
      break;
    }
    break;
  }

  // handle the specific events depending on the current screen
  current_screen->handle_event(event);
}

/**
 * Redraws the current screen.
 */
void ZSDX::display_current_screen(void) {

  SDL_FillRect(root_surface, NULL, Color::black);
  current_screen->display(root_surface);
  SDL_Flip(root_surface);
}

/**
 * Entry point of the program.
 */
int main(int argc, char **argv) {

  try {
    zsdx = new ZSDX();
    zsdx->main();
  }
  catch (const string &s) {
    cerr << "A fatal error occured: " << s << endl;
  }

  delete zsdx;

  return 0;
}
