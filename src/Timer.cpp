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
#include "Timer.h"
#include "Game.h"
#include "lua/LuaContext.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"

/**
 * @brief Creates and starts a timer.
 * @param delay duration of the timer in milliseconds
 */
Timer::Timer(uint32_t delay):
  expiration_date(System::now() + delay),
  finished(false),
  suspended_with_map(false),
  suspended(false),
  when_suspended(0),
  next_sound_date(0) {

}

/**
 * @brief Destructor.
 */
Timer::~Timer() {

}

/**
 * @brief Returns whether a clock sound is played during this timer.
 * @return true if a clock sound is played.
 */
bool Timer::is_with_sound() {
  return next_sound_date != 0;
}

/**
 * @brief Sets whether a clock sound is played during this timer.
 * @param with_sound true to play a clock sound during this timer.
 */
void Timer::set_with_sound(bool with_sound) {
  next_sound_date = with_sound ? System::now() : 0;
}

/**
 * @brief Returns whether this timer is currently suspended.
 * @return true if this timer is suspended.
 */
bool Timer::is_suspended() {
  return suspended;
}

/**
 * @brief Suspends or resumes the timer.
 *
 * It is okay to call this function when is_suspended_with_map() is true:
 * this means that you temporarily override the automatic suspending behavior.
 *
 * @param suspended true to suspend the timer, false to resume it.
 */
void Timer::set_suspended(bool suspended) {

  if (suspended != this->suspended) {
    this->suspended = suspended;

    uint32_t now = System::now();

    if (suspended) {
      // the timer is being suspended
      when_suspended = now;
    }
    else {
      // recalculate the expiration date
      if (when_suspended != 0) {
        expiration_date += now - when_suspended;
        if (is_with_sound()) {
          next_sound_date += now - when_suspended;
        }
      }
    }
  }
}

/**
 * @brief Returns whether this timer is suspended when the map is suspended.
 * @return true if this timer is suspended with the map.
 */
bool Timer::is_suspended_with_map() {
  return suspended_with_map;
}

/**
 * @brief Sets whether this timer should be suspended when the map is suspended.
 * @param suspended_with_map true to suspend this timer with the map.
 */
void Timer::set_suspended_with_map(bool suspended_with_map) {

  if (suspended_with_map != this->suspended_with_map) {
    this->suspended_with_map = suspended_with_map;

    if (is_suspended() && !suspended_with_map) {
      set_suspended(false);
    }
  }
}

/**
 * @brief Returns whether the timer is finished.
 * @return true if the timer is finished
 */
bool Timer::is_finished() {
  return finished;
}

/**
 * @brief Updates the timer.
 */
void Timer::update() {

  if (suspended || is_finished()) {
    return;
  }

  // check the time
  uint32_t now = System::now();
  finished = (now >= expiration_date);

  // play the sound
  if (is_with_sound() && now >= next_sound_date) {

    uint32_t remaining_time = expiration_date - now;
    if (remaining_time > 6000) {
      Sound::play("timer");
      next_sound_date += 1000;
    }
    else {
      Sound::play("timer_hurry");
      if (remaining_time > 2000) {
        next_sound_date += 1000;
      }
      else {
        next_sound_date += 250;
      }
    }
  }
}

/**
 * @brief Notifies this timer that the current map is being suspended or resumed.
 * @param suspended true if the map is suspended, false if it is resumed.
 */
void Timer::notify_map_suspended(bool suspended) {

  if (suspended_with_map || !suspended) {
    set_suspended(suspended);
  }
}

/**
 * @brief Returns the name identifying this type in Lua.
 * @return the name identifying this type in Lua
 */
const std::string& Timer::get_lua_type_name() const {
  return LuaContext::timer_module_name;
}

