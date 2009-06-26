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
 * Represents the game controls.
 * This class handles the customization of the keyboard and joypad keys.
 * It receives the SDL keyboard and joypad events that occur during
 * the game and notifies the appropriate objects
 * that a key was pressed, depending on the game situation.
 */
class Controls {

 public:

  /**
   * The game keys that can be customized.
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

  Game *game;
  Savegame *savegame;
  SDL_Joystick *joystick;

  std::map<SDLKey, GameKey> keyboard_mapping;
  std::map<std::string, GameKey> joypad_mapping;
  bool keys_pressed[9];
  static const Uint16 arrows_masks[4];

  bool customizing;
  GameKey key_to_customize;

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
  bool is_customization_done(void);
};

#endif
