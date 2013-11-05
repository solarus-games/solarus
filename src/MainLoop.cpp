/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
#include "Settings.h"
#include "QuestProperties.h"
#include "Game.h"
#include "Savegame.h"
#include "StringResource.h"
#include "QuestResourceList.h"

/**
 * \brief Initializes the game engine.
 * \param argc number of arguments of the command line
 * \param argv command-line arguments
 */
MainLoop::MainLoop(int argc, char** argv):
  root_surface(NULL),
  lua_context(NULL),
  exiting(false),
  game(NULL),
  next_game(NULL) {

  // Initialize low-level features (audio, video, files...).
  System::initialize(argc, argv);

  // Read the quest general properties.
  QuestProperties quest_properties(*this);
  quest_properties.load();

  // Read the quest resource list from data.
  QuestResourceList::initialize();
    
  // Create the window now that we know the final engine outset size.
  VideoManager::get_instance()->create_window();
    
  // Load the lua quest stuff now that the window is created.
  root_surface = new Surface(VideoManager::get_instance()->get_quest_size());
  root_surface->increment_refcount();
  lua_context = new LuaContext(*this);
  lua_context->initialize();
}

/**
 * \brief Cleans everything.
 */
MainLoop::~MainLoop() {

  delete lua_context;
  root_surface->decrement_refcount();
  delete root_surface;
  QuestResourceList::quit();
  System::quit();
}

/**
 * \brief Returns the shared Lua context.
 * \return The Lua context where all scripts are run.
 */
LuaContext& MainLoop::get_lua_context() {
  return *lua_context;
}

/**
 * \brief Returns whether the user just closed the window.
 *
 * When this function returns true, you should stop immediately
 * whatever you are doing, free your memory and let the program quit.
 *
 * \return true if the user wants to exit the program
 */
bool MainLoop::is_exiting() {
  return exiting;
}

/**
 * \brief Sets whether the user wants to quit the program.
 */
void MainLoop::set_exiting() {

  // Stop the program.
  exiting = true;
}

/**
 * \brief Returns whether the program is being reset.
 */
bool MainLoop::is_resetting() {

  return game != NULL && next_game == NULL;
}

/**
 * \brief Marks the current game as finished and sets the initial screen
 * to be started at the next cycle.
 */
void MainLoop::set_resetting() {

  // Reset the program.
  set_game(NULL);
}

/**
 * \brief Returns the current game if any.
 * \return The game currently running or NULL.
 */
Game* MainLoop::get_game() {
  return game;
}

/**
 * \brief Changes the game.
 * \param game The new game to start, or NULL to start no game.
 */
void MainLoop::set_game(Game* game) {

  if (this->game != NULL) {
    this->game->stop();
  }

  this->next_game = game;
}

/**
 * \brief The main function.
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
  uint32_t frame_interval = 25; // time interval between two drawings
  int delay;
  bool just_redrawn = false; // to detect when the FPS number needs to be decreased

  while (!is_exiting()) {

    // handle the input events
    event = InputEvent::get_event();
    if (event != NULL) {
      notify_input(*event);
      delete event;
    }

    // update the current screen
    update();

    // go to another game?
    if (next_game != game) {
      if (game != NULL) {
        delete game;
      }

      game = next_game;

      if (game != NULL) {
        game->start();
      }
      else {
        lua_context->exit();
        lua_context->initialize();
        Music::play(Music::none);
      }
    }
    else {

      now = System::now();
      delay = next_frame_date - now;
      // delay is the time remaining before the next drawing

      if (delay <= 0) { // it's time to redraw

        // see if the FPS number is too high
        if (just_redrawn && frame_interval <= 30) {
          frame_interval += 5; // redraw the screen less often
          //std::cout << "\rFPS: " << (1000 / frame_interval) << std::flush;
        }

        next_frame_date = now + frame_interval;
        just_redrawn = true;
        draw();
      }
      else {
        just_redrawn = false;

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

  if (game != NULL) {
    game->stop();
    delete game;
    game = NULL;
  }
}

/**
 * \brief This function is called when there is an input event.
 *
 * It handles the events common to all screens:
 * closing the window, pressing F5 or a debug key.
 * The notify_input() method of the current screen
 * is then called.
 */
void MainLoop::notify_input(InputEvent& event) {

  if (event.is_window_closing()) {
    exiting = true;
  }
  else if (event.is_keyboard_key_pressed()) {
    // A key was pressed.
#if defined(PANDORA)
    // TODO make a clean flag
    if (event.get_keyboard_key() == InputEvent::KEY_ESCAPE) {
      exiting = true;
    }
#endif
  }

  // Send the event to Lua and to the current screen.
  bool handled = lua_context->notify_input(event);
  if (!handled && game != NULL) {
    game->notify_input(event);
  }
}

/**
 * \brief Updates the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void MainLoop::update() {

  if (game != NULL) {
    game->update();
  }
  lua_context->update();
  System::update();
}

/**
 * \brief Redraws the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void MainLoop::draw() {

  if (game != NULL) {
    game->draw(*root_surface);
  }
  lua_context->main_on_draw(*root_surface);
  VideoManager::get_instance()->render(*root_surface);
}

