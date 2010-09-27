/*
 * Copyright (C) 2009 Christopho, Solarus - http://www.solarus-engine.org
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
#include "hero/SwordTappingState.h"
#include "hero/SwordLoadingState.h"
#include "hero/HeroSprites.h"
#include "entities/Detector.h"
#include "lowlevel/System.h"
#include "lowlevel/Sound.h"
#include "Game.h"
#include "GameControls.h"
#include "Map.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 */
Hero::SwordTappingState::SwordTappingState(Hero &hero):
  State(hero), next_sound_date(0) {

}

/**
 * @brief Destructor.
 */
Hero::SwordTappingState::~SwordTappingState() {

}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::SwordTappingState::start(State *previous_state) {

  State::start(previous_state);

  get_sprites().set_animation_sword_tapping();
  next_sound_date = System::now() + 100;
}

/**
 * @brief Updates this state.
 */
void Hero::SwordTappingState::update() {

  State::update();

  const Rectangle &facing_point = hero.get_facing_point();

  if (!get_controls().is_key_pressed(GameControls::SWORD)
      || get_controls().get_wanted_direction8() != get_sprites().get_animation_direction8()
      || !get_map().test_collision_with_obstacles(hero.get_layer(), facing_point.get_x(), facing_point.get_y(), &hero)) {
    // the sword key has been released, the player has moved or the obstacle is gone

    if (get_sprites().get_current_frame() >= 5) {
      // when the animation is ok, stop tapping the wall, go back to loading the sword
      hero.set_state(new SwordLoadingState(hero));
    }
  }
  else {

    // play the sound every 100 ms
    uint32_t now = System::now();
    if (get_sprites().get_current_frame() == 3 && now >= next_sound_date) {

      Detector *facing_entity = hero.get_facing_entity();
      SoundId sound_id;
      if (facing_entity != NULL) {
	sound_id = facing_entity->get_sword_tapping_sound();
      }
      else {
	sound_id = "sword_tapping";
      }
      Sound::play(sound_id);
      next_sound_date = now + 100;
    }
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::SwordTappingState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    next_sound_date += System::now() - when_suspended;
  }
}

/**
 * @brief Returns whether crystal switches can be activated by the sword in this state.
 * @return true if crystal switches can be activated by the sword in this state
 */
bool Hero::SwordTappingState::can_sword_hit_crystal_switch() {
  return true;
}

/**
 * @brief Tests whether the hero is cutting with his sword the specified detector
 * for which a collision was detected.
 * @param detector the detector to check
 * @return true if the sword is cutting this detector
 */
bool Hero::SwordTappingState::is_cutting_with_sword(Detector *detector) {

  return detector->is_obstacle_for(&hero)	// only obstacle entities can be cut
    && hero.get_facing_entity() == detector	// only one entity at a time
    && get_sprites().get_current_frame() >= 3;	// wait until the animation shows an appropriate frame
}

