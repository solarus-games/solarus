/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
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
#include "MainLoop.h"
#include "lowlevel/System.h"
#include "lowlevel/VideoManager.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "lowlevel/Music.h"
#include "lowlevel/FileTools.h"
#include "lowlevel/Debug.h"
#include "lua/LuaContext.h"
#include "Game.h"
#include "Savegame.h"
#include "StringResource.h"
#include "DebugKeys.h"

/**
 * @brief Initializes the game engine.
 * @param argc number of arguments of the command line
 * @param argv command-line arguments
 */
MainLoop::MainLoop(int argc, char** argv):
  current_screen(NULL),
  next_screen(NULL),
  root_surface(NULL),
  debug_keys(NULL),
  lua_context(NULL),
  resetting(false),
  exiting(false) {

  // Initialize low-level features (audio, video, files...).
  System::initialize(argc, argv);
  root_surface = new Surface(SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT);
  debug_keys = new DebugKeys(*this);
  lua_context = new LuaContext(*this);
  lua_context->initialize();
}

/**
 * @brief Cleans everything.
 */
MainLoop::~MainLoop() {

  delete current_screen;
  delete next_screen;
  delete lua_context;
  delete root_surface;
  delete debug_keys;
  System::quit();
}

/**
 * @brief Returns the debugging keys object.
 * @return the debbuging keys object
 */
DebugKeys& MainLoop::get_debug_keys() {
  return *debug_keys;
}

/**
 * @brief Returns the shared Lua context.
 * @return The Lua context where all scripts are run.
 */
LuaContext& MainLoop::get_lua_context() {
  return *lua_context;
}

/**
 * @brief Returns whether the user just closed the window.
 *
 * When this function returns true, you should stop immediately
 * whatever you are doing, free your memory and let the program quit.
 *
 * @return true if the user wants to exit the program
 */
bool MainLoop::is_exiting() {
  return exiting;
}

/**
 * @brief Sets whether the user wants to quit the program.
 */
void MainLoop::set_exiting() {

  // Stop the program.
  exiting = true;
}

/**
 * @brief Marks the current screen as finished and sets the initial screen
 * to be started at the next cycle.
 */
void MainLoop::set_resetting() {

  // Reset the program.
  resetting = true;
  get_debug_keys().set_game(NULL);
}

/**
 * @brief Marks the current screen as finished and sets another one to be
 * started at the next cycle.
 * @param next_screen The next screen to show
 */
void MainLoop::set_next_screen(Screen* next_screen) {

  Debug::check_assertion(this->next_screen == NULL,
      "Another new screen is already set to be started");

  this->next_screen = next_screen;
}

/**
 * @brief Marks the current screen as finished and starts a game.
 *
 * This function is equivalent to:
 * set_next_screen(new Game(*this, Savegame(savegame_file))),
 * except that if the savegame file does not exists, nothing happens.
 *
 * @param savegame_file name of the savegame file to load
 */
void MainLoop::start_game(const std::string& savegame_file) {

  if (FileTools::data_file_exists(savegame_file)) {
    Savegame* savegame = new Savegame(savegame_file);
    get_lua_context().set_created(savegame);  // The savegame belongs to Lua.
    Game* game = new Game(*this, savegame);
    set_next_screen(game);
  }
}

/**
 * @brief The main function.
 *
 * The main loop is executed here.
 * The input events are forwarded to the current screen.
 * The current screen is redrawn when necessary.
 */
void MainLoop::run() {

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
      notify_input(*event);
      delete event;
    }

    // update the current screen
    update();

    // go to another screen?
    if (next_screen != NULL || resetting) {
      if (current_screen != NULL) {
        current_screen->stop();
        delete current_screen;
      }
      current_screen = next_screen;
      next_screen = NULL;

      if (current_screen != NULL) {
        current_screen->start();
      }
      else if (resetting) {
        resetting = false;
        lua_context->exit();
        lua_context->initialize();
        Music::play(Music::none);
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

  if (current_screen != NULL) {
    current_screen->stop();
  }
}

/**
 * @brief This function is called when there is an input event.
 *
 * It handles the events common to all screens:
 * closing the window, pressing F5 or a debug key.
 * The notify_input() method of the current screen
 * is then called.
 */
void MainLoop::notify_input(InputEvent& event) {

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
    else if (key == InputEvent::KEY_RETURN
        && (event.is_alt_down() || event.is_control_down())) {
      // Alt + Return or Ctrl + Return: switch fullscreen
      VideoManager::get_instance()->switch_fullscreen();
    }
    else if (key == InputEvent::KEY_F4 && event.is_alt_down()) {
      // Alt + F4: quit the program
      exiting = true;
    }
#if defined(PANDORA)
    else if (key == InputEvent::KEY_ESCAPE) {
      exiting = true;
    }
#endif
    else {
      debug_keys->key_pressed(key);
    }
  }
  else if (event.is_keyboard_key_released()) {
    // a key is released
    debug_keys->key_released(key);
  }

  // send the event to the current screen
  if (current_screen != NULL) {
    current_screen->notify_input(event);
  }
  lua_context->notify_input(event);
}

/**
 * @brief Updates the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void MainLoop::update() {

  debug_keys->update();
  if (current_screen != NULL) {
    current_screen->update();
  }
  lua_context->update();
  System::update();
}

/**
 * @brief Redraws the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void MainLoop::display() {

  root_surface->fill_with_color(Color::get_black());
  if (current_screen != NULL) {
    current_screen->display(*root_surface);
  }
  VideoManager::get_instance()->display(*root_surface);
}

