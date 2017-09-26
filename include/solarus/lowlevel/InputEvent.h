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
#ifndef SOLARUS_INPUT_EVENT_H
#define SOLARUS_INPUT_EVENT_H

#include "solarus/Common.h"
#include "solarus/EnumInfo.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>
#include <SDL_events.h>
#include <SDL_joystick.h>

namespace Solarus {

class Point;
class Rectangle;

/**
 * \brief Represents a low-level event.
 *
 * This low-level class provides an API for input events (keyboard, joypad and window events).
 * It encapsulates the library-dependent events.
 */
class InputEvent {

  public:

    /**
     * Enumeration of keyboard keys.
     * This is a Solarus dedicated enumeration.
     * To make the SDL encapsulation easier, this enumeration is compatible
     * with the SDL_Keycode enumeration from SDL.
     * If you intend to replace SDL by another library one day, the API of the
     * Input class will not have to change
     * (only its implementation will).
     * The numeric values of the enumeration can be changed without breaking
     * savegames because savegames use a string name to store the commands.
     */
    enum class KeyboardKey {
      NONE                     = SDLK_UNKNOWN,
      BACKSPACE                = SDLK_BACKSPACE,
      TABULATION               = SDLK_TAB,
      CLEAR                    = SDLK_CLEAR,
      RETURN                   = SDLK_RETURN,
      PAUSE                    = SDLK_PAUSE,
      ESCAPE                   = SDLK_ESCAPE,
      SPACE                    = SDLK_SPACE,
      EXCLAMATION_MARK         = SDLK_EXCLAIM,
      DOULE_QUOTE              = SDLK_QUOTEDBL,
      HASH                     = SDLK_HASH,
      DOLLAR                   = SDLK_DOLLAR,
      AMPERSAND                = SDLK_AMPERSAND,
      SINGLE_QUOTE             = SDLK_QUOTE,
      LEFT_PARENTHESIS         = SDLK_LEFTPAREN,
      RIGHT_PARENTHESIS        = SDLK_RIGHTPAREN,
      ASTERISK                 = SDLK_ASTERISK,
      PLUS                     = SDLK_PLUS,
      COMMA                    = SDLK_COMMA,
      MINUS                    = SDLK_MINUS,
      PERIOD                   = SDLK_PERIOD,
      SLASH                    = SDLK_SLASH,
      NUMBER_0                 = SDLK_0,
      NUMBER_1                 = SDLK_1,
      NUMBER_2                 = SDLK_2,
      NUMBER_3                 = SDLK_3,
      NUMBER_4                 = SDLK_4,
      NUMBER_5                 = SDLK_5,
      NUMBER_6                 = SDLK_6,
      NUMBER_7                 = SDLK_7,
      NUMBER_8                 = SDLK_8,
      NUMBER_9                 = SDLK_9,
      COLON                    = SDLK_COLON,
      SEMICOLON                = SDLK_SEMICOLON,
      LESS                     = SDLK_LESS,
      EQUALS                   = SDLK_EQUALS,
      GREATER                  = SDLK_GREATER,
      QUESTION_MARK            = SDLK_QUESTION,
      AT                       = SDLK_AT,
      LEFT_BRACKET             = SDLK_LEFTBRACKET,
      BACKSLASH                = SDLK_BACKSLASH,
      RIGHT_BRACKET            = SDLK_RIGHTBRACKET,
      CARET                    = SDLK_CARET,
      UNDERSCORE               = SDLK_UNDERSCORE,
      BACKQUOTE                = SDLK_BACKQUOTE,
      a                        = SDLK_a,
      b                        = SDLK_b,
      c                        = SDLK_c,
      d                        = SDLK_d,
      e                        = SDLK_e,
      f                        = SDLK_f,
      g                        = SDLK_g,
      h                        = SDLK_h,
      i                        = SDLK_i,
      j                        = SDLK_j,
      k                        = SDLK_k,
      l                        = SDLK_l,
      m                        = SDLK_m,
      n                        = SDLK_n,
      o                        = SDLK_o,
      p                        = SDLK_p,
      q                        = SDLK_q,
      r                        = SDLK_r,
      s                        = SDLK_s,
      t                        = SDLK_t,
      u                        = SDLK_u,
      v                        = SDLK_v,
      w                        = SDLK_w,
      x                        = SDLK_x,
      y                        = SDLK_y,
      z                        = SDLK_z,
      DELETE                   = SDLK_DELETE,

      KP0                      = SDLK_KP_0,
      KP1                      = SDLK_KP_1,
      KP2                      = SDLK_KP_2,
      KP3                      = SDLK_KP_3,
      KP4                      = SDLK_KP_4,
      KP5                      = SDLK_KP_5,
      KP6                      = SDLK_KP_6,
      KP7                      = SDLK_KP_7,
      KP8                      = SDLK_KP_8,
      KP9                      = SDLK_KP_9,
      KP_PERIOD                = SDLK_KP_PERIOD,
      KP_DIVIDE                = SDLK_KP_DIVIDE,
      KP_MULTIPLY              = SDLK_KP_MULTIPLY,
      KP_MINUS                 = SDLK_KP_MINUS,
      KP_PLUS                  = SDLK_KP_PLUS,
      KP_ENTER                 = SDLK_KP_ENTER,
      KP_EQUALS                = SDLK_KP_EQUALS,

      UP                       = SDLK_UP,
      DOWN                     = SDLK_DOWN,
      RIGHT                    = SDLK_RIGHT,
      LEFT                     = SDLK_LEFT,
      INSERT                   = SDLK_INSERT,
      HOME                     = SDLK_HOME,
      END                      = SDLK_END,
      PAGE_UP                  = SDLK_PAGEUP,
      PAGE_DOWN                = SDLK_PAGEDOWN,

      F1                       = SDLK_F1,
      F2                       = SDLK_F2,
      F3                       = SDLK_F3,
      F4                       = SDLK_F4,
      F5                       = SDLK_F5,
      F6                       = SDLK_F6,
      F7                       = SDLK_F7,
      F8                       = SDLK_F8,
      F9                       = SDLK_F9,
      F10                      = SDLK_F10,
      F11                      = SDLK_F11,
      F12                      = SDLK_F12,
      F13                      = SDLK_F13,
      F14                      = SDLK_F14,
      F15                      = SDLK_F15,

      NUMLOCK                  = SDLK_NUMLOCKCLEAR,
      CAPSLOCK                 = SDLK_CAPSLOCK,
      SCROLLOCK                = SDLK_SCROLLLOCK,
      RIGHT_SHIFT              = SDLK_RSHIFT,
      LEFT_SHIFT               = SDLK_LSHIFT,
      RIGHT_CONTROL            = SDLK_RCTRL,
      LEFT_CONTROL             = SDLK_LCTRL,
      RIGHT_ALT                = SDLK_RALT,
      LEFT_ALT                 = SDLK_LALT,
      RIGHT_META               = SDLK_RGUI,
      LEFT_META                = SDLK_LGUI
    };

    /**
     * Enumeration of mouse buttons.
     * This is a Solarus dedicated enumeration.
     * To make the SDL encapsulation easier, this enumeration is compatible
     * with SDL_BUTTON definitions from SDL.
     * If you intend to replace SDL by another library one day, the API of the
     * Input class will not have to change
     * (only its implementation will).
     */
    enum class MouseButton {
      NONE            = -1,
      LEFT            = SDL_BUTTON_LEFT,
      MIDDLE          = SDL_BUTTON_MIDDLE,
      RIGHT           = SDL_BUTTON_RIGHT,
      X1              = SDL_BUTTON_X1,
      X2              = SDL_BUTTON_X2,
    };

    static void initialize();
    static void quit();
    static bool is_initialized();

    // retrieve the current event
    static std::unique_ptr<InputEvent> get_event();

    // global information
    static void set_key_repeat(bool repeat);
    static bool is_shift_down();
    static bool is_control_down();
    static bool is_alt_down();
    static bool is_caps_lock_on();
    static bool is_num_lock_on();
    static bool is_key_down(KeyboardKey key);
    static bool is_joypad_button_down(int button);
    static bool is_mouse_button_down(MouseButton button);
    static bool is_finger_down(int finger_id);
    static int get_joypad_axis_state(int axis);
    static int get_joypad_hat_direction(int hat);
    static Point get_global_mouse_position();
    static bool get_global_finger_position(int finger_id, Point& finger_xy);
    static bool get_global_finger_pressure(int finger_id, float& finger_pressure);

    // event type
    bool is_valid() const;
    bool is_keyboard_event() const;
    bool is_joypad_event() const;
    bool is_mouse_event() const;
    bool is_finger_event() const;
    bool is_window_event() const;

    // keyboard
    bool is_keyboard_key_pressed() const;
    bool is_keyboard_key_pressed(KeyboardKey key) const;
    bool is_keyboard_key_pressed(const KeyboardKey* keys) const;
    bool is_keyboard_direction_key_pressed() const;
    bool is_keyboard_non_direction_key_pressed() const;

    bool is_keyboard_key_released() const;
    bool is_keyboard_key_released(KeyboardKey key) const;
    bool is_keyboard_key_released(const KeyboardKey* keys) const;
    bool is_keyboard_direction_key_released() const;
    bool is_keyboard_non_direction_key_released() const;

    bool is_with_shift() const;
    bool is_with_control() const;
    bool is_with_alt() const;

    KeyboardKey get_keyboard_key() const;

    bool is_character_pressed() const;
    std::string get_character() const;

    static void simulate_key_pressed(KeyboardKey key);
    static void simulate_key_released(KeyboardKey key);

    // joypad
    static bool is_joypad_enabled();
    static void set_joypad_enabled(bool joypad_enabled);

    bool is_joypad_button_pressed() const;
    bool is_joypad_button_released() const;
    int get_joypad_button() const;
    bool is_joypad_axis_moved() const;
    int get_joypad_axis() const;
    int get_joypad_axis_state() const;
    bool is_joypad_axis_centered() const;
    bool is_joypad_hat_moved() const;
    int get_joypad_hat() const;
    int get_joypad_hat_direction() const;
    bool is_joypad_hat_centered() const;

    // mouse
    bool is_mouse_button_pressed() const;
    bool is_mouse_button_pressed(MouseButton button) const;
    bool is_mouse_button_released() const;
    bool is_mouse_button_released(MouseButton button) const;

    MouseButton get_mouse_button() const;
    bool get_mouse_position(Point& mouse_xy) const;

    // touch finger
    bool is_finger_pressed() const;
    bool is_finger_pressed(int finger_id) const;
    bool is_finger_released() const;
    bool is_finger_released(int finger_id) const;
    bool is_finger_moved() const;
    bool is_finger_moved(int finger_id) const;

    int get_finger() const;
    Point get_finger_position() const;
    Point get_finger_distance() const;
    float get_finger_pressure() const;

    // functions common to keyboard, joypad and mouse
    int get_direction() const;
    bool is_pressed() const;
    bool is_direction_pressed() const;
    bool is_non_direction_pressed() const;
    bool is_released() const;

    // window event
    bool is_window_closing() const;

  private:

    explicit InputEvent(const SDL_Event& event);

    static const KeyboardKey directional_keys[];  /**< array of the keyboard directional keys */
    static bool initialized;                      /**< Whether the input manager is initialized. */
    static bool joypad_enabled;                   /**< true if joypad support is enabled
                                                   * (may be true even without joypad plugged) */
    static SDL_Joystick* joystick;                /**< the joystick object if enabled and plugged */
    static std::vector<int> joypad_axis_state;    /**< keep track of the current horizontal and vertical axis states */
    static std::map<KeyboardKey, std::string>
      keyboard_key_names;                         /**< Names of all existing keyboard keys. */
    static std::map<MouseButton, std::string>
      mouse_button_names;                         /**< Names of all existing mouse buttons. */
    static bool repeat_keyboard;                  /**< True to handle repeat KEYDOWN and KEYUP events. */
    static std::set<SDL_Keycode> keys_pressed;    /**< Keys currently down, only according to SDL_KEYDOWN and SDL_KEYUP events
                                                   * (i.e. independently of the real current state SDL_GetKeyboardState()). */

    const SDL_Event internal_event;               /**< the internal event encapsulated */

};

template <>
struct SOLARUS_API EnumInfoTraits<InputEvent::KeyboardKey> {
  static const std::string pretty_name;

  static const EnumInfo<InputEvent::KeyboardKey>::names_type names;
};

template <>
struct SOLARUS_API EnumInfoTraits<InputEvent::MouseButton> {
  static const std::string pretty_name;

  static const EnumInfo<InputEvent::MouseButton>::names_type names;
};

}

#endif

