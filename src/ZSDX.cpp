/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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

  root_surface = SDL_CreateRGBSurface(SDL_HWSURFACE, 320, 240, 32, 0, 0, 0, 0);
  SDL_ShowCursor(SDL_ENABLE);
  video_manager = new VideoManager();
  video_manager->set_default_video_mode();
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
  SDL_FreeSurface(root_surface);
  delete current_screen;
  delete video_manager;
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
  SDL_Event event = {};
  Uint32 now;
  Uint32 next_frame_date = SDL_GetTicks();
  Uint32 frame_interval = 25; // time interval between to displays
  int delay;
  bool just_displayed = false; // to detect when the FPS number needs to be decreased

  while (!is_exiting()) {

    //    std::cout << "FPS: " << (1000 / frame_interval) << std::endl;

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
      delay = next_frame_date - now;
      // delay is the time remaining before the next display

      if (delay <= 0) { // it's time to display

	// see whether the FPS number is too high
	if (just_displayed && frame_interval <= 30) {
	  frame_interval += 5; // display the screen less often
	  //	  std::cout << "\rFPS: " << (1000 / frame_interval) << std::flush;
	}

	next_frame_date = now + frame_interval;
	just_displayed = true;
	display();
      }
      else {
	just_displayed = false;

	// if we have time, let's sleep to avoid using all the processor
	SDL_Delay(1);

	if (delay >= 15) {
	  // if we have much time, increase the FPS number
	  frame_interval--;
	  //	  std::cout << "\rFPS: " << (1000 / frame_interval) << std::flush;
	}
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

      // F5: change the video mode
    case SDLK_F5:
      video_manager->switch_video_mode();
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
void ZSDX::display(void) {

  SDL_FillRect(root_surface, NULL, Color::black);
  current_screen->display(root_surface);
  video_manager->display(root_surface);
}

/**
 * Returns the video system.
 * @return the video system
 */
VideoManager * ZSDX::get_video_manager(void) {
  return video_manager;
}

/**
 * Entry point of the program.
 */
int main(int argc, char **argv) {

  try {
    zsdx = new ZSDX();
    zsdx->main();
  }
  catch (const std::string &s) {
    std::cerr << "Fatal error: " << s << std::endl;
  }

  delete zsdx;

  return 0;
}
