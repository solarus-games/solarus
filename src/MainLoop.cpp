/*
 * Copyright (C) 2006-2015 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Output.h"
#include "solarus/lowlevel/QuestFiles.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/System.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/CurrentQuest.h"
#include "solarus/Game.h"
#include "solarus/QuestProperties.h"
#include "solarus/MainLoop.h"
#include "solarus/Savegame.h"
#include "solarus/Settings.h"
#include <lua.hpp>
#include <iostream>
#include <sstream>
#include <string>

namespace Solarus {

namespace {

/**
 * \brief Checks that the quest is compatible with the current version of
 * Solarus.
 * \param solarus_required_version Version of the quest.
 */
void check_version_compatibility(const std::string& solarus_required_version) {

  if (solarus_required_version.empty()) {
    Debug::die("No Solarus version is specified in your quest.dat file!");
  }

  // TODO check the syntax of the version string

  int dot_index_1 = solarus_required_version.find('.');
  std::istringstream iss(solarus_required_version.substr(0, dot_index_1));
  int required_major_version = 0;
  iss >> required_major_version;

  int dot_index_2 = solarus_required_version.find('.', dot_index_1 + 1);
  std::istringstream iss2(solarus_required_version.substr(dot_index_1 + 1, dot_index_2));
  int required_minor_version = 0;
  iss2 >> required_minor_version;

  // The third digit of the version (patch) is ignored because compatibility
  // is not broken by patches.

  // For now, we assume that any mismatch of major or minor version (first
  // and second digits) breaks compatibility and we just die.
  // This may change in the future, because all minor versions won't
  // necessarily break compatibility.
  if (required_major_version != SOLARUS_MAJOR_VERSION ||
      required_minor_version != SOLARUS_MINOR_VERSION) {
    std::ostringstream oss;
    oss << "This quest is made for Solarus " << required_major_version << "."
        << required_minor_version << ".x but you are running Solarus "
        << SOLARUS_VERSION;
    Debug::die(oss.str());
  }
}

}

/**
 * \brief Initializes the game engine.
 * \param args Command-line arguments.
 */
MainLoop::MainLoop(const Arguments& args):
  lua_context(nullptr),
  root_surface(nullptr),
  game(nullptr),
  next_game(nullptr),
  exiting(false) {

  Output::initialize(args);
  std::cout << "Solarus " << SOLARUS_VERSION << std::endl;

  // Initialize basic features (input, audio, video, files...).
  System::initialize(args);

  // Read the quest general properties.
  load_quest_properties();

  // Read the quest resource list from data.
  CurrentQuest::initialize();

  // Create the quest surface.
  root_surface = Surface::create(
      Video::get_quest_size()
  );
  root_surface->set_software_destination(false);  // Accelerate this surface.

  // Run the Lua world.
  // Do this after the creation of the window, but before showing the window,
  // because Lua might change the video mode initially.
  lua_context = std::unique_ptr<LuaContext>(new LuaContext(*this));
  lua_context->initialize();

  // Finally show the window.
  Video::show_window();
}

/**
 * \brief Cleans everything.
 */
MainLoop::~MainLoop() {

  if (game != nullptr) {
    game->stop();
    game.reset();  // While deleting the game, the Lua world must still exist.
  }

  lua_context->exit();
  CurrentQuest::quit();
  System::quit();
  Output::quit();
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

  return game != nullptr && next_game == nullptr;
}

/**
 * \brief Marks the current game as finished and sets the initial screen
 * to be started at the next cycle.
 */
void MainLoop::set_resetting() {

  // Reset the program.
  if (game != nullptr) {
    game->stop();
  }
  set_game(nullptr);
}

/**
 * \brief Returns the current game if any.
 * \return The game currently running or nullptr.
 */
Game* MainLoop::get_game() {
  return game.get();
}

/**
 * \brief Changes the game.
 * \param game The new game to start, or nullptr to start no game.
 */
void MainLoop::set_game(Game* game) {
  this->next_game = game;
}

/**
 * \brief Runs the main loop until the user requests to stop the program.
 *
 * The main loop controls simulated time and repeatedly updates the world and
 * redraws the screen.
 */
void MainLoop::run() {

  // Main loop.

  uint32_t last_frame_date = System::get_real_time();
  uint32_t lag = 0;  // Lose time of the simulation to catch up.
  uint32_t time_dropped = 0;  // Time that won't be caught up.

  // The main loop basically repeats
  // check_input(), update(), draw() and sleep().
  // Each call to update() makes the simulated time advance one fixed step.
  while (!is_exiting()) {

    // Measure the time of the last iteration.
    uint32_t now = System::get_real_time() - time_dropped;
    uint32_t last_frame_duration = now - last_frame_date;
    last_frame_date = now;
    lag += last_frame_duration;
    // At this point, lag represents how much late the simulated time with
    // compared to the real time.

    if (lag >= 200) {
      // Huge lag: don't try to catch up.
      // Maybe we have just made a one-time heavy operation like loading a
      // big file, or the process was just unsuspended.
      // Let's fake the real time instead.
      time_dropped += lag - System::timestep;
      lag = System::timestep;
      last_frame_date = System::get_real_time() - time_dropped;
    }

    // 1. Detect and handle input events.
    check_input();

    // 2. Update the world once, or several times (skipping some draws)
    // to catch up if the system is slow.
    int num_updates = 0;
    while (lag >= System::timestep
        && num_updates < 10  // To draw sometimes anyway on very slow systems.
        && !is_exiting()) {
      step();
      lag -= System::timestep;
      ++num_updates;
    }

    // 3. Redraw the screen.
    if (num_updates > 0) {
      draw();
    }

    // 4. Sleep if we have time, to save CPU and GPU cycles.
    last_frame_duration = (System::get_real_time() - time_dropped) - last_frame_date;
    if (last_frame_duration < System::timestep) {
      System::sleep(System::timestep - last_frame_duration);
    }
  }
}

/**
 * \brief Advances the simulation of one tick.
 *
 * You can use this function if you want to simulate step by step.
 * Otherwise, use run() to execute the standard main loop.
 */
void MainLoop::step() {
  update();
}

/**
 * \brief Detects whether there were input events and if yes, handles them.
 */
void MainLoop::check_input() {

  std::unique_ptr<InputEvent> event = InputEvent::get_event();
  while (event != nullptr) {
    notify_input(*event);
    event = InputEvent::get_event();
  }
}

/**
 * \brief This function is called when there is an input event.
 *
 * It forwards the event to Lua, and then to the game if Lua did not
 * handle it.
 */
void MainLoop::notify_input(const InputEvent& event) {

  if (event.is_window_closing()) {
    set_exiting();
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
  if (!handled && game != nullptr) {
    game->notify_input(event);
  }
}

/**
 * \brief Updates the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void MainLoop::update() {

  if (game != nullptr) {
    game->update();
  }
  lua_context->update();
  System::update();

  // go to another game?
  if (next_game != game.get()) {

    game = std::unique_ptr<Game>(next_game);

    if (game != nullptr) {
      game->start();
    }
    else {
      lua_context->exit();
      lua_context->initialize();
      Music::stop_playing();
    }
  }
}

/**
 * \brief Redraws the current screen.
 *
 * This function is called repeatedly by the main loop.
 */
void MainLoop::draw() {

  root_surface->clear();

  if (game != nullptr) {
    game->draw(root_surface);
  }
  lua_context->main_on_draw(root_surface);
  Video::render(root_surface);
}

/**
 * \brief Reads the quest properties file quest.dat and applies its settings.
 */
void MainLoop::load_quest_properties() {

  // Read the quest properties file.
  const std::string file_name("quest.dat");
  lua_State* l = luaL_newstate();
  const std::string& buffer = QuestFiles::data_file_read(file_name);
  int load_result = luaL_loadbuffer(l, buffer.data(), buffer.size(), file_name.c_str());

  if (load_result != 0) {
    // Syntax error in quest.dat.
    // Loading quest.dat failed.
    // There may be a syntax error, or this is a quest for Solarus 0.9.
    // There was no version number at that time.

    if (std::string(buffer).find("[info]")) {
      // Quest format of Solarus 0.9.
      Debug::die(std::string("This quest is made for Solarus 0.9 but you are running Solarus ")
          + SOLARUS_VERSION);
    }
    else {
      Debug::die(std::string("Failed to load quest.dat: ") + lua_tostring(l, -1));
    }
  }

  QuestProperties properties;
  properties.import_from_lua(l);
  lua_close(l);

  check_version_compatibility(properties.get_solarus_version());
  QuestFiles::set_quest_write_dir(properties.get_quest_write_dir());
  if (!properties.get_title_bar().empty()) {
    Video::set_window_title(properties.get_title_bar());
  }

  Video::set_quest_size_range(
      properties.get_normal_quest_size(),
      properties.get_min_quest_size(),
      properties.get_max_quest_size()
  );

}

}
