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
#include "Timer.h"
#include "ResourceManager.h"
#include "Game.h"
#include "lowlevel/Sound.h"
#include "lowlevel/System.h"

/**
 * Creates and starts a timer.
 * @param game the current game (if any)
 * @param duration duration of the timer in milliseconds
 * @param name a name to identify this timer
 * @param with_sound plays a sound until the timer expires
 */
Timer::Timer(Game *game, uint32_t duration, const std::string &name, bool with_sound):
  name(name), finished(false), suspended(false), when_suspended(0) {

  uint32_t now = System::now();
  expiration_date = now + duration;

  if (with_sound) {
    next_sound_date = now;
    countdown_sound = ResourceManager::get_sound("timer");
    countdown_hurry_sound = ResourceManager::get_sound("timer_hurry");
  }
  else {
    next_sound_date = 0;
  }

  // start the timer even if the game is suspended (e.g. a timer started during a camera movement)
  // except when it is suspended because of a dialog box
  if (game != NULL && game->is_showing_message()) {
    set_suspended(true);
  }
}

/**
 * Destructor.
 */
Timer::~Timer(void) {

}

/**
 * Returns the name identifying this timer.
 * @return the name of this timer
 */
const std::string& Timer::get_name(void) {
  return name;
}

/**
 * Returns whether the timer is finished.
 * @return true if the timer is finished
 */
bool Timer::is_finished(void) {
  return finished;
}

/**
 * Updates the timer.
 */
void Timer::update(void) {

  if (suspended || is_finished()) {
    return;
  }

  // check the time
  uint32_t now = System::now();
  finished = (now >= expiration_date);

  // play the sound
  if (next_sound_date != 0 && now >= next_sound_date) {

    uint32_t remaining_time = expiration_date - now;
    if (remaining_time > 6000) {
      countdown_sound->play();
      next_sound_date += 1000;
    }
    else {
      countdown_hurry_sound->play();
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
 * Suspends or resumes the timer.
 * @param suspended true to suspend the timer, false to resume it
 */
void Timer::set_suspended(bool suspended) {

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
      if (next_sound_date != 0) {
	next_sound_date += now - when_suspended;
      }
    }
  }
}

