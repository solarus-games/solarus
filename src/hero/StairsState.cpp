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
#include "hero/StairsState.h"
#include "hero/FreeState.h"
#include "hero/HeroSprites.h"
#include "entities/MapEntities.h"
#include "entities/Teletransporter.h"
#include "movements/PathMovement.h"
#include "lowlevel/Rectangle.h"
#include "lowlevel/System.h"
#include "Game.h"
#include "Map.h"
#include "KeysEffect.h"
#include "lowlevel/Debug.h"

/**
 * @brief Constructor.
 * @param hero the hero controlled by this state
 * @param stairs the stairs to take
 * @param way the way you are taking the stairs
 */
Hero::StairsState::StairsState(Hero *hero, Stairs *stairs, Stairs::Way way):
  State(hero), stairs(stairs), way(way), phase(0), next_phase_date(0) {

}

/**
 * @brief Destructor.
 */
Hero::StairsState::~StairsState(void) {

}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::StairsState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (!suspended) {
    next_phase_date += System::now() - when_suspended;
  }
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::StairsState::start(State *previous_state) {

  State::start(previous_state);

  // movement
  int speed = stairs->is_inside_floor() ? 4 : 2;
  std::string path = stairs->get_path(way);
  PathMovement *movement = new PathMovement(path, speed, false, true, false);

  // sprites and sound
  sprites->set_animation_walking_normal();
  sprites->set_animation_direction((path[0] - '0') / 2);
  game->get_keys_effect()->set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (stairs->is_inside_floor()) {
    if (way == Stairs::NORMAL_WAY) {
      // low layer to intermediate layer: change the layer now
      map->get_entities()->set_entity_layer(hero, LAYER_INTERMEDIATE);
    }
  }
  else {
    sprites->set_clipping_rectangle(stairs->get_clipping_rectangle(way));
    if (way == Stairs::REVERSE_WAY) {
      Rectangle dxy = movement->get_xy_change();
      int fix_y = 8;
      if (path[path.size() - 1] == '2') {
        fix_y *= -1;
      }
      hero->set_xy(hero->get_x() - dxy.get_x(), hero->get_y() - dxy.get_y() + fix_y);
    }
  }
  hero->set_movement(movement);
}

/**
 * @brief Updates this state.
 */
void Hero::StairsState::update(void) {

  State::update();

  if (suspended) {
    return;
  }

  // first time: we play the sound and initialize
  if (phase == 0) {
    stairs->play_sound(way);
    next_phase_date = System::now() + 450;
    phase++;
  }

  if (stairs->is_inside_floor()) {

    // inside a single floor: return to normal state as soon as the movement is finished
    if (hero->get_movement()->is_finished()) {

      if (way == Stairs::REVERSE_WAY) {
	map->get_entities()->set_entity_layer(hero, LAYER_LOW);
      }
      hero->clear_movement();
      hero->set_state(new FreeState(hero));
    }
  }
  else {
    // stairs between two different floors: more complicated

    if (hero->get_movement()->is_finished()) {
      hero->clear_movement();
      hero->set_state(new FreeState(hero));

      if (way == Stairs::NORMAL_WAY) {
	// we are on the old floor:
	// there must be a teletransporter associated with these stairs,
	// otherwise the hero would get stuck into the walls
	Teletransporter *teletransporter = hero->get_delayed_teletransporter();
	Debug::assert(teletransporter != NULL, "Teletransporter expected with the stairs");
	teletransporter->transport_hero(hero);
      }
      else {
	// we are on the new floor: everything is finished
	sprites->set_clipping_rectangle();
	hero->set_state(new FreeState(hero));
      }
    }
    else { // movement not finished yet

      uint32_t now = System::now();
      if (now >= next_phase_date) {
	phase++;
	next_phase_date += 350;

	// main movement direction corresponding to each animation direction while taking stairs
	static const int movement_directions[] = { 0, 0, 2, 4, 4, 4, 6, 0 };

	int animation_direction = stairs->get_animation_direction(way);
	if (phase == 2) { // the first phase of the movement is finished
	  if (animation_direction % 2 != 0) {
	    // if the stairs are spiral, take a diagonal direction of animation
	    sprites->set_animation_walking_diagonal(animation_direction);
	  }
	  else {
	    // otherwise, take a usual direction
	    sprites->set_animation_direction(animation_direction / 2);
	    sprites->set_animation_walking_normal();
	  }
	}
	else if (phase == 3) { // the second phase of the movement (possibly diagonal) is finished
	  sprites->set_animation_walking_normal();

	  if (way == Stairs::NORMAL_WAY) {
	    // on the old floor, take a direction towards the next floor
	    sprites->set_animation_direction(movement_directions[animation_direction] / 2);
	  }
	  else {
	    // on the new floor, take the opposite direction from the stairs
	    sprites->set_animation_direction((stairs->get_direction() + 2) % 4);
	  }
	}
      }
    }
  }
}

/**
 * @brief Returns whether the effect of teletransporters is delayed in this state.
 *
 * When overlapping a teletransporter, if this function returns true, the teletransporter
 * will not be activated immediately. The state then has to activate it when it is ready.
 */
bool Hero::StairsState::is_teletransporter_delayed(void) {
  return true;
}

