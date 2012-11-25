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
#include "Game.h"
#include "MainLoop.h"
#include "Map.h"
#include "Savegame.h"
#include "KeysEffect.h"
#include "Equipment.h"
#include "DialogBox.h"
#include "Treasure.h"
#include "Dungeon.h"
#include "GameoverSequence.h"
#include "DebugKeys.h"
#include "lua/LuaContext.h"
#include "hud/HUD.h"
#include "menus/PauseMenu.h"
#include "entities/Hero.h"
#include "lowlevel/Color.h"
#include "lowlevel/Surface.h"
#include "lowlevel/IniFile.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include "lowlevel/Music.h"
#include <sstream>

Rectangle Game::outside_world_size(0, 0, 0, 0); // loaded from quest.dat

/**
 * @brief Creates a game.
 * @param main_loop The Solarus root object.
 * @param savegame The saved data of this game. Will be deleted in the
 * destructor unless someone is still using it (the refcount info is used).
 */
Game::Game(MainLoop& main_loop, Savegame* savegame):

  main_loop(main_loop),
  savegame(savegame),
  pause_key_available(true),
  pause_menu(NULL), 
  gameover_sequence(NULL),
  resetting(false),
  restarting(false),
  keys_effect(NULL),
  current_map(NULL),
  next_map(NULL),
  previous_map_surface(NULL),
  transition_style(Transition::IMMEDIATE),
  transition(NULL),
  dungeon(NULL),
  crystal_state(false),
  hud(NULL),
  hud_enabled(true),
  dialog_box(NULL) {

  // notify objects
  get_savegame().increment_refcount();
  get_savegame().set_game(*this);

  // initialize members
  controls = new GameControls(*this);
  dialog_box = new DialogBox(*this);
  hero = new Hero(get_equipment());
  hero->increment_refcount();
  keys_effect = new KeysEffect();
  hud = new HUD(*this);

  // launch the starting map
  set_current_map(get_savegame().get_string(Savegame::KEY_STARTING_MAP),
      "", Transition::FADE);
}

/**
 * @brief Destroys the game.
 */
Game::~Game() {

  savegame->decrement_refcount();
  if (savegame->get_refcount() == 0) {
    // No one is using the savegame anymore (especially not Lua).
    delete savegame;
  }

  current_map->unload();
  current_map->decrement_refcount();
  if (current_map->get_refcount() == 0) {
    delete current_map;
  }

  Music::play(Music::none);

  delete transition;
  delete dialog_box;
  delete pause_menu;
  delete dungeon;
  delete gameover_sequence;
  delete keys_effect;
  delete hud;
  hero->decrement_refcount();
  if (hero->get_refcount() == 0) {
    delete hero;
  }
  delete controls;

  if (previous_map_surface != NULL) {
    delete previous_map_surface;
  }
}

/**
 * @brief Starts this screen.
 */
void Game::start() {
  get_lua_context().game_on_started(*this);
}

/**
 * @brief Ends this screen.
 */
void Game::stop() {
  get_equipment().notify_game_finished();
  get_lua_context().game_on_finished(*this);
}

/**
 * @brief Returns the Solarus main loop.
 * @return The main loop object.
 */
MainLoop& Game::get_main_loop() {
  return main_loop;
}

/**
 * @brief Returns the Lua context of this game.
 * @return The Lua context.
 */
LuaContext& Game::get_lua_context() {
  return main_loop.get_lua_context();
}

/**
 * @brief Returns the hero.
 * @return the hero
 */
Hero& Game::get_hero() {
  return *hero;
}

/**
 * @brief Returns the coordinates of the hero on the current map.
 *
 * The coordinates returned are the coordinates of the hero's origin point on the map.
 * The width and height are not used.
 *
 * @return the position of the hero
 */
const Rectangle& Game::get_hero_xy() {

  static Rectangle xy;
  xy = hero->get_xy();
  return xy;
}

/**
 * @brief Returns the game controls for the keyboard and the joypad.
 * @return the game controls
 */
GameControls& Game::get_controls() {
  return *controls;
}

/**
 * @brief Returns the current effect of the main keys (action, sword, pause, etc.).
 * @return the current effect of the main keys
 */
KeysEffect& Game::get_keys_effect() {
  return *keys_effect;
}

/**
 * @brief Returns the saved data associated to this game.
 * @return the saved data
 */
Savegame& Game::get_savegame() {
  return *savegame;
}

/**
 * @brief Returns the equipment of the player.
 *
 * It is equivalent to get_savegame().get_equipment().
 *
 * @return the equipment
 */
Equipment& Game::get_equipment() {
  return get_savegame().get_equipment();
}

/**
 * @brief This function is called when a low-level input event occurs during the game.
 * @param event the event to handle
 * @return \c true if the event was handled and should stop being propagated.
 */
bool Game::notify_input(InputEvent& event) {

  bool handled = false;

  if (current_map != NULL && current_map->is_loaded()) {
    handled = get_lua_context().game_on_input(*this, event);
    if (!handled) {
      handled = current_map->notify_input(event);
      if (!handled) {
        // Built-in behavior:
        // the GameControl object will transform the low-level input event into
        // a high-level game control event (i.e. a call to key_pressed() or key_released()).
        controls->notify_input(event);
      }
    }
  }
  return true;
}

/**
 * @brief This function is called when a game key is pressed.
 * @param key a key
 */
void Game::key_pressed(GameControls::GameKey key) {

  if (!is_suspended()) {

    if (key == GameControls::PAUSE) {
      if (can_pause()) {
        set_paused(true);
      }
    }
    else {
      // when the game is not suspended, all other keys apply to the hero
      hero->key_pressed(key);
    }
  }

  // is a message being shown?
  else if (is_showing_dialog()) {
    dialog_box->key_pressed(key);
  }

  // is the game paused?
  else if (is_paused()) {
    pause_menu->key_pressed(key);
  }

  // is the game over sequence shown?
  else if (is_showing_gameover()) {
    gameover_sequence->key_pressed(key);
  }
}

/**
 * @brief This function is called when a game key is released.
 * @param key a key
 */
void Game::key_released(GameControls::GameKey key) {

  if (!is_suspended()) {
    // if the game is not suspended, the keys apply to the hero
    hero->key_released(key);
  }
}

/**
 * @brief Updates the game elements.
 *
 * Updates the map, the equipment, the HUD, etc.
 */
void Game::update() {

  // update the transitions between maps
  update_transitions();

  if (resetting || restarting) {
    return; // the game may have just been reset
  }

  // update the map
  current_map->update();

  // call game:on_update() in Lua
  get_lua_context().game_on_update(*this);

  // update the equipment and HUD
  get_equipment().update();
  update_keys_effect();
  if (hud_enabled) {
    hud->update();
  }
  dialog_box->update();

  // update the pause menu (if the game is paused)
  if (is_paused()) {
    pause_menu->update();
  }

  // update the game over sequence (if any)
  if (is_showing_gameover()) {
    update_gameover_sequence();
  }
}

/**
 * @brief Handles the transitions.
 *
 * This functions changes the map when needed and plays the
 * transitions between the two maps. This function is called
 * by the update() function.
 * Note that the two maps can actually be the same.
 */
void Game::update_transitions() {

  if (transition != NULL) {
    transition->update();
  }

  // if the map has just changed, close the current map if any and play an out transition
  if (next_map != NULL && transition == NULL) { // the map has changed (i.e. set_current_map has been called)

    if (current_map == NULL) { // special case: no map was playing, so we don't have any out transition to do
      load_dungeon();
      current_map = next_map;
      next_map = NULL;
    }
    else { // normal case: stop the control and play an out transition before leaving the current map
      transition = Transition::create(transition_style, Transition::OUT, this);
      transition->start();
    }
  }

  Rectangle previous_map_location = current_map->get_location();

  // if a transition was playing and has just been finished
  if (transition != NULL && transition->is_finished()) {

    Transition::Direction transition_direction = transition->get_direction();
    bool needs_previous_surface = transition->needs_previous_surface();
    delete transition;
    transition = NULL;

    MainLoop& main_loop = get_main_loop();
    if (resetting) {
      current_map->unload();
      main_loop.set_resetting();
    }
    else if (restarting) {
      current_map->unload();
      main_loop.set_game(new Game(main_loop, savegame));
      savegame = NULL;  // The new game is the owner.
    }
    else if (transition_direction == Transition::OUT) {

      if (next_map == current_map) {
        // same map
        hero->place_on_destination(*current_map, previous_map_location);
        transition = Transition::create(transition_style, Transition::IN, this);
        transition->start();
        next_map = NULL;
      }
      else {

        // change the map
        current_map->leave();

        // special treatments for a transition between two different worlds
        // (e.g. outside world to a dungeon)
        if (next_map->get_world() != current_map->get_world()) {

          // reset the crystal blocks
          crystal_state = false;

          // save the location
          get_savegame().set_string(Savegame::KEY_STARTING_MAP, next_map->get_id());
          get_savegame().set_string(Savegame::KEY_STARTING_POINT, next_map->get_destination_name());
        }

        // before closing the map, draw it on a backup surface for transition effects
        // that want to display both maps at the same time
        if (needs_previous_surface) {
          previous_map_surface = new Surface(SOLARUS_SCREEN_WIDTH, SOLARUS_SCREEN_HEIGHT);
          current_map->draw();
          current_map->get_visible_surface().draw(*previous_map_surface);
        }

        // set the next map
        load_dungeon();
        current_map->unload();
        current_map->decrement_refcount();
        if (current_map->get_refcount() == 0) {
          delete current_map;
        }

        current_map = next_map;
        next_map = NULL;
      }
    }
    else {
      current_map->notify_opening_transition_finished();

      if (previous_map_surface != NULL) {
        delete previous_map_surface;
        previous_map_surface = NULL;
      }
    }
  }

  // if a map has just been set as the current map, start it and play the in transition
  if (!current_map->is_started()) {
    transition = Transition::create(transition_style, Transition::IN, this);

    if (previous_map_surface != NULL) {
      // some transition effects need to display both maps simultaneously
      transition->set_previous_surface(previous_map_surface);
    }

    hero->place_on_destination(*current_map, previous_map_location);
    transition->start();
    current_map->start();
  }
}

/**
 * @brief Makes sure the keys effects are coherent with the hero's equipment and abilities.
 */
void Game::update_keys_effect() {

  // when the game is paused or a dialog box is shown, the sword key is not the usual one
  if (is_paused() || is_showing_dialog()) {
    return; // if the game is interrupted for some other reason (e.g. a transition), let the normal sword icon
  }

  // make sure the sword key is coherent with having a sword
  if (get_equipment().has_ability("sword")
      && keys_effect->get_sword_key_effect() != KeysEffect::SWORD_KEY_SWORD) {

    keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_SWORD);
  }
  else if (!get_equipment().has_ability("sword")
      && keys_effect->get_sword_key_effect() == KeysEffect::SWORD_KEY_SWORD) {

    keys_effect->set_sword_key_effect(KeysEffect::SWORD_KEY_NONE);
  }
}

/**
 * @brief Updates the game over sequence.
 *
 * This function is called repeatedly while the game over sequence is shown.
 */
void Game::update_gameover_sequence() {

  if (!gameover_sequence->is_finished()) {
    gameover_sequence->update();
  }
  else {
    delete gameover_sequence;
    gameover_sequence = NULL;
  }
}

/**
 * @brief Draws the game.
 * @param dst_surface The surface where the game will be drawn.
 */
void Game::draw(Surface& dst_surface) {

  // draw the map
  if (current_map->is_loaded()) {
    current_map->draw();
    if (transition != NULL) {
      transition->draw(current_map->get_visible_surface());
    }
    current_map->get_visible_surface().draw(dst_surface);

    // draw the pause screen if any
    if (is_paused()) {
      pause_menu->draw(dst_surface);
    }

    // draw the game over sequence if any
    else if (is_showing_gameover()) {
      gameover_sequence->draw(dst_surface);
    }

    // draw the hud
    if (hud_enabled) {
      hud->draw(dst_surface);
    }

    // draw the dialog box if any
    if (is_showing_dialog()) {
      dialog_box->draw(dst_surface);
    }
  }

  get_lua_context().game_on_draw(*this, dst_surface);
}

/**
 * @brief Returns whether there is a current map in this game.
 *
 * This function always returns true except when the game is being created
 * and no map is loaded yet.
 *
 * @return true if there is a map
 */
bool Game::has_current_map() {
  return current_map != NULL;
}

/**
 * @brief Returns the current map.
 * @return the current map
 */
Map& Game::get_current_map() {
  return *current_map;
}

/**
 * @brief Changes the current map.
 *
 * Call this function when you want the hero to go to another map.
 *
 * @param map_id id of the map to launch
 * @param destination_name name of the destination point of the map you want to use,
 * or en ampty string to pick the destination point saved
 * @param transition_style type of transition between the two maps
 */
void Game::set_current_map(const std::string& map_id, const std::string &destination_name,
    Transition::Style transition_style) {

  // stop the hero's movement
  hero->reset_movement();

  // prepare the next map
  if (current_map == NULL || map_id != current_map->get_id()) {
    // another map
    next_map = new Map(map_id);
    next_map->increment_refcount();
    next_map->load(*this);
    next_map->check_suspended();
  }
  else {
    // same map
    next_map = current_map;
  }

  if (current_map != NULL) {
    current_map->check_suspended();
  }

  // initialize the destination point, from the specified name or from the savegame
  if (destination_name == "") {
    next_map->set_destination(get_savegame().get_string(Savegame::KEY_STARTING_POINT));
  }
  else {
    next_map->set_destination(destination_name);
  }
  this->transition_style = transition_style;
}

/**
 * @brief Returns the size of the oustide world in pixels.
 * @return the size of the oustide world
 */
const Rectangle& Game::get_outside_world_size() {

  if (outside_world_size.get_width() == 0) {
    // first time: read the information
    IniFile ini("quest.dat", IniFile::READ);
    ini.set_group("info");
    int width = ini.get_integer_value("outside_world_width", 0);
    int height = ini.get_integer_value("outside_world_height", 0);
    Debug::check_assertion(width > 0 && height > 0, "Missing outside world size in file quest.dat");
    outside_world_size.set_size(width, height);
  }
  return outside_world_size;
}

/**
 * @brief Returns whether the current map belongs to a dungeon.
 * @return true if the current map is in a dungeon
 */
bool Game::is_in_dungeon() {
  return has_current_map() && get_current_map().is_in_dungeon();
}

/**
 * @brief Returns the dungeon where the current map is.
 *
 * Returns NULL if we are not in a dungeon.
 *
 * @return the current dungeon
 */
Dungeon& Game::get_current_dungeon() {
  return *dungeon;
}

/**
 * @brief Returns the state of the crystal blocks.
 *
 * Returns false if the orange blocks are lowered or true if the blue blocks are lowered.
 *
 * @return the state of the crystals or this world
 */
bool Game::get_crystal_state() {
  return crystal_state;
}

/**
 * @brief Changes the state of the crystal blocks.
 */
void Game::change_crystal_state() {
  crystal_state = !crystal_state;
}

/**
 * @brief Returns whether the game is paused.
 * @return true if the game is paused
 */
bool Game::is_paused() {
  return pause_menu != NULL;
}

/**
 * @brief Returns whether we are playing a transition between two maps.
 * @return true if there is a transition
 */
bool Game::is_playing_transition() {
  return transition != NULL || next_map != NULL;
}

/**
 * @brief Returns whether the game is suspended.
 *
 * This is true in the following cases:
 * - the game is paused,
 * - a dialog a being dispayed,
 * - a transition between two maps is playing,
 * - the game over sequence is active,
 * - the camera is moving.
 *
 * @return true if the game is suspended
 */
bool Game::is_suspended() {

  return current_map == NULL
      || is_paused()
      || is_showing_dialog()
      || is_playing_transition()
      || is_showing_gameover()
      || !current_map->is_camera_fixed_on_hero();
}

/**
 * @brief Returns whether we are showing a dialog box.
 * @return true if a dialog box is being shown
 */
bool Game::is_showing_dialog() {
  return dialog_box->is_enabled();
}

/**
 * @brief Returns the dialog box manager.
 * @return the dialog box manager
 */
DialogBox& Game::get_dialog_box() {
  return *dialog_box;
}

/**
 * @brief Sets whether the HUD is currently displayed.
 * @param hud_enabled true to make the HUD displayed
 */
void Game::set_hud_enabled(bool hud_enabled) {
  this->hud_enabled = hud_enabled;
}

/**
 * @brief Returns whether the player is currently allowed to pause the game.
 *
 * He can pause the game if the pause key is enabled
 * and if his life is greater than zero.
 *
 * @return true if the player is currently allowed to pause the game
 */
bool Game::can_pause() {
  return is_pause_key_available()		// see if the map currently allows the pause key
    && get_equipment().get_life() > 0;	// don't allow to pause the game if the gameover sequence is about to start
}

/**
 * @brief Returns whether the pause key is available.
 *
 * Even when the pause key is available, the player may still
 * be unauthorized to pause the game, depending on the result of can_pause().
 *
 * @return true if the pause key is available
 */
bool Game::is_pause_key_available() {
  return pause_key_available;
}

/**
 * @brief Sets whether the pause key menu is available.
 *
 * Even when the pause key is available, the player may still
 * be unauthorized to pause the game, depending on the result of can_pause().
 *
 * @param pause_key_available true to make the pause key available
 */
void Game::set_pause_key_available(bool pause_key_available) {

  this->pause_key_available = pause_key_available;
  keys_effect->set_pause_key_enabled(pause_key_available);
}

/**
 * @brief Pauses or resumes the game.
 * @param paused true to pause the game, false to resume it.
 */
void Game::set_paused(bool paused) {

  if (paused != is_paused()) {

    if (paused) {
      pause_menu = new PauseMenu(*this);
    }
    else {
      delete pause_menu;
      pause_menu = NULL;
    }
  }
}

/**
 * @brief Loads the dungeon data for the current map.
 *
 * This function is called when the map changes.
 */
void Game::load_dungeon() {

  if (current_map == NULL || next_map->get_world() != current_map->get_world()) {

    if (current_map != NULL && current_map->is_in_dungeon()) {
      delete dungeon;
      dungeon = NULL;
    }

    if (next_map->is_in_dungeon()) {
      // TODO remove the notion of dungeon
      dungeon = new Dungeon(next_map->get_world()[8] - '0');
    }
  }
}

/**
 * @brief Ends the game and goes back to the initial screen.
 */
void Game::reset() {

  transition = Transition::create(Transition::FADE, Transition::OUT, this);
  transition->start();
  resetting = true;
}

/**
 * @brief Restarts the game with the current savegame state.
 */
void Game::restart() {

  transition = Transition::create(Transition::FADE, Transition::OUT, this);
  transition->start();
  restarting = true;
}

/**
 * @brief Launches the gameover sequence.
 */
void Game::start_gameover_sequence() {
  gameover_sequence = new GameoverSequence(*this, hero->get_animation_direction());
}

/**
 * @brief Returns whether the gameover sequence is being shown.
 * @return true if the gameover sequence is being shown
 */
bool Game::is_showing_gameover() {
  return gameover_sequence != NULL;
}

/**
 * @brief This function is called when the hero was dead but saved by a fairy.
 */
void Game::get_back_from_death() {
  hero->get_back_from_death();
}

