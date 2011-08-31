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
#include "hero/CarryingState.h"
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
Hero::StairsState::StairsState(Hero& hero, Stairs& stairs, Stairs::Way way):
  State(hero),
  stairs(stairs),
  way(way),
  phase(0),
  next_phase_date(0),
  carried_item(NULL) {

}

/**
 * @brief Destructor.
 */
Hero::StairsState::~StairsState() {

}

/**
 * @brief Changes the map.
 * @param map the new map
 */
void Hero::StairsState::set_map(Map& map) {

  State::set_map(map);

  // the hero may go to another map while taking stairs and carrying an item
  if (carried_item != NULL) {
    carried_item->set_map(map);
  }
}

/**
 * @brief Starts this state.
 * @param previous_state the previous state
 */
void Hero::StairsState::start(State* previous_state) {

  State::start(previous_state);

  // movement
  int speed = stairs.is_inside_floor() ? 40 : 24;
  std::string path = stairs.get_path(way);
  PathMovement* movement = new PathMovement(path, speed, false, true, false);

  // sprites and sound
  HeroSprites& sprites = get_sprites();
  if (carried_item == NULL) {
    sprites.set_animation_walking_normal();
  }
  else {
    sprites.set_lifted_item(carried_item);
    sprites.set_animation_walking_carrying();
  }
  sprites.set_animation_direction((path[0] - '0') / 2);
  get_keys_effect().set_action_key_effect(KeysEffect::ACTION_KEY_NONE);

  if (stairs.is_inside_floor()) {
    if (way == Stairs::NORMAL_WAY) {
      // low layer to intermediate layer: change the layer now
      get_entities().set_entity_layer(&hero, LAYER_INTERMEDIATE);
    }
  }
  else {
    sprites.set_clipping_rectangle(stairs.get_clipping_rectangle(way));
    if (way == Stairs::REVERSE_WAY) {
      Rectangle dxy = movement->get_xy_change();
      int fix_y = 8;
      if (path[path.size() - 1] == '2') {
        fix_y *= -1;
      }
      hero.set_xy(hero.get_x() - dxy.get_x(), hero.get_y() - dxy.get_y() + fix_y);
    }
  }
  hero.set_movement(movement);
}

/**
 * @brief Stops this state.
 * @param next_state the next state
 */
void Hero::StairsState::stop(State* next_state) {

  State::stop(next_state);

  if (carried_item != NULL) {

    switch (next_state->get_previous_carried_item_behavior(*carried_item)) {

    case CarriedItem::BEHAVIOR_THROW:
      carried_item->throw_item(get_sprites().get_animation_direction());
      get_entities().add_entity(carried_item);
      carried_item = NULL;
      break;

    case CarriedItem::BEHAVIOR_DESTROY:
      delete carried_item;
      carried_item = NULL;
      get_sprites().set_lifted_item(NULL);
      break;

    case CarriedItem::BEHAVIOR_KEEP:
      carried_item = NULL;
      break;

    default:
      Debug::die("Invalid carried item behavior");
    }
  }
}

/**
 * @brief Updates this state.
 */
void Hero::StairsState::update() {

  State::update();

  if (suspended) {
    return;
  }

  // first time: we play the sound and initialize
  if (phase == 0) {
    stairs.play_sound(way);
    next_phase_date = System::now() + 450;
    phase++;
  }

  // update the carried item if any
  if (carried_item != NULL) {
    carried_item->update();
  }

  if (stairs.is_inside_floor()) {

    // inside a single floor: return to normal state as soon as the movement is finished
    if (hero.get_movement()->is_finished()) {

      if (way == Stairs::REVERSE_WAY) {
        get_entities().set_entity_layer(&hero, LAYER_LOW);
      }
      hero.clear_movement();
      if (carried_item == NULL) {
        hero.set_state(new FreeState(hero));
      }
      else {
        hero.set_state(new CarryingState(hero, carried_item));
      }
    }
  }
  else {
    // stairs between two different floors: more complicated

    HeroSprites &sprites = get_sprites();
    if (hero.get_movement()->is_finished()) {
      hero.clear_movement();

      if (carried_item == NULL) {
        hero.set_state(new FreeState(hero));
      }
      else {
        hero.set_state(new CarryingState(hero, carried_item));
      }

      if (way == Stairs::NORMAL_WAY) {
        // we are on the old floor:
        // there must be a teletransporter associated with these stairs,
        // otherwise the hero would get stuck into the walls
        Teletransporter *teletransporter = hero.get_delayed_teletransporter();
        Debug::check_assertion(teletransporter != NULL, "Teletransporter expected with the stairs");
        teletransporter->transport_hero(hero);
      }
      else {
        // we are on the new floor: everything is finished
        sprites.set_clipping_rectangle();
      }
    }
    else { // movement not finished yet

      uint32_t now = System::now();
      if (now >= next_phase_date) {
        phase++;
        next_phase_date += 350;

        // main movement direction corresponding to each animation direction while taking stairs
        static const int movement_directions[] = { 0, 0, 2, 4, 4, 4, 6, 0 };

        int animation_direction = stairs.get_animation_direction(way);
        if (phase == 2) { // the first phase of the movement is finished
          if (animation_direction % 2 != 0) {
            // if the stairs are spiral, take a diagonal direction of animation
            sprites.set_animation_walking_diagonal(animation_direction);
          }
          else {
            // otherwise, take a usual direction
            sprites.set_animation_direction(animation_direction / 2);
            sprites.set_animation_walking_normal();
          }
        }
        else if (phase == 3) { // the second phase of the movement (possibly diagonal) is finished
          sprites.set_animation_walking_normal();

          if (way == Stairs::NORMAL_WAY) {
            // on the old floor, take a direction towards the next floor
            sprites.set_animation_direction(movement_directions[animation_direction] / 2);
          }
          else {
            // on the new floor, take the opposite direction from the stairs
            sprites.set_animation_direction((stairs.get_direction() + 2) % 4);
          }
        }
      }
    }
  }
}

/**
 * @brief Notifies this state that the game was just suspended or resumed.
 * @param suspended true if the game is suspended
 */
void Hero::StairsState::set_suspended(bool suspended) {

  State::set_suspended(suspended);

  if (carried_item != NULL) {
    carried_item->set_suspended(suspended);
  }

  if (!suspended) {
    next_phase_date += System::now() - when_suspended;
  }
}

/**
 * @brief Returns whether the hero's current position can be considered
 * as a place to come back after a bad ground (hole, deep water, etc).
 * @return true if the hero can come back here
 */
bool Hero::StairsState::can_come_from_bad_ground() {
  return false;
}

/**
 * @brief Returns whether the effect of teletransporters is delayed in this state.
 *
 * When overlapping a teletransporter, if this function returns true, the teletransporter
 * will not be activated immediately. The state then has to activate it when it is ready.
 *
 * @return true if the effect of teletransporters is delayed in this state
 */
bool Hero::StairsState::is_teletransporter_delayed() {
  return true;
}

/**
 * @brief Returns the direction of the hero's movement as defined by the controls applied by the player
 * and the movements allowed is the current state.
 *
 * If he is not moving, -1 is returned.
 * This direction may be different from the real movement direction because of obstacles.
 *
 * @return the hero's wanted direction between 0 and 7, or -1 if he is stopped
 */
int Hero::StairsState::get_wanted_movement_direction8() {
  return get_sprites().get_animation_direction8();
}

/**
 * @brief Returns the item currently carried by the hero in this state, if any.
 * @return the item carried by the hero, or NULL
 */
CarriedItem* Hero::StairsState::get_carried_item() {
  return carried_item;
}

/**
 * @brief Returns the action to do with an item previously carried by the hero when this state starts.
 * @param carried_item the item carried in the previous state
 * @return the action to do with a previous carried item when this state starts
 */
CarriedItem::Behavior Hero::StairsState::get_previous_carried_item_behavior(CarriedItem& carried_item) {

  if (stairs.is_inside_floor()) {
    this->carried_item = &carried_item;
    return CarriedItem::BEHAVIOR_KEEP;
  }
  return CarriedItem::BEHAVIOR_DESTROY;
}

/**
 * @brief Notifies this state that the layer has changed.
 */
void Hero::StairsState::notify_layer_changed() {

  if (carried_item != NULL) {
    carried_item->set_layer(hero.get_layer());
  }
}

