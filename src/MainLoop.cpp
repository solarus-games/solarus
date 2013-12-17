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
#include "lowlevel/Video.h"
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

namespace solarus {

/**
 * \brief Initializes the game engine.
 * \param args Command-line arguments.
 */
MainLoop::MainLoop(const CommandLine& args):
  root_surface(NULL),
  lua_context(NULL),
  exiting(false),
  game(NULL),
  next_game(NULL) {

  // Initialize basic features (input, audio, video, files...).
  System::initialize(args);

  // Read the quest general properties.
  QuestProperties quest_properties(*this);
  quest_properties.load();

  // Read the quest resource list from data.
  QuestResourceList::initialize();

  // Create the quest surface.
  root_surface = Surface::create(
      Video::get_quest_size()
  );
  RefCountable::ref(root_surface);

  // Run the Lua world.
  // Do this after the creation of the window because Lua might change the
  // video mode initially. This will avoid blinking.
  lua_context = new LuaContext(*this);
  lua_context->initialize();

  // Finally show the window.
  Video::show_window();
}

/**
 * \brief Cleans everything.
 */
MainLoop::~MainLoop() {

  if (game != NULL) {
    game->stop();
    delete game;
    game = NULL;
  }

  delete lua_context;
  RefCountable::unref(root_surface);
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
 */
void MainLoop::run() {

  // Main loop.
  uint32_t last_frame_date = System::get_real_time();
  uint32_t lag = 0;  // Lose time of the simulation.

  // The main loop basically repeats
  // check_input(), update(), draw() and sleep().
  // Each call to update() makes the simulated time advance one fixed step.
  while (!is_exiting()) {

    // Measure the time of the last iteration without the check_input() phase.
    // Some check_input() calls are much slower than other, for example when
    // they involve loading a map. However, these long check_input() calls do
    // not mean that the system is slow and that we should skip drawings,
    // unlike long updates and long drawings.
    // That is is why to compute the lag, we ignore the time spent in
    // check_input().
    uint32_t current_frame_date = System::get_real_time();
    uint32_t last_frame_duration = current_frame_date - last_frame_date;

    // 1. Detect and handle input events.
    check_input();

    last_frame_date = System::get_real_time();
    lag += last_frame_duration;
    // At this point, lag represents how much late the simulated time with
    // compared to the real time.

    // 2. Update the world once, or several times (skipping some draws)
    // if the system is slow.
    int num_updates = 0;
    while (lag >= System::timestep
        && num_updates < 10  // To draw sometimes anyway on very slow systems.
        && !is_exiting()) {
      update();
      lag -= System::timestep;
      ++num_updates;
    }

    // 3. Redraw the screen.
    if (num_updates > 0) {
      draw();
    }

    // 4. Sleep if we have time, to save CPU and GPU cycles.
    uint32_t frame_duration = System::get_real_time() - last_frame_date;
    if (frame_duration < System::timestep) {
      System::sleep(System::timestep - frame_duration);
    }
  }
}

/**
 * \brief Detects whether there was an input event and if yes, handles it.
 */
void MainLoop::check_input() {

  InputEvent* event = InputEvent::get_event();
  if (event != NULL) {
    notify_input(*event);
    delete event;
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
void MainLoop::notify_input(const InputEvent& event) {

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

  // go to another game?
  if (next_game != game) {

    delete game;
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
}

/**
 * \brief Redraws the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void MainLoop::draw() {

  if (root_surface->is_software_destination()
      || !Video::is_acceleration_enabled()) {
    root_surface->fill_with_color(Color::get_transparent());
  }
  if (game != NULL) {
    game->draw(*root_surface);
  }
  lua_context->main_on_draw(*root_surface);
  Video::render(*root_surface);
}

}

