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
#ifndef ZSDX_CONTROLS_H
#define ZSDX_CONTROLS_H

#include "Common.h"
#include <map>

/**
 * Represents the game controls. This class handles the customization of the keyboard and joypad buttons.
 * It receives the SDL keyboard and joypad events that occur during
 * the game and then notifies the appropriate objects that a game key was pressed or released.
 * What we call a game key is a high-level notion such as "the action key".
 * The corresponding low-level event can be a keyboard event or a joypad event.
 */
class Controls {

 public:

  /**
   * The game keys that can be customized.
   * 
   */
  enum GameKey {
    NONE = 0,
    ACTION,
    SWORD,
    ITEM_1,
    ITEM_2,
    PAUSE,
    RIGHT,
    UP,
    LEFT,
    DOWN
  };
  
 private:

  Game *game;             /**< the game */
  Savegame *savegame;     /**< the savegame, which stores the keyboard and joypad mappings of the game keys */
  SDL_Joystick *joystick; /**< the joystick object */
  DebugKeys *debug_keys;  /**< special keys to debug the game, e.g. by traversing walls (disabled in release mode) */

  std::string game_key_names[9];                 /**< human name of each game key, in the current language */
  std::map<SDLKey, GameKey> keyboard_mapping;    /**< associates each game key to the keyboard key that triggers it */
  std::map<std::string, GameKey> joypad_mapping; /**< associates each game key to the joypad action that triggers it*/
  bool keys_pressed[9];                          /**< memorizes the state of each game key */
  static const uint16_t arrows_masks[4];           /**< bit mask associated to each direction key: this allows to
                                                  * store any combination of the 4 directions into a simple integer */

  bool customizing;                              /**< indicates that the next keyboard event will be considered as the new mapping for a game key */
  GameKey key_to_customize;                      /**< the game key to customize when customizing is true */

  static const std::string direction_names[4];   /**< English name of each arrow direction, used to represent a joypad action by a string */

  void game_key_pressed(GameKey key);
  void game_key_released(GameKey key);

  // keyboard mapping
  void key_pressed(const SDL_keysym &keysym);
  void key_released(const SDL_keysym &keysym);
  SDLKey get_keyboard_key(GameKey game_key);

  // joypad mapping
  void joypad_button_pressed(int button);
  void joypad_button_released(int button);
  void joypad_axis_moved(int axis, int state);
  void joypad_hat_moved(int hat, int value);

 public:

  // creation and destruction
  Controls(Game *game);
  ~Controls(void);

  // controls
  const std::string& get_key_name(GameKey game_key);
  const std::string get_keyboard_string(GameKey game_key);
  const std::string& get_joypad_string(GameKey key);
  void handle_event(const SDL_Event &event);
  bool is_key_pressed(GameKey game_key);
  int get_arrows_direction(void);

  // customization
  void customize(GameKey key);
  bool is_customizing(void);
  GameKey get_key_to_customize(void);
  bool is_customization_done(void);
};

#endif

