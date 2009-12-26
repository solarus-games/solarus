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
#include "lowlevel/System.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/Color.h"
#include "lowlevel/TextSurface.h"
#include "lowlevel/Sound.h"
#include "lowlevel/Random.h"
#include "StringResource.h"
#include "ResourceManager.h"
#include "Sprite.h"
#include <SDL/SDL.h>


/**
 * Initializes the whole lowlevel system: the graphics, the audio system,
 * the data file system, etc.
 * @param argc number of command line arguments
 * @param argv command line arguments
 */
void System::initialize(int argc, char **argv) {

  // initialize SDL
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK);

  // files
  FileTools::initialize(argc, argv);
  StringResource::initialize();
  ResourceManager::initialize();

  // video
  VideoManager::initialize();
  Color::initialize();
  TextSurface::initialize();
  Sprite::initialize();

  // audio
  Sound::initialize();

  // input
  SDL_EnableUNICODE(SDL_ENABLE); // TODO move in an Input class
  SDL_EnableKeyRepeat(0, 0);

  // random number generator
  Random::initialize();
}

/**
 * Closes the lowlevel system.
 * This closes all initializations made in initialize().
 */
void System::quit(void) {

  ResourceManager::quit();
  Random::quit();
  // TODO Input::quit();
  Sound::quit();
  Sprite::quit();
  TextSurface::quit();
  Color::quit();
  VideoManager::quit();
  StringResource::quit();
  FileTools::quit();

  SDL_Quit();
}

/**
 * This function is called repeatedly by the main loop.
 * It calls the update function of the low level systems that needs it.
 */
void System::update(void) {
  Sound::update();
}

/**
 * Returns the number of milliseconds elapsed since the beginning of the program.
 */
uint32_t System::now(void) {
  return SDL_GetTicks();
}

/**
 * Makes the program sleep during some time.
 * Due to OS scheduling, the real delay may be longer.
 * @param duration duration of the sleep in milliseconds
 */
void System::sleep(uint32_t duration) {
  SDL_Delay(duration);
}

