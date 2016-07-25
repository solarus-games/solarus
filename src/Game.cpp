/*
 * Copyright (C) 2006-2016 Christopho, Solarus - http://www.solarus-games.org
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
#include "solarus/entities/Destination.h"
#include "solarus/entities/Entities.h"
#include "solarus/entities/EntityState.h"
#include "solarus/entities/Hero.h"
#include "solarus/entities/NonAnimatedRegions.h"
#include "solarus/entities/StartingLocationMode.h"
#include "solarus/entities/TilePattern.h"
#include "solarus/entities/Tileset.h"
#include "solarus/lowlevel/Color.h"
#include "solarus/lowlevel/Debug.h"
#include "solarus/lowlevel/Music.h"
#include "solarus/lowlevel/Surface.h"
#include "solarus/lowlevel/Video.h"
#include "solarus/lua/LuaContext.h"
#include "solarus/CommandsEffects.h"
#include "solarus/CurrentQuest.h"
#include "solarus/Equipment.h"
#include "solarus/Game.h"
#include "solarus/MainLoop.h"
#include "solarus/Map.h"
#include "solarus/Savegame.h"
#include "solarus/Treasure.h"
#include "solarus/TransitionFade.h"
#include <map>

namespace Solarus {

/**
 * \brief Creates a game.
 * \param main_loop The Solarus root object.
 * \param savegame The saved data of this game.
 */
Game::Game(MainLoop& main_loop, const std::shared_ptr<Savegame>& savegame):
  main_loop(main_loop),
  savegame(savegame),
  pause_allowed(true),
  paused(false),
  dialog_box(*this),
  showing_game_over(false),
  suspended_by_script(false),
  started(false),
  restarting(false),
  commands_effects(),
  current_map(nullptr),
  next_map(nullptr),
  previous_map_surface(nullptr),
  transition_style(Transition::Style::IMMEDIATE),
  transition(nullptr),
  crystal_state(false) {

  // notify objects
  savegame->set_game(this);

  // initialize members
  commands = std::unique_ptr<GameCommands>(new GameCommands(*this));
  hero = std::make_shared<Hero>(get_equipment());
  update_commands_effects();

  // Maybe we are restarting after a game-over sequence.
  if (get_equipment().get_life() <= 0) {
    get_equipment().restore_all_life();
  }

  // Launch the starting map.
  std::string starting_map_id = savegame->get_string(Savegame::KEY_STARTING_MAP);
  std::string starting_destination_name = savegame->get_string(Savegame::KEY_STARTING_POINT);

  bool valid_map_saved = false;
  if (!starting_map_id.empty()) {

    if (CurrentQuest::resource_exists(ResourceType::MAP, starting_map_id)) {
      // We are okay: the savegame file refers to an existing map.
      valid_map_saved = true;
    }
    else {
      // The savegame refers to a map that no longer exists.
      // Maybe the quest is in an intermediate development phase.
      // Show an error and fallback to the default map.
      Debug::error(std::string("The savegame refers to a non-existing map: '") + starting_map_id + "'");
    }
  }

  if (!valid_map_saved) {
    // When no valid starting map is set, use the first one declared in the
    // resource list file.
    const std::map<std::string, std::string>& maps =
        CurrentQuest::get_resources(ResourceType::MAP);
    if (maps.empty()) {
      Debug::die("This quest has no map");
    }
    starting_map_id = maps.begin()->first;
    starting_destination_name = "";  // Default destination.
  }

  set_current_map(starting_map_id, starting_destination_name, Transition::Style::FADE);
}

/**
 * \brief Starts this game.
 *
 * Does nothing if the game is already started.
 */
void Game::start() {

  if (started) {
    return;
  }

  started = true;
  get_savegame().notify_game_started();
  get_lua_context().game_on_started(*this);
}

/**
 * \brief Ends this game.
 *
 * Does nothing if the game is not started.
 */
void Game::stop() {

  if (!started) {
    return;
  }

  if (current_map != nullptr) {

    if (hero->is_on_map()) {
      // The hero was initialized.
      hero->notify_being_removed();
    }

    if (current_map->is_started()) {
      current_map->leave();
    }
    if (current_map->is_loaded()) {
      current_map->unload();
    }
  }

  get_lua_context().game_on_finished(*this);
  get_savegame().notify_game_finished();
  get_savegame().set_game(nullptr);

  Music::stop_playing();

  started = false;
}

/**
 * \brief Returns the Solarus main loop.
 * \return The main loop object.
 */
MainLoop& Game::get_main_loop() {
  return main_loop;
}

/**
 * \brief Returns the Lua context of this game.
 * \return The Lua context.
 */
LuaContext& Game::get_lua_context() {
  return main_loop.get_lua_context();
}

/**
 * \brief Returns the hero.
 * \return the hero
 */
const HeroPtr& Game::get_hero() {
  return hero;
}

/**
 * \brief Returns the game commands mapped to the keyboard and the joypad.
 * \return The game commands.
 */
GameCommands& Game::get_commands() {
  return *commands;
}

/**
 * \brief Returns the game commands mapped to the keyboard and the joypad.
 * \return The game commands.
 */
const GameCommands& Game::get_commands() const {
  return *commands;
}

/**
 * \brief Returns the current effect of the main keys (action, sword, pause, etc.).
 * \return the current effect of the main keys
 */
CommandsEffects& Game::get_commands_effects() {
  return commands_effects;
}

/**
 * \brief Returns the saved data associated to this game.
 * \return The saved data.
 */
Savegame& Game::get_savegame() {
  return *savegame;
}

/**
 * \brief Returns the saved data associated to this game.
 * \return The saved data.
 */
const Savegame& Game::get_savegame() const {
  return *savegame;
}

/**
 * \brief Returns the equipment of the player.
 *
 * It is equivalent to get_savegame().get_equipment().
 *
 * \return The equipment.
 */
Equipment& Game::get_equipment() {
  return get_savegame().get_equipment();
}

/**
 * \brief Returns the equipment of the player.
 *
 * It is equivalent to get_savegame().get_equipment().
 *
 * \return The equipment.
 */
const Equipment& Game::get_equipment() const {
  return get_savegame().get_equipment();
}

/**
 * \brief This function is called when a low-level input event occurs during the game.
 * \param event the event to handle
 * \return \c true if the event was handled and should stop being propagated.
 */
bool Game::notify_input(const InputEvent& event) {

  if (current_map != nullptr && current_map->is_loaded()) {
    bool handled = get_lua_context().game_on_input(*this, event);
    if (!handled) {
      handled = current_map->notify_input(event);
      if (!handled) {
        // Built-in behavior:
        // the GameCommands object will transform the low-level input event into
        // a high-level game command event (i.e. a command_pressed event or
        // a command_released event).
        commands->notify_input(event);
      }
    }
  }
  return true;
}

/**
 * \brief This function is called when a game command is pressed.
 * \param command A game command.
 */
void Game::notify_command_pressed(GameCommand command) {

  // Is a built-in dialog box being shown?
  if (is_dialog_enabled()) {
    if (dialog_box.notify_command_pressed(command)) {
      return;
    }
  }

  // See if the game script handles the command.
  if (get_lua_context().game_on_command_pressed(*this, command)) {
    return;
  }

  // See if the map script handled the command.
  if (get_lua_context().map_on_command_pressed(get_current_map(), command)) {
    return;
  }

  // Lua scripts did not override the command: do the built-in behavior.
  if (command == GameCommand::PAUSE) {
    if (is_paused()) {
      if (can_unpause()) {
        set_paused(false);
      }
    }
    else {
      if (can_pause()) {
        set_paused(true);
      }
    }
  }

  else if (!is_suspended()) {
    // When the game is not suspended, all other commands apply to the hero.
    hero->notify_command_pressed(command);
  }
}

/**
 * \brief This function is called when a game command is released.
 * \param command A game command.
 */
void Game::notify_command_released(GameCommand command) {

  bool handled = get_lua_context().game_on_command_released(*this, command);

  if (!handled) {

    handled = get_lua_context().map_on_command_released(get_current_map(), command);

    if (!handled) {
      // The Lua script did not override the command: do the built-in behavior.

      if (!is_suspended()) {
        // When the game is not suspended, the command apply to the hero.
        hero->notify_command_released(command);
      }
    }
  }
}

/**
 * \brief Updates the game elements.
 *
 * Updates the map, the equipment, the HUD, etc.
 */
void Game::update() {

  // update the transitions between maps
  update_transitions();

  if (restarting || !started) {
    return;
  }

  // update the map
  current_map->update();

  // call game:on_update() in Lua
  get_lua_context().game_on_update(*this);

  // update the equipment and HUD
  get_equipment().update();
  update_commands_effects();
}

/**
 * \brief Handles the transitions.
 *
 * This functions changes the map when needed and plays the
 * transitions between the two maps. This function is called
 * by the update() function.
 * Note that the two maps can actually be the same.
 */
void Game::update_transitions() {

  if (transition != nullptr) {
    transition->update();
  }

  // if the map has just changed, close the current map if any and play an out transition
  if (next_map != nullptr && transition == nullptr) { // the map has changed (i.e. set_current_map has been called)

    if (current_map == nullptr) { // special case: no map was playing, so we don't have any out transition to do
      current_map = next_map;
      next_map = nullptr;
    }
    else { // normal case: stop the control and play an out transition before leaving the current map
      transition = std::unique_ptr<Transition>(Transition::create(
          transition_style,
          Transition::Direction::CLOSING,
          *current_map->get_camera_surface(),
          this
      ));
      transition->start();
    }
  }

  Rectangle previous_map_location = current_map->get_location();

  // if a transition was playing and has just been finished
  if (transition != nullptr && transition->is_finished()) {

    Transition::Direction transition_direction = transition->get_direction();
    bool needs_previous_surface = transition->needs_previous_surface();
    transition = nullptr;

    MainLoop& main_loop = get_main_loop();
    if (restarting) {
      stop();
      main_loop.set_game(new Game(main_loop, savegame));
      this->savegame = nullptr;  // The new game is the owner.
    }
    else if (transition_direction == Transition::Direction::CLOSING) {

      bool world_changed = next_map != current_map &&
          (!next_map->has_world() || next_map->get_world() != current_map->get_world());

      if (world_changed) {
        // Reset the crystal blocks.
        crystal_state = false;
      }

      // Determine the destination to use and its name.
      std::string destination_name = next_map->get_destination_name();
      bool special_destination = destination_name == "_same" ||
          destination_name.substr(0,5) == "_side";
      StartingLocationMode starting_location_mode = StartingLocationMode::NO;
      if (!special_destination) {
        EntityPtr destination;
        if (destination_name.empty()) {
          std::shared_ptr<Destination> default_destination = next_map->get_entities().get_default_destination();
          if (default_destination != nullptr) {
            destination = default_destination;
            destination_name = destination->get_name();
          }
        }
        else {
          destination = next_map->get_entities().find_entity(destination_name);
        }
        if (destination != nullptr && destination->get_type() == EntityType::DESTINATION) {
          starting_location_mode =
              std::static_pointer_cast<Destination>(destination)->get_starting_location_mode();
        }
      }

      bool save_starting_location = false;
      switch (starting_location_mode) {

      case StartingLocationMode::YES:
        save_starting_location = true;
        break;

      case StartingLocationMode::NO:
        save_starting_location = false;
        break;

      case StartingLocationMode::WHEN_WORLD_CHANGES:
        save_starting_location = world_changed;
        break;
      }

      // Save the location if needed, except if this is a special destination.
      if (save_starting_location && !special_destination) {
        get_savegame().set_string(Savegame::KEY_STARTING_MAP, next_map->get_id());
        get_savegame().set_string(Savegame::KEY_STARTING_POINT, destination_name);
      }

      if (next_map == current_map) {
        // same map
        hero->place_on_destination(*current_map, previous_map_location);
        transition = std::unique_ptr<Transition>(Transition::create(
            transition_style,
            Transition::Direction::OPENING,
            *current_map->get_camera_surface(),
            this
        ));
        transition->start();
        next_map = nullptr;
      }
      else {

        // change the map
        current_map->leave();

        // before closing the map, draw it on a backup surface for transition effects
        // that want to display both maps at the same time
        if (needs_previous_surface && current_map->get_camera() != nullptr) {
          previous_map_surface = Surface::create(
              current_map->get_camera()->get_size()
          );
          current_map->draw();
          current_map->get_camera_surface()->draw(previous_map_surface);
        }

        // set the next map
        current_map->unload();

        current_map = next_map;
        next_map = nullptr;
      }
    }
    else {
      current_map->notify_opening_transition_finished();
      previous_map_surface = nullptr;
    }
  }

  // if a map has just been set as the current map, start it and play the in transition
  if (started && !current_map->is_started()) {
    Debug::check_assertion(current_map->is_loaded(), "This map is not loaded");
    transition = std::unique_ptr<Transition>(Transition::create(
        transition_style,
        Transition::Direction::OPENING,
        *current_map->get_camera_surface(),
        this
    ));

    if (previous_map_surface != nullptr) {
      // some transition effects need to display both maps simultaneously
      transition->set_previous_surface(previous_map_surface.get());
    }

    set_suspended_by_script(false);

    hero->place_on_destination(*current_map, previous_map_location);
    transition->start();
    current_map->start();
    notify_map_changed();
  }
}

/**
 * \brief Makes sure the keys effects are coherent with the hero's equipment and abilities.
 */
void Game::update_commands_effects() {

  // when the game is paused or a dialog box is shown, the sword key is not the usual one
  if (is_paused() || is_dialog_enabled()) {
    return; // if the game is interrupted for some other reason (e.g. a transition), let the normal sword icon
  }

  // make sure the sword key is coherent with having a sword
  if (get_equipment().has_ability(Ability::SWORD)
      && commands_effects.get_sword_key_effect() != CommandsEffects::ATTACK_KEY_SWORD) {

    commands_effects.set_sword_key_effect(CommandsEffects::ATTACK_KEY_SWORD);
  }
  else if (!get_equipment().has_ability(Ability::SWORD)
      && commands_effects.get_sword_key_effect() == CommandsEffects::ATTACK_KEY_SWORD) {

    commands_effects.set_sword_key_effect(CommandsEffects::ATTACK_KEY_NONE);
  }
}

/**
 * \brief Draws the game.
 * \param dst_surface The surface where the game will be drawn.
 */
void Game::draw(const SurfacePtr& dst_surface) {

  if (current_map == nullptr) {
    // Nothing to do. The game is not fully initialized yet.
    return;
  }

  // Draw the map.
  if (current_map->is_loaded()) {
    dst_surface->fill_with_color(current_map->get_tileset().get_background_color());
    current_map->draw();
    const CameraPtr& camera = current_map->get_camera();
    if (camera != nullptr) {
      const SurfacePtr& camera_surface = camera->get_surface();
      if (transition != nullptr) {
        transition->draw(*camera_surface);
      }
      camera_surface->draw(dst_surface, camera->get_position_on_screen());
    }

    // Draw the built-in dialog box if any.
    if (is_dialog_enabled()) {
      dialog_box.draw(dst_surface);
    }
  }

  get_lua_context().game_on_draw(*this, dst_surface);
}

/**
 * \brief Returns whether there is a current map in this game.
 *
 * This function always returns true except when the game is being created
 * and no map is loaded yet.
 *
 * \return true if there is a map
 */
bool Game::has_current_map() const {
  return current_map != nullptr;
}

/**
 * \brief Returns the current map.
 * \return the current map
 */
Map& Game::get_current_map() {
  return *current_map;
}

/**
 * \brief Changes the current map.
 *
 * Call this function when you want the hero to go to another map.
 *
 * \param map_id Id of the map to launch. It must exist.
 * \param destination_name Name of the destination point you want to use.
 * An empty string means the default one.
 * You can also use "_same" to keep the hero's coordinates, or
 * "_side0", "_side1", "_side2" or "_side3"
 * to place the hero on a side of the map.
 * \param transition_style Type of transition between the two maps.
 */
void Game::set_current_map(
    const std::string& map_id,
    const std::string& destination_name,
    Transition::Style transition_style) {

  if (current_map != nullptr) {
    // stop the hero's movement
    hero->reset_movement();
  }

  // prepare the next map
  if (current_map == nullptr || map_id != current_map->get_id()) {
    // another map
    next_map = std::make_shared<Map>(map_id);
    next_map->load(*this);
    next_map->check_suspended();
  }
  else {
    // same map
    next_map = current_map;
  }

  if (current_map != nullptr) {
    current_map->check_suspended();
  }

  next_map->set_destination(destination_name);
  this->transition_style = transition_style;
}

/**
 * \brief Notifies the game objects that the another map has just become active.
 */
void Game::notify_map_changed() {

  // Call game:on_map_changed() in Lua.
  get_lua_context().game_on_map_changed(*this, *current_map);

  // Notify the equipment.
  get_equipment().notify_map_changed(*current_map);
}

/**
 * \brief Returns the state of the crystal blocks.
 *
 * Returns false if the orange blocks are lowered or true if the blue blocks are lowered.
 *
 * \return the state of the crystals or this world
 */
bool Game::get_crystal_state() const {
  return crystal_state;
}

/**
 * \brief Changes the state of the crystal blocks.
 */
void Game::change_crystal_state() {
  crystal_state = !crystal_state;
}

/**
 * \brief Returns whether the game is paused.
 * \return true if the game is paused
 */
bool Game::is_paused() const {
  return paused;
}

/**
 * \brief Returns whether we are playing a transition between two maps.
 * \return true if there is a transition
 */
bool Game::is_playing_transition() const {
  return transition != nullptr || next_map != nullptr;
}

/**
 * \brief Returns whether the game is suspended.
 *
 * This is true in the following cases:
 * - the game is paused,
 * - or a dialog a being dispayed,
 * - or a transition between two maps is playing,
 * - or the game over sequence is active,
 * - or the camera is moving,
 * - or a script explicitly suspended the game.
 *
 * \return true if the game is suspended
 */
bool Game::is_suspended() const {

  return current_map == nullptr ||
      is_paused() ||
      is_dialog_enabled() ||
      is_playing_transition() ||
      is_showing_game_over() ||
      is_suspended_by_camera() ||
      is_suspended_by_script();
}

/**
 * \brief Returns whether a dialog is currently active.
 * \return true if a dialog box is being shown
 */
bool Game::is_dialog_enabled() const {
  return dialog_box.is_enabled();
}

/**
 * \brief Starts to show a dialog.
 *
 * No other dialog should be already running.
 * If the dialog does not exist, a non-fatal error message is printed
 * and no dialog is shown.
 *
 * \param dialog_id Id of the dialog to show.
 * \param info_ref Lua ref to an optional info parameter to pass to the
 * dialog box, or LUA_REFNIL.
 * \param callback_ref Lua ref to a function to call when the dialog finishes,
 * or an empty ref.
 */
void Game::start_dialog(
    const std::string& dialog_id,
    const ScopedLuaRef& info_ref,
    const ScopedLuaRef& callback_ref
) {
  if (!CurrentQuest::dialog_exists(dialog_id)) {
    Debug::error(std::string("No such dialog: '") + dialog_id + "'");
  }
  else {
    dialog_box.open(dialog_id, info_ref, callback_ref);
  }
}

/**
 * \brief Stops the dialog currently running if any.
 * \param status_ref Lua ref to a status value to return to the start_dialog
 * callback, or an empty ref. "skipped" means that the dialog was canceled by
 * the user.
 */
void Game::stop_dialog(const ScopedLuaRef& status_ref) {

  dialog_box.close(status_ref);
}

/**
 * \brief Returns whether the player can currently pause the game.
 *
 * He can pause the game if the pause command is enabled
 * and if his life is greater than zero.
 *
 * \return \c true if the player can currently pause the game.
 */
bool Game::can_pause() const {
  return !is_suspended()
      && is_pause_allowed()               // see if the map currently allows the pause command
      && get_equipment().get_life() > 0;  // don't allow to pause the game if the gameover sequence is about to start
}

/**
 * \brief Returns whether the player can currently unpause the game.
 * \return \c true if the player can currently unpause the game.
 */
bool Game::can_unpause() const {
  return is_paused()
      && is_pause_allowed()
      && !is_dialog_enabled();
}

/**
 * \brief Returns whether the pause command is available.
 *
 * Even when the pause command is available, the player may still
 * be unauthorized to pause the game, depending on the result of can_pause().
 *
 * \return \c true if the pause command is available.
 */
bool Game::is_pause_allowed() const {
  return pause_allowed;
}

/**
 * \brief Sets whether the pause command is available.
 *
 * Even when the pause command is available, the player may still
 * be unauthorized to pause the game, depending on the result of can_pause().
 *
 * \param pause_allowed \c true to make the pause command available.
 */
void Game::set_pause_allowed(bool pause_allowed) {

  this->pause_allowed = pause_allowed;
  commands_effects.set_pause_key_enabled(pause_allowed);
}

/**
 * \brief Pauses or resumes the game.
 * \param paused true to pause the game, false to resume it.
 */
void Game::set_paused(bool paused) {

  if (paused != is_paused()) {

    this->paused = paused;
    if (paused) {
      commands_effects.save_action_key_effect();
      commands_effects.set_action_key_effect(CommandsEffects::ACTION_KEY_NONE);
      commands_effects.save_sword_key_effect();
      commands_effects.set_sword_key_effect(CommandsEffects::ATTACK_KEY_NONE);
      commands_effects.set_pause_key_effect(CommandsEffects::PAUSE_KEY_RETURN);
      get_lua_context().game_on_paused(*this);
    }
    else {
      get_lua_context().game_on_unpaused(*this);
      commands_effects.restore_action_key_effect();
      commands_effects.restore_sword_key_effect();
      commands_effects.set_pause_key_effect(CommandsEffects::PAUSE_KEY_PAUSE);
    }
  }
}

/**
 * \brief Returns whether this game is currently suspended by a camera sequence.
 * \return \c true if a camera is suspending the game.
 */
bool Game::is_suspended_by_camera() const {

  if (current_map == nullptr) {
    return false;
  }

  // The game is suspended when the camera is scrolling on a separator.
  const CameraPtr& camera = current_map->get_camera();
  return camera->is_traversing_separator();
}

/**
 * \brief Returns whether this game is currently suspended by a script.
 * \return \c true if a script is suspending the game.
 */
bool Game::is_suspended_by_script() const {
  return suspended_by_script;
}

/**
 * \brief Sets whether this game is currently suspended by a script.
 * \param \c true to suspend the game, \c false to resume it
 * if nothing else is suspending it.
 */
void Game::set_suspended_by_script(bool suspended) {
  this->suspended_by_script = suspended;
}

/**
 * \brief Restarts the game with the current savegame state.
 */
void Game::restart() {

  if (current_map != nullptr) {
    transition = std::unique_ptr<Transition>(Transition::create(
        Transition::Style::FADE,
        Transition::Direction::CLOSING,
        *current_map->get_camera_surface(),
        this
    ));
    transition->start();
  }
  restarting = true;
}

/**
 * \brief Returns whether the gameover sequence is being shown.
 * \return true if the gameover sequence is being shown
 */
bool Game::is_showing_game_over() const {
  return showing_game_over;
}

/**
 * \brief Launches the game-over sequence.
 */
void Game::start_game_over() {

  Debug::check_assertion(!is_showing_game_over(),
      "The game-over sequence is already active");

  showing_game_over = true;

  if (!get_lua_context().game_on_game_over_started(*this)) {
    // The script does not define a game-over sequence:
    // then, the built-in behavior is to restart the game.
    restart();
    stop_game_over();
  }
}

/**
 * \brief Cancels the current game-over sequence.
 */
void Game::stop_game_over() {

  Debug::check_assertion(is_showing_game_over(),
      "The game-over sequence is not running");

  get_lua_context().game_on_game_over_finished(*this);
  showing_game_over = false;
  if (!restarting && !get_main_loop().is_resetting()) {
    // The hero gets back to life.
    current_map->check_suspended();  // To unsuspend the hero before making him blink.
    hero->notify_game_over_finished();
  }
}

/**
 * \brief Simulates pressing a game command.
 */
void Game::simulate_command_pressed(GameCommand command){
  commands->game_command_pressed(command);
}

/**
 * \brief Simulates releasing a game command.
 */
void Game::simulate_command_released(GameCommand command){
  commands->game_command_released(command);
}

}
