/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "Solarus.h"
#include "lowlevel/System.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "lowlevel/FileTools.h"
#include "menus/LanguageScreen.h"
#include "Game.h"
#include "Savegame.h"
#include "StringResource.h"
#include "DebugKeys.h"

/**
 * @brief Initializes the game engine.
 *
 * The following optional command-line arguments are supported:
 *   -datapath=/some/path/to/datadir            loads the data files from the specified directory
 *   -no-audio                                  disables sounds and musics
 *   -no-video                                  disables displaying (used for unitary tests)
 *
 * @param argc number of arguments of the command line
 * @param argv command-line arguments
 */
Solarus::Solarus(int argc, char** argv) {

  // initialize the lowlevel features (audio, video, files...)
  System::initialize(argc, argv);
  root_surface = new Surface(320, 240);
  debug_keys = new DebugKeys(*this);

  // create the first screen
  current_screen = new LanguageScreen(*this);
  exiting = false;
}

/**
 * @brief Cleans everything.
 */
Solarus::~Solarus() {

  delete root_surface;
  delete current_screen;
  delete debug_keys;
  System::quit();
}

/**
 * @brief Returns the debugging keys object.
 * @return the debbuging keys object
 */
DebugKeys& Solarus::get_debug_keys() {

  return *debug_keys;
}

/**
 * @brief Skips the menus and starts the game immediately.
 *
 * This function can be called during the title screen or the selection menu to skip them
 * and start the game immediately with the first savegame (for debugging purposes only!).
 */
void Solarus::skip_menus() {

  if (FileTools::data_file_exists("save1.dat")) {

    Savegame savegame("save1.dat");
    Game* game = new Game(*this, savegame);
    delete current_screen;
    current_screen = game;
  }
}

/**
 * @brief Returns whether the user just closed the window.
 *
 * When this function returns true, you should stop immediately
 * whatever you are doing, free your memory and let the program quit.
 *
 * @return true if the user wants to exit the program
 */
bool Solarus::is_exiting() {
  return exiting;
}

/**
 * @brief Sets whether the user wants to quit the program.
 */
void Solarus::set_exiting() {
  exiting = true;
}

/**
 * @brief The main function.
 *
 * The main loop is executed here.
 * The input events are forwarded to the current screen.
 * The current screen is redrawn when necessary.
 */
void Solarus::main_loop() {

  // main loop
  InputEvent *event;
  uint32_t now;
  uint32_t next_frame_date = System::now();
  uint32_t frame_interval = 25; // time interval between to displays
  int delay;
  bool just_displayed = false; // to detect when the FPS number needs to be decreased

  while (!is_exiting()) {

    // handle the input events
    event = InputEvent::get_event();
    if (event != NULL) {
      notify_event(*event);
      delete event;
    }

    // update the current screen
    update();

    // go to another screen?
    if (current_screen->is_screen_finished()) {
      Screen *next_screen = current_screen->get_next_screen();
      delete current_screen;

      if (next_screen != NULL) {
        current_screen = next_screen;
      }
      else {
        current_screen = new LanguageScreen(*this);
      }
    }
    else {

      now = System::now();
      delay = next_frame_date - now;
      // delay is the time remaining before the next display

      if (delay <= 0) { // it's time to display

	// see if the FPS number is too high
	if (just_displayed && frame_interval <= 30) {
	  frame_interval += 5; // display the screen less often
	  //std::cout << "\rFPS: " << (1000 / frame_interval) << std::flush;
	}

	next_frame_date = now + frame_interval;
	just_displayed = true;
	display();
      }
      else {
        just_displayed = false;

        // if we have time, let's sleep to avoid using all the processor
        System::sleep(1);

        if (delay >= 15) {
          // if we have much time, increase the FPS number
          frame_interval--;
          //std::cout << "\rFPS: " << (1000 / frame_interval) << std::flush;
        }
      }
    }
  }
}

/**
 * @brief This function is called when there is an input event.
 *
 * It handles the events common to all screens:
 * closing the window, pressing F5 or a debug key.
 * The notify_event() method of the current screen
 * is then called.
 */
void Solarus::notify_event(InputEvent& event) {

  // handle the common events
  InputEvent::KeyboardKey key = event.get_keyboard_key();

  if (event.is_window_closing()) {
    exiting = true;
  }
  else if (event.is_keyboard_key_pressed()) {
    // a key is pressed
    if (key == InputEvent::KEY_F5) {
      // F5: change the video mode
      VideoManager::get_instance()->switch_video_mode();
    }
    else if (key == InputEvent::KEY_F4 && event.is_alt_down()) {
      // Alt + F4: quit the program
      exiting = true;
    }
    else {
      debug_keys->key_pressed(key);
    }
  }
  else if (event.is_keyboard_key_released()) {
    // a key is released
    debug_keys->key_released(key);
  }

  // send the event to the current screen
  current_screen->notify_event(event);
}

/**
 * @brief Updates the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void Solarus::update() {

  debug_keys->update();
  current_screen->update();
  System::update();
}

/**
 * @brief Redraws the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void Solarus::display() {

  root_surface->fill_with_color(Color::get_black());
  current_screen->display(root_surface);
  VideoManager::get_instance()->display(root_surface);
}

