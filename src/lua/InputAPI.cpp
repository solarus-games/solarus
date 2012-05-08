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
#include "lua/Script.h"
#include "lowlevel/Debug.h"
#include "lowlevel/StringConcat.h"
#include <lua.hpp>

const std::string Script::input_module_name = "sol.input";

std::map<InputEvent::KeyboardKey, std::string>
  Script::input_key_names;

/**
 * @brief Initializes the input features provided to Lua.
 */
void Script::register_input_module() {

  static const luaL_Reg functions[] = {
      // no function from Lua to C++ for inputs (yet?)
      { NULL, NULL }
  };
  // create the "sol.input" table anyway
  register_functions(input_module_name, functions);

  // initialize the map of keyboard key names
  input_key_names[InputEvent::KEY_BACKSPACE]             = "backspace";
  input_key_names[InputEvent::KEY_TABULATION]            = "tab";
  input_key_names[InputEvent::KEY_CLEAR]                 = "clear";
  input_key_names[InputEvent::KEY_RETURN]                = "return";
  input_key_names[InputEvent::KEY_PAUSE]                 = "pause";
  input_key_names[InputEvent::KEY_ESCAPE]                = "escape";
  input_key_names[InputEvent::KEY_SPACE]                 = "space";
  input_key_names[InputEvent::KEY_EXCLAMATION_MARK]      = "!";
  input_key_names[InputEvent::KEY_DOULE_QUOTE]           = "\"";
  input_key_names[InputEvent::KEY_HASH]                  = "#";
  input_key_names[InputEvent::KEY_DOLLAR]                = "$";
  input_key_names[InputEvent::KEY_AMPERSAND]             = "&";
  input_key_names[InputEvent::KEY_SINGLE_QUOTE]          = "'";
  input_key_names[InputEvent::KEY_LEFT_PARENTHESIS]      = "(";
  input_key_names[InputEvent::KEY_RIGHT_PARENTHESIS]     = ")";
  input_key_names[InputEvent::KEY_ASTERISK]              = "*";
  input_key_names[InputEvent::KEY_PLUS]                  = "+";
  input_key_names[InputEvent::KEY_COMMA]                 = ",";
  input_key_names[InputEvent::KEY_MINUS]                 = "-";
  input_key_names[InputEvent::KEY_PERIOD]                = ".";
  input_key_names[InputEvent::KEY_SLASH]                 = "/";
  input_key_names[InputEvent::KEY_0]                     = "0";
  input_key_names[InputEvent::KEY_1]                     = "1";
  input_key_names[InputEvent::KEY_2]                     = "2";
  input_key_names[InputEvent::KEY_3]                     = "3";
  input_key_names[InputEvent::KEY_4]                     = "4";
  input_key_names[InputEvent::KEY_5]                     = "5";
  input_key_names[InputEvent::KEY_6]                     = "6";
  input_key_names[InputEvent::KEY_7]                     = "7";
  input_key_names[InputEvent::KEY_8]                     = "8";
  input_key_names[InputEvent::KEY_9]                     = "9";
  input_key_names[InputEvent::KEY_COLON]                 = ".";
  input_key_names[InputEvent::KEY_SEMICOLON]             = ":";
  input_key_names[InputEvent::KEY_LESS]                  = "<";
  input_key_names[InputEvent::KEY_EQUALS]                = "=";
  input_key_names[InputEvent::KEY_GREATER]               = ">";
  input_key_names[InputEvent::KEY_QUESTION_MARK]         = "?";
  input_key_names[InputEvent::KEY_AT]                    = "@";
  input_key_names[InputEvent::KEY_LEFT_BRACKET]          = "[";
  input_key_names[InputEvent::KEY_BACKSLASH]             = "\\";
  input_key_names[InputEvent::KEY_RIGHT_BRACKET]         = "]";
  input_key_names[InputEvent::KEY_CARET]                 = "^";
  input_key_names[InputEvent::KEY_UNDERSCORE]            = "_";
  input_key_names[InputEvent::KEY_BACKQUOTE]             = "`";
  input_key_names[InputEvent::KEY_a]                     = "a";
  input_key_names[InputEvent::KEY_b]                     = "b";
  input_key_names[InputEvent::KEY_c]                     = "c";
  input_key_names[InputEvent::KEY_d]                     = "d";
  input_key_names[InputEvent::KEY_e]                     = "e";
  input_key_names[InputEvent::KEY_f]                     = "f";
  input_key_names[InputEvent::KEY_g]                     = "g";
  input_key_names[InputEvent::KEY_h]                     = "h";
  input_key_names[InputEvent::KEY_i]                     = "i";
  input_key_names[InputEvent::KEY_j]                     = "j";
  input_key_names[InputEvent::KEY_k]                     = "k";
  input_key_names[InputEvent::KEY_l]                     = "l";
  input_key_names[InputEvent::KEY_m]                     = "m";
  input_key_names[InputEvent::KEY_n]                     = "n";
  input_key_names[InputEvent::KEY_o]                     = "o";
  input_key_names[InputEvent::KEY_p]                     = "p";
  input_key_names[InputEvent::KEY_q]                     = "q";
  input_key_names[InputEvent::KEY_r]                     = "r";
  input_key_names[InputEvent::KEY_s]                     = "s";
  input_key_names[InputEvent::KEY_t]                     = "t";
  input_key_names[InputEvent::KEY_u]                     = "u";
  input_key_names[InputEvent::KEY_v]                     = "v";
  input_key_names[InputEvent::KEY_w]                     = "w";
  input_key_names[InputEvent::KEY_x]                     = "x";
  input_key_names[InputEvent::KEY_y]                     = "y";
  input_key_names[InputEvent::KEY_z]                     = "z";
  input_key_names[InputEvent::KEY_DELETE]                = "delete";
  input_key_names[InputEvent::KEY_KP0]                   = "kp 0";
  input_key_names[InputEvent::KEY_KP1]                   = "kp 1";
  input_key_names[InputEvent::KEY_KP2]                   = "kp 2";
  input_key_names[InputEvent::KEY_KP3]                   = "kp 3";
  input_key_names[InputEvent::KEY_KP4]                   = "kp 4";
  input_key_names[InputEvent::KEY_KP5]                   = "kp 5";
  input_key_names[InputEvent::KEY_KP6]                   = "kp 6";
  input_key_names[InputEvent::KEY_KP7]                   = "kp 7";
  input_key_names[InputEvent::KEY_KP8]                   = "kp 8";
  input_key_names[InputEvent::KEY_KP9]                   = "kp 9";
  input_key_names[InputEvent::KEY_KP_PERIOD]             = "kp .";
  input_key_names[InputEvent::KEY_KP_DIVIDE]             = "kp /";
  input_key_names[InputEvent::KEY_KP_MULTIPLY]           = "kp *";
  input_key_names[InputEvent::KEY_KP_MINUS]              = "kp -";
  input_key_names[InputEvent::KEY_KP_PLUS]               = "kp +";
  input_key_names[InputEvent::KEY_KP_ENTER]              = "kp return";
  input_key_names[InputEvent::KEY_KP_EQUALS]             = "kp =";
  input_key_names[InputEvent::KEY_UP]                    = "up";
  input_key_names[InputEvent::KEY_DOWN]                  = "down";
  input_key_names[InputEvent::KEY_RIGHT]                 = "right";
  input_key_names[InputEvent::KEY_LEFT]                  = "left";
  input_key_names[InputEvent::KEY_INSERT]                = "insert";
  input_key_names[InputEvent::KEY_HOME]                  = "home";
  input_key_names[InputEvent::KEY_END]                   = "end";
  input_key_names[InputEvent::KEY_PAGE_UP]               = "page up";
  input_key_names[InputEvent::KEY_PAGE_DOWN]             = "page down";
  input_key_names[InputEvent::KEY_F1]                    = "f1";
  input_key_names[InputEvent::KEY_F2]                    = "f2";
  input_key_names[InputEvent::KEY_F3]                    = "f3";
  input_key_names[InputEvent::KEY_F4]                    = "f4";
  input_key_names[InputEvent::KEY_F5]                    = "f5";
  input_key_names[InputEvent::KEY_F6]                    = "f6";
  input_key_names[InputEvent::KEY_F7]                    = "f7";
  input_key_names[InputEvent::KEY_F8]                    = "f8";
  input_key_names[InputEvent::KEY_F9]                    = "f9";
  input_key_names[InputEvent::KEY_F10]                   = "f10";
  input_key_names[InputEvent::KEY_F11]                   = "f11";
  input_key_names[InputEvent::KEY_F12]                   = "f12";
  input_key_names[InputEvent::KEY_F13]                   = "f13";
  input_key_names[InputEvent::KEY_F14]                   = "f14";
  input_key_names[InputEvent::KEY_F15]                   = "f15";
  input_key_names[InputEvent::KEY_NUMLOCK]               = "num lock";
  input_key_names[InputEvent::KEY_CAPSLOCK]              = "caps lock";
  input_key_names[InputEvent::KEY_SCROLLOCK]             = "scroll lock";
  input_key_names[InputEvent::KEY_RIGHT_SHIFT]           = "right shift";
  input_key_names[InputEvent::KEY_LEFT_SHIFT]            = "left shift";
  input_key_names[InputEvent::KEY_RIGHT_CONTROL]         = "right control";
  input_key_names[InputEvent::KEY_LEFT_CONTROL]          = "left control";
  input_key_names[InputEvent::KEY_RIGHT_ALT]             = "right alt";
  input_key_names[InputEvent::KEY_LEFT_ALT]              = "left alt";
  input_key_names[InputEvent::KEY_RIGHT_META]            = "right meta";
  input_key_names[InputEvent::KEY_LEFT_META]             = "left meta";
  input_key_names[InputEvent::KEY_LEFT_WINDOWS]          = "right windows";
  input_key_names[InputEvent::KEY_RIGHT_WINDOWS]         = "left windows";
}

/**
 * @brief Returns the name of a keyboard key for Lua.
 * @param key a keyboard key
 * @return a string with the name of this key for the Lua API,
 * or an empty string if this key is unknown
 */
const std::string& Script::input_get_key_name(InputEvent::KeyboardKey key) {

  return input_key_names[key];
}

